// Internal evidence for the exact native contract of a generated free-handler ABI.
struct FUEHandlerContractAnnotation : das::FunctionAnnotation
{
    FUEHandlerContractAnnotation() : FunctionAnnotation("__ue_handler_contract") {}
    virtual bool apply(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList&,
                       das::string& Error) override
    {
        if (!F->generated)
        {
            Error = "Internal annotation requires a generated handler";
            return false;
        }
        return true;
    }
    virtual bool finalize(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
};
// One site description covers parsed method calls and already lowered calls.
int DSLDelegateOperation(const das::string& Name)
{
    const auto Sep = Name.rfind("::");
    const auto N = Sep == das::string::npos ? Name : Name.substr(Sep + 2);
    return N == "BindUFunction" ? 1 : N == "AddUFunction" ? 2 : N == "RemoveUFunction" ? 3 : -1;
}
das::ExprCallMacro* DSLDelegateInvokeReference(das::ExprInvoke* E, const das::TypeDeclPtr& Owner = nullptr)
{
    if (!E->isInvokeMethod || E->arguments.size() != 4 || !E->arguments[0]->rtti_isField())
        return nullptr;
    const int Op = DSLDelegateOperation(static_cast<das::ExprField*>(E->arguments[0])->name);
    if (Op < 0)
        return nullptr;
    // Before inference, the implicit receiver slot is still type<auto>.
    auto Receiver =
        E->arguments[1]->rtti_isTypeDecl() ? static_cast<das::ExprField*>(E->arguments[0])->value : E->arguments[1];
    if (Receiver->rtti_isCast())
    {
        auto* Cast = static_cast<das::ExprCast*>(Receiver);
        if (Cast->castType && Cast->castType->baseType == das::Type::autoinfer && Cast->subexpr->rtti_isPtr2Ref())
        {
            auto* Deref = static_cast<das::ExprPtr2Ref*>(Cast->subexpr);
            if (Deref->alwaysSafe)
                Receiver = Deref->subexpr;
        }
    }
    return DSLHandlerCall(E->at,
                          {Receiver, E->arguments[2], E->arguments[3],
                           new das::ExprTypeDecl(E->at, Owner ? Owner : new das::TypeDecl(das::Type::tVoid, E->at)),
                           new das::ExprConstInt(E->at, Op)});
}
void DSLNormalizeDelegateReferences(const das::StructurePtr& St, das::ModuleGroup& Group)
{
    TSet<FString> Names;
    struct FRewrite : das::Visitor
    {
        das::StructurePtr Class;
        das::string Self;
        TSet<FString>& Names;
        FRewrite(const das::StructurePtr& C, const das::string& S, TSet<FString>& N) : Class(C), Self(S), Names(N) {}
        bool Convert(das::ExpressionPtr& Arg)
        {
            if (!Arg || !Arg->rtti_isField())
                return false;
            auto* F = static_cast<das::ExprField*>(Arg);
            if (!F->value->rtti_isVar() || static_cast<das::ExprVar*>(F->value)->name != Self)
                return false;
            const auto* Member = Class->findField(F->name);
            if (Member && !Member->classMethod)
                return false; // Existing dynamic string/FName fields remain values.
            Names.Add(DSLText(F->name));
            Arg = new das::ExprConstString(F->at, F->name);
            return true;
        }
        das::TypeDeclPtr Owner()
        {
            auto* T = new das::TypeDecl(das::Type::tStructure, Class->at);
            T->structType = Class;
            return T;
        }
        virtual das::ExpressionPtr visit(das::ExprCallMacro* E) override
        {
            if (E->name != "ue_bind" || E->arguments.size() != 3 || !Convert(E->arguments[2]))
                return E;
            return DSLHandlerCall(E->at, {E->arguments[0], E->arguments[1], E->arguments[2],
                                          new das::ExprTypeDecl(E->at, Owner()), new das::ExprConstInt(E->at, 0)});
        }
        virtual das::ExpressionPtr visit(das::ExprCall* E) override
        {
            const int Op = DSLDelegateOperation(E->name);
            if (Op < 0 || E->arguments.size() != 3 || !Convert(E->arguments[2]))
                return E;
            return DSLHandlerCall(E->at, {E->arguments[0], E->arguments[1], E->arguments[2],
                                          new das::ExprTypeDecl(E->at, Owner()), new das::ExprConstInt(E->at, Op)});
        }
        virtual das::ExpressionPtr visit(das::ExprInvoke* E) override
        {
            if (E->arguments.size() != 4 || !E->arguments[0]->rtti_isField() ||
                DSLDelegateOperation(static_cast<das::ExprField*>(E->arguments[0])->name) < 0 ||
                !Convert(E->arguments[3]))
                return E;
            return DSLDelegateInvokeReference(E, Owner());
        }
    };
    das::vector<das::FunctionPtr> Functions;
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St && F->body && !F->arguments.empty())
                Functions.push_back(F);
        });
    for (const auto& F : Functions)
    {
        FRewrite Rewrite(St, F->arguments[0]->name, Names);
        F->body = F->body->visit(Rewrite);
    }
    for (const auto& Name : Names)
    {
        das::FunctionPtr Handler = nullptr;
        for (const auto& F : Functions)
            if (DSLText(DSLMethodName(F)) == Name)
            {
                if (Handler)
                    DSLFail(TEXT("Ambiguous delegate handler: ")+ Name);
                Handler = F;
            }
        if (!Handler)
            DSLFail(TEXT("Unknown delegate handler self.")+ Name);
        DSLHiddenCallback(Handler, St, Group, false);
    }
}
FDasDeclaredMethod DSLHandlerMethod(das::Function* F, das::ModuleGroup& G)
{
    if (!F || F->classParent || F->builtIn || F->isStaticClassMethod || F->arguments.size() > 16)
        DSLFail(TEXT("@@handler must refer to a unique non-generic free script function"));
    FDasDeclaredMethod M;
    M.Name = TEXT("Invoke");
    M.Id = TEXT("delegate.contract.Invoke");
    M.ScriptFile = TEXT("delegate.das");
    M.EntryPoint = TEXT("invoke");
    M.ReturnType = DSLDelegateDeclaredType(DSLObjectNormalize(F->result, G, F->module), {}, {});
    for (const auto& A : F->arguments)
    {
        if (A->init)
            DSLFail(TEXT("Handler parameters cannot have defaults"));
        FDasDeclaredParameter P;
        P.Name = DSLText(A->name);
        P.Id = M.Id + TEXT(".")+ P.Name;
        P.Direction = DSLParameterDirection(A, FString());
        auto T = DSLCopyType(DSLObjectNormalize(A->type, G, F->module));
        T->ref = false;
        T->constant = false;
        P.Type = DSLDelegateDeclaredType(T, {}, {});
        M.Parameters.Add(P);
    }
    return M;
}
void DSLCheckHandlerContract(const FDasDeclaredMethod& Expected, const FDasDeclaredMethod& Actual, bool Multi)
{
    if (Expected.Parameters.Num() != Actual.Parameters.Num() || Expected.ReturnType.Kind != Actual.ReturnType.Kind)
        DSLFail(TEXT("Delegate handler signature mismatch"));
    for (int32 I = 0; I < Expected.Parameters.Num(); ++I)
        if (Expected.Parameters[I].Direction != Actual.Parameters[I].Direction)
            DSLFail(TEXT("Delegate handler signature mismatch: ref/out direction differs"));
    FString Error;
    TStrongObjectPtr<UFunction> A(MakeDasDelegateSignature(Expected, Multi, GetTransientPackage(), NAME_None, Error));
    TStrongObjectPtr<UFunction> B(MakeDasDelegateSignature(Actual, Multi, GetTransientPackage(), NAME_None, Error));
    if (!A.IsValid() || !B.IsValid() || !DelegateFrameMatches(A.Get(), B.Get()))
        DSLFail(TEXT("Delegate handler signature mismatch: exact parameter, result and ref/out types required. ")+
                     Error);
}
das::ExpressionPtr DSLDelegateReferenceExpansion(das::Program* P, das::Module* Module, das::ExprCallMacro* E)
{
    if (E->arguments.size() != 5 || !E->arguments[3]->rtti_isTypeDecl() ||
        E->arguments[4]->__rtti != das::string("ExprConstInt"))
        DSLFail(TEXT("Invalid delegate reference syntax"));
    const int Op = das::cast<int32_t>::to(static_cast<das::ExprConstInt*>(E->arguments[4])->value);
    auto T = E->arguments[0]->rtti_isTypeDecl() ? static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr
                                                : E->arguments[0]->type;
    if (!T)
        return nullptr;
    T = DSLObjectNormalize(T, *P->thisModuleGroup, Module);
    if (!DSLDelegate(T))
        DSLFail(TEXT("Handler reference requires a delegate receiver/type"));
    const auto* Annotation = DSLDelegateAnnotation(T);
    const bool Multi = Annotation->annotation->name == "ue_multicast";
    if ((Op < 2 && Multi) || (Op >= 2 && !Multi))
        DSLFail(TEXT("Delegate operation does not match single/multicast type"));
    const FString Wire = DSLString(Annotation->arguments, "_ue_delegate_wire");
    FDasAuthoringModule Model;
    FString Error;
    if (!FDasAuthoringModule::FromJson(Wire, Model, Error))
        DSLFail(Error);
    const auto& Method = Model.Classes[0].Methods[0];
    const bool Free = E->arguments[2]->rtti_isAddr();
    das::string Name;
    if (Free)
    {
        auto* Address = static_cast<das::ExprAddr*>(E->arguments[2]);
        if (!Address->func)
            return nullptr;
        auto* F = Address->func;
        const auto Actual = DSLHandlerMethod(F, *P->thisModuleGroup);
        DSLCheckHandlerContract(Method, Actual, Multi);
        const FString Identity = DSLText(F->getMangledName()) + TEXT(":")+ Wire;
        const auto Utf8 = DSLUtf8(Identity);
        Name =
            DSLUtf8(TEXT("__ue_handler_")+ FMD5::HashBytes(reinterpret_cast<const uint8*>(Utf8.data()), Utf8.size()));
        if (!Module->findUniqueFunction(Name))
        {
            das::FunctionPtr Source = F;
            auto Prototype = F->clone();
            Prototype->name = Name;
            Prototype->classParent = nullptr;
            Prototype->exports = true;
            Prototype->generated = true;
            Prototype->privateFunction = false;
            Prototype->annotations.clear();
            auto Wrapper =
                DSLHandlerABI(Source, Prototype, F->module == Module ? F->name : F->module->name + "::" + F->name,
                              Method, 0, *P->thisModuleGroup, Module, {}, {});
            auto Annotations = P->thisModuleGroup->findAnnotation("__ue_handler_contract", Module);
            if (Annotations.size() != 1)
                DSLFail(TEXT("Missing generated handler contract annotation"));
            auto* Evidence = new das::AnnotationDeclaration();
            Evidence->flags = 0;
            Evidence->annotation = Annotations[0];
            Evidence->at = E->at;
            Evidence->arguments.push_back(das::AnnotationArgument("wire", DSLUtf8(Wire)));
            Wrapper->annotations.push_back(Evidence);
            if (!Module->addFunction(Wrapper, true))
                DSLFail(TEXT("Free handler wrapper identity collision"));
        }
    }
    else
    {
        if (E->arguments[2]->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Use self.Method or @@function as handler"));
        Name = static_cast<das::ExprConstString*>(E->arguments[2])->text;
        auto Owner = static_cast<das::ExprTypeDecl*>(E->arguments[3])->typeexpr;
        const auto* A =
            Owner && Owner->structType ? DSLAnnotation(Owner->structType->annotations, "ue_class") : nullptr;
        if (!A)
            DSLFail(TEXT("Missing handler class contract"));
        if (!CollectTypesOnly)
        {
            FDasAuthoringModule ClassModel;
            if (!FDasAuthoringModule::FromJson(DSLString(A->arguments, DSLModelKey), ClassModel, Error))
                DSLFail(Error);
            const auto* C = ClassModel.Classes.FindByPredicate([&](const auto& C)
                                                               { return C.Name == DSLText(Owner->structType->name); });
            const auto* Found =
                C ? C->Methods.FindByPredicate([&](const auto& M) { return M.Name == DSLText(Name); }) : nullptr;
            if (!Found)
                DSLFail(TEXT("Unknown reflected delegate handler: ")+ DSLText(Name));
            DSLCheckHandlerContract(Method, *Found, Multi);
        }
    }
    auto Target = DSLCall(E->at, "ue_delegate_target", {E->arguments[1]});
    if (!Free)
        Target = DSLCall(E->at, "__ue_delegate_self_target", {Target});
    if (Op != 0)
    {
        const das::string Function = Free      ? (Op == 3 ? "__ue_remove_script_value" : "__ue_bind_script_value")
                                     : Op == 1 ? "BindUFunction"
                                     : Op == 2 ? "AddUFunction"
                                               : "RemoveUFunction";
        return DSLCall(E->at, Function, {E->arguments[0], Target, new das::ExprConstString(E->at, Name)});
    }
    auto* V = new das::ExprMakeStruct(E->at);
    V->makeType = DSLCopyType(T);
    V->useInitializer = true;
    V->alwaysUseInitializer = true;
    auto* Fields = new das::MakeStruct();
    auto* Call = DSLCall(E->at, Free ? "__ue_delegate_bind_script" : "__ue_delegate_bind",
                         {new das::ExprConstUInt64(E->at, 0), new das::ExprConstString(E->at, DSLUtf8(Wire)),
                          new das::ExprConstBool(E->at, false), Target, new das::ExprConstString(E->at, Name)});
    if (Free)
        Call->arguments.push_back(new das::ExprConstBool(E->at, false));
    Fields->push_back(new das::MakeFieldDecl(E->at, "__delegate_token", Call, false, false));
    V->structs.push_back(Fields);
    return V;
}
struct FUEFunctionReferencePass : das::PassMacro
{
    FUEFunctionReferencePass() : PassMacro("ue_function_references") {}
    virtual bool apply(das::Program* P, das::Module* M) override
    {
        bool Changed = false;
        struct FRewrite : das::Visitor
        {
            das::Program* P;
            das::Module* M;
            bool& Changed;
            FRewrite(das::Program* P0, das::Module* M0, bool& C) : P(P0), M(M0), Changed(C) {}
            virtual das::ExpressionPtr visit(das::ExprInvoke* E) override
            {
                if (E->arguments.size() != 4 || !E->arguments[3]->rtti_isAddr())
                    return E;
                auto* Call = DSLDelegateInvokeReference(E);
                if (Call)
                {
                    Changed = true;
                    return Call;
                }
                return E;
            }
            virtual das::ExpressionPtr visit(das::ExprCall* E) override
            {
                if (E->arguments.size() == 3 && DSLDelegateOperation(E->name) > 0 && E->arguments[2]->rtti_isAddr())
                {
                    Changed = true;
                    return DSLHandlerCall(E->at,
                                          {E->arguments[0], E->arguments[1], E->arguments[2],
                                           new das::ExprTypeDecl(E->at, new das::TypeDecl(das::Type::tVoid, E->at)),
                                           new das::ExprConstInt(E->at, DSLDelegateOperation(E->name))});
                }
                int I = DSLCallbackArgument(E->name, E->arguments.size());
                if (I < 0 || !E->arguments[I]->rtti_isAddr())
                    return E;
                auto* A = static_cast<das::ExprAddr*>(E->arguments[I]);
                if (!A->func)
                    return E;
                try
                {
                    const auto Method = DSLHandlerMethod(A->func, *P->thisModuleGroup);
                    if (Method.ReturnType.Kind != TEXT("void"))
                        DSLFail(TEXT("Managed @@callback must return void"));
                    for (const auto& Param : Method.Parameters)
                        if (Param.Direction != TEXT("in"))
                            DSLFail(TEXT("Managed @@callback cannot have ref/out parameters"));
                    const auto N = DSLOperationName(E->name);
                    const bool Empty = N == "ue_input_bind_action" || N == "ue_widget_bind_click" ||
                                       (N == "ue_timer_start" && E->arguments.size() == 4) ||
                                       (N == "ue_timer_next_tick" && E->arguments.size() == 2) ||
                                       N == "ue_latent_delay";
                    if (Empty && !Method.Parameters.IsEmpty())
                        DSLFail(TEXT("Managed callback argument count mismatch"));
                    A->func->exports = true;
                    if (A->func->module != M)
                        DSLFail(TEXT("Managed @@callback must be declared in the current script module"));
                    E->arguments[I] = new das::ExprConstString(A->at, A->func->name);
                    Changed = true;
                }
                catch (const std::exception& X)
                {
                    P->error("Function handler reference failed", X.what(), "", E->at,
                             das::CompilationError::invalid_type);
                }
                return E;
            }
        } Rewrite(P, M, Changed);
        das::vector<das::FunctionPtr> Functions;
        M->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->body)
                    Functions.push_back(F);
            });
        for (const auto& F : Functions)
            F->body = F->body->visit(Rewrite);
        return Changed;
    }
};
