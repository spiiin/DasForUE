// Runtime subscriptions are lowered before hidden callback discovery and type inference.
void DSLNormalizeDynamicEvents(const das::StructurePtr& St)
{
    struct FRewrite : das::Visitor
    {
        das::StructurePtr Class;
        das::string Self;
        FRewrite(const das::StructurePtr& C, const das::string& S) : Class(C), Self(S) {}
        bool IsSelf(das::Expression* E) { return E && E->rtti_isVar() && static_cast<das::ExprVar*>(E)->name == Self; }
        virtual das::ExpressionPtr visit(das::ExprOp2* E) override
        {
            if ((E->op != "+=" && E->op != "-=") || !E->left->rtti_isField() || !E->right->rtti_isField())
                return E;
            auto* Handler = static_cast<das::ExprField*>(E->right);
            if (!IsSelf(Handler->value))
                return E;
            // Keep ordinary arithmetic on declared data fields in the normal lowering path.
            const auto* Member = Class->findField(Handler->name);
            if (Member && !Member->classMethod)
                return E;
            auto* Event = static_cast<das::ExprField*>(E->left);
            das::string Source = "self";
            if (!IsSelf(Event->value))
            {
                if (!Event->value->rtti_isField())
                    return E;
                auto* Component = static_cast<das::ExprField*>(Event->value);
                if (!IsSelf(Component->value))
                    return E;
                Source = Component->name;
            }
            return DSLCall(E->at, "__ue_dynamic_event",
                           {new das::ExprConstString(E->at, Source), new das::ExprConstString(E->at, Event->name),
                            Handler, new das::ExprConstBool(E->at, E->op == "+=")});
        }
    };
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St && F->body && !F->arguments.empty() && DSLMethodName(F) != "BindEvents")
            {
                FRewrite Rewrite(St, F->arguments[0]->name);
                F->body = F->body->visit(Rewrite);
            }
        });
}

// A declarative block shares bind_events' candidate validation and lifecycle cleanup.
void DSLNormalizeEventDeclarations(const das::StructurePtr& St, das::ModuleGroup& Group)
{
    das::vector<das::FunctionPtr> Functions;
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St)
                Functions.push_back(F);
        });
    for (const auto& F : Functions)
    {
        if (DSLMethodName(F) != "BindEvents")
            continue;
        if (F->isStaticClassMethod || F->arguments.size() != 1 || !F->body || !F->body->rtti_isBlock() || !F->result ||
            (F->result->baseType != das::Type::autoinfer && F->result->baseType != das::Type::tVoid) ||
            !F->annotations.empty())
            DSLFail(TEXT("BindEvents must be an unannotated instance declaration block without parameters/result"));
        const auto Self = F->arguments[0]->name;
        auto IsSelf = [&](das::Expression* E)
        {
            return E && E->rtti_isVar() && static_cast<das::ExprVar*>(E)->name == Self;
        };
        auto* Body = static_cast<das::ExprBlock*>(F->body);
        // daScript wraps instance methods in a generated block/with self.
        if (Body->generated && Body->list.size() == 1 && Body->list[0]->generated && Body->list[0]->rtti_isWith())
        {
            auto* With = static_cast<das::ExprWith*>(Body->list[0]);
            if (!IsSelf(With->with) || !With->body->rtti_isBlock())
                DSLFail(TEXT("Unexpected BindEvents method wrapper"));
            Body = static_cast<das::ExprBlock*>(With->body);
        }
        for (auto* E : Body->list)
        {
            if (!E->rtti_isOp2())
                DSLFail(TEXT("BindEvents accepts only source.Event += self.Handler declarations"));
            auto* Op = static_cast<das::ExprOp2*>(E);
            if (Op->op != "+=" || !Op->left->rtti_isField() || !Op->right->rtti_isField())
                DSLFail(TEXT("Expected self.Component.Event += self.Handler in BindEvents"));
            auto* Event = static_cast<das::ExprField*>(Op->left);
            auto* Handler = static_cast<das::ExprField*>(Op->right);
            if (!IsSelf(Handler->value))
                DSLFail(TEXT("Event handler must be self.Method"));
            das::string Source = "self";
            if (!IsSelf(Event->value))
            {
                if (!Event->value->rtti_isField())
                    DSLFail(TEXT("Event source must be self or a declared component"));
                auto* Field = static_cast<das::ExprField*>(Event->value);
                if (!IsSelf(Field->value))
                    DSLFail(TEXT("Event source must be self or a declared component"));
                Source = Field->name;
            }
            das::FunctionPtr Target = nullptr;
            for (const auto& Candidate : Functions)
                if (DSLMethodName(Candidate) == Handler->name)
                {
                    if (Target)
                        DSLFail(TEXT("Ambiguous event handler"));
                    Target = Candidate;
                }
            if (!Target)
                DSLFail(TEXT("Unknown event handler self.")+ DSLText(Handler->name));
            if (!Target->annotations.empty())
                DSLFail(TEXT("BindEvents handler must be unannotated and bound to one source"));
            auto Types = Group.findAnnotation("ue_event", St->module);
            if (Types.size() != 1)
                DSLFail(TEXT("Missing ue_event annotation"));
            auto* A = new das::AnnotationDeclaration();
            A->flags = 0;
            A->annotation = Types[0];
            A->at = E->at;
            A->arguments.push_back(das::AnnotationArgument("source", Source));
            A->arguments.push_back(das::AnnotationArgument("name", Event->name));
            Target->annotations.push_back(A);
        }
        auto* Empty = new das::ExprBlock();
        Empty->at = F->at;
        F->body = Empty;
        F->result = new das::TypeDecl(das::Type::tVoid, F->at);
    }
}
das::TypeDeclPtr DSLEventOwnerType(const das::AnnotationArgumentList& Options, const das::FunctionPtr& F,
                                   const das::StructurePtr& St, das::ModuleGroup& Group)
{
    const auto Hint = DSLString(Options, "source_type");
    if (Hint.IsEmpty())
        DSLFail(TEXT("Owner event requires source_type=OwnerRef for compile-time validation"));
    auto* Alias = new das::TypeDecl(das::Type::alias, F->at);
    Alias->alias = DSLUtf8(Hint);
    auto T = DSLObjectNormalize(Alias, Group, St->module);
    if (!DSLTypedReference(T) || DSLTypedInterface(T))
        DSLFail(TEXT("source_type must name a concrete Unreal object reference"));
    return T;
}
// Validate event sources from declarations before any Blueprint or runtime object is modified.
void DSLValidateEventSignature(const FDasDeclaredClass& C, const FDasDeclaredCallback& Callback,
                               const das::FunctionPtr& F, const das::StructurePtr& St, das::ModuleGroup& Group,
                               const FString& Folder, const FString& Registry,
                               const das::AnnotationArgumentList& Options, bool Exact = false)
{
    TArray<FDasDeclaredType> Arguments;
    for (size_t I = 1; I < F->arguments.size(); ++I)
    {
        auto T = DSLCallbackField(F->arguments[I]->type, Folder, Registry);
        if (DSLAuthoredView(F->arguments[I]->type))
            T.AuthoredId = DSLReferenceDeclaration(F->arguments[I]->type).Id;
        Arguments.Add(T);
    }
    auto SignatureFrom = [&](const FDasDeclaredClass& Source) -> UFunction*
    {
        const auto* P = Source.Properties.FindByPredicate([&](const auto& P) { return P.Name == Callback.EventName; });
        if (!P)
            return nullptr;
        if (P->Type.Kind != TEXT("event") || P->DelegateWire.IsEmpty())
            DSLFail(TEXT("Event source property is not a multicast event: ")+ Callback.EventName);
        FDasAuthoringModule Wire;
        FString Error;
        if (!FDasAuthoringModule::FromJson(P->DelegateWire, Wire, Error) || Wire.Classes.Num() != 1 ||
            Wire.Classes[0].Methods.Num() != 1)
            DSLFail(Error);
        const auto& Method = Wire.Classes[0].Methods[0];
        for (int32 I = 0; I < Arguments.Num() && I < Method.Parameters.Num(); ++I)
            if (!Arguments[I].AuthoredId.IsEmpty() && Arguments[I].AuthoredId != Method.Parameters[I].Type.AuthoredId)
                DSLFail(TEXT("Authored event argument identity differs: ")+ Callback.EventName);
        auto* Sig = MakeDasDelegateSignature(Method, true, GetTransientPackage(), NAME_None, Error);
        if (!Sig)
            DSLFail(Error);
        return Sig;
    };
    UClass* Native = nullptr;
    TStrongObjectPtr<UFunction> Owned;
    UFunction* Signature = nullptr;
    if (Callback.Source == TEXT("self"))
    {
        Signature = SignatureFrom(C);
        Native = DasNativeScriptHost(C.NativeParent);
    }
    else if (Callback.Source == TEXT("owner"))
    {
        auto T = DSLEventOwnerType(Options, F, St, Group);
        if (DSLAuthoredView(T))
        {
            auto Decl = DSLReferenceDeclaration(T);
            Signature = SignatureFrom(Decl);
            Native = DasNativeScriptHost(Decl.NativeParent);
        }
        else
            Native = FindObject<UClass>(nullptr, *DSLTypedReferencePath(T));
    }
    else
    {
        const auto* Component = C.Components.FindByPredicate([&](const auto& X) { return X.Name == Callback.Source; });
        if (!Component)
            DSLFail(TEXT("Unknown event component: ")+ Callback.Source);
        if (!Component->NativeClass.IsEmpty())
            Native = FindObject<UClass>(nullptr, *Component->NativeClass);
        else
        {
            auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
            FDasAuthoringModule Model;
            FDasDeclaredClass Decl;
            FString Error;
            const FString Key = Component->ScriptFile + TEXT("::")+ Component->ClassName;
            if (const auto* Active = DSLActiveRefDeclarations.Find(Key))
                Decl = *Active;
            else
            {
                if (DSLRefSourceStack.Contains(Key))
                    DSLFail(TEXT("Authored event source dependency cycle: ")+ Key);
                DSLRefSourceStack.Add(Key);
                ON_SCOPE_EXIT
                {
                    DSLRefSourceStack.Remove(Key);
                };
                if (!R.ReadAuthoringScript(R.ScriptPath(Component->ScriptFile), Model, Error))
                    DSLFail(Error);
                const auto* Found =
                    Model.Classes.FindByPredicate([&](const auto& X) { return X.Name == Component->ClassName; });
                if (!Found || !Model.ResolveClass(Found->Id, Decl, Error))
                    DSLFail(TEXT("Unknown authored event component: ")+ Component->ClassName);
            }
            Signature = SignatureFrom(Decl);
            Native = DasNativeScriptHost(Decl.NativeParent);
        }
    }
    if (Signature)
        Owned.Reset(Signature);
    else
    {
        auto* P = Native ? FindFProperty<FMulticastDelegateProperty>(Native, *Callback.EventName) : nullptr;
        if (!P || !P->HasAnyPropertyFlags(CPF_BlueprintAssignable))
            DSLFail(TEXT("Unknown BlueprintAssignable event ")+ Callback.Source + TEXT(".")+ Callback.EventName);
        Signature = P->SignatureFunction;
    }
    FString Error;
    if (Exact)
    {
        const auto* Method =
            C.Methods.FindByPredicate([&](const auto& M) { return M.Name == DSLText(DSLMethodName(F)); });
        TStrongObjectPtr<UFunction> Handler(
            Method ? MakeDasDelegateSignature(*Method, true, GetTransientPackage(), NAME_None, Error) : nullptr);
        if (!Handler.IsValid() || !DelegateFrameMatches(Signature, Handler.Get()))
            DSLFail(TEXT("Dynamic event requires exactly matching handler argument types: ") + Callback.Source +
                         TEXT(".") + Callback.EventName);
    }
    if (!ValidateDasCallbackSignature(Signature, Arguments, Error))
        DSLFail(TEXT("ue_event ")+ Callback.Source + TEXT(".")+ Callback.EventName + TEXT(": ")+ Error);
}

void DSLValidateDynamicEvent(das::ExprCall* E, const FDasDeclaredClass& C, const das::StructurePtr& St,
                             das::ModuleGroup& Group, const FString& Folder, const FString& Registry)
{
    if (E->arguments.size() != 4 || E->arguments[0]->__rtti != das::string("ExprConstString") ||
        E->arguments[1]->__rtti != das::string("ExprConstString") ||
        E->arguments[2]->__rtti != das::string("ExprConstString"))
        DSLFail(TEXT("Dynamic event requires self.Event +=/-= self.Method"));
    FDasDeclaredCallback Callback;
    Callback.Source = DSLText(static_cast<das::ExprConstString*>(E->arguments[0])->text);
    Callback.EventName = DSLText(static_cast<das::ExprConstString*>(E->arguments[1])->text);
    const auto Name = static_cast<das::ExprConstString*>(E->arguments[2])->text;
    auto Handler = DSLFindHandlerMethod(St, DSLText(Name));
    if (!Handler)
        DSLFail(TEXT("Unknown dynamic event handler: ") + DSLText(Name));
    DSLValidateEventSignature(C, Callback, Handler, St, Group, Folder, Registry, {}, true);
}
