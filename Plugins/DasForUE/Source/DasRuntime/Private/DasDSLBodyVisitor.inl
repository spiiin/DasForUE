// Part of DasDSL.inl: Method body lowering: rewrites self/field access, calls and managed APIs inside authored methods.
// Included in place; declaration order matters.
// daScript self accessor for a declared property: <Prefix><kind>, with struct snapshots for
// structs and container carriers and name as the remaining reflected kind.
das::string DSLSelfAccessor(const char* Prefix, const FDasDeclaredType& Type)
{
    if (const char* Suffix = DasAccessorKindSuffix(Type.Kind))
        return das::string(Prefix) + Suffix;
    if (Type.Kind == TEXT("struct") || DasContainerCarrier(Type))
        return das::string(Prefix) + "struct";
    if (Type.Kind == TEXT("object"))
        return das::string(Prefix) + "object";
    return das::string(Prefix) + "name";
}
// Unqualified name of a class method function (after the last ` or ' separator).
das::string DSLMethodName(const das::FunctionPtr& F)
{
    return F->name.substr(F->name.find_last_of(das::string(1, char(96)) + das::string(1, char(39))) + 1);
}
// Index of the callback-method argument of managed APIs, or -1.
int32 DSLCallbackArgument(const das::string& QualifiedName, size_t Count)
{
    const auto Separator = QualifiedName.rfind("::");
    const das::string Name = Separator == das::string::npos ? QualifiedName : QualifiedName.substr(Separator + 2);
    int32 Index = -1;
    if (Name == "ue_timer_start" || Name == "ue_input_bind_action" || Name == "ue_enhanced_bind" ||
        Name == "ue_latent_delay" || Name == "ue_asset_load_async" || Name == "ue_soft_load_async" ||
        Name == "ue_widget_bind_click")
        Index = 2;
    else if (Name == "ue_timer_next_tick" || Name == "ue_input_bind_axis")
        Index = 1;
    else if (Name == "ue_sound_play" || Name == "ue_niagara_spawn" || Name == "ue_ai_move_to_actor" ||
             Name == "ue_ai_move_to_location" || Name == "ue_montage_play")
        Index = 3;
    else if (Name == "ue_effect_play" || Name == "ue_effect_at")
        Index = 4;
    return Index >= 0 && size_t(Index) < Count ? Index : -1;
}
// Native class of a ue_event source when it is known at compile time: self of a class with a native
// parent (its script host) or a native component. Owner and authored sources return null.
UClass* DSLEventSourceClass(const FDasDeclaredClass& C, const FString& Source)
{
    if (Source == TEXT("self"))
        return C.ParentId.IsEmpty() ? DasNativeScriptHost(C.NativeParent) : nullptr;
    if (Source == TEXT("owner"))
        return nullptr;
    const auto* Component = C.Components.FindByPredicate([&](const auto& X) { return X.Name == Source; });
    return Component && !Component->NativeClass.IsEmpty() ? FindObject<UClass>(nullptr, *Component->NativeClass)
                                                          : nullptr;
}
// A BlueprintAssignable multicast delegate with the given number of parameters.
bool DSLNativeEventMatches(UClass* Class, const FString& Name, int32 Arity)
{
    auto* P = FindFProperty<FMulticastDelegateProperty>(Class, FName(*Name));
    if (!P || !P->HasAnyPropertyFlags(CPF_BlueprintAssignable) || !P->SignatureFunction)
        return false;
    int32 Parameters = 0;
    for (TFieldIterator<FProperty> It(P->SignatureFunction); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))
            ++Parameters;
    return Parameters == Arity;
}
// Rewrites self.Method callback arguments in the declared class methods to the method name, so the
// methods themselves type-check against the name-based managed APIs. The generated entry points
// are lowered by FDSLBodyVisitor, which accepts both forms.
struct FDSLMethodReferenceVisitor : das::Visitor
{
    das::string Self;
    das::StructurePtr Source;
    FDSLMethodReferenceVisitor(const das::string& InSelf, const das::StructurePtr& InSource)
        : Self(InSelf), Source(InSource)
    {
    }
    virtual void preVisit(das::ExprCall* E) override
    {
        const int32 Index = DSLCallbackArgument(E->name, E->arguments.size());
        if (Index < 0 || !E->arguments[Index] || !E->arguments[Index]->rtti_isField())
            return;
        auto* Field = static_cast<das::ExprField*>(E->arguments[Index]);
        if (!Field->value || !Field->value->rtti_isVar() || static_cast<das::ExprVar*>(Field->value)->name != Self)
            return;
        const auto* Member = Source->findField(Field->name);
        if (Member && Member->classMethod)
            E->arguments[Index] = new das::ExprConstString(Field->at, Field->name);
    }
};
// Applies FDSLMethodReferenceVisitor to every method declared by the class.
void DSLNormalizeMethodReferences(const das::StructurePtr& St)
{
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent != St || !F->body || F->arguments.empty())
                return;
            FDSLMethodReferenceVisitor References(F->arguments[0]->name, St);
            F->body = F->body->visit(References);
        });
}
struct FDSLBodyVisitor : das::Visitor
{
    const FDasDeclaredClass& Decl;
    das::string Self;
    das::TypeDeclPtr SelfReference = nullptr, SuperReference = nullptr;
    TMap<FString, das::string> LocalTargets, LocalSupers;
    TMap<FString, das::string> LocalRenames;
    TMap<FString, das::string> QualifiedLocals;
    const das::StructurePtr Source;
    FString Folder, Registry;
    das::ModuleGroup& Group;
    das::vector<das::FunctionPtr>& GeneratedHelpers;
    TSet<FString>& HelperPaths;
    TSet<const das::Expression*> LeftFields;
    TMap<FString, FDasDeclaredProperty> NativeProperties;
    bool NativeOnly = false;
    FString Error;
    FDSLBodyVisitor(const FDasDeclaredClass& C, const das::string& S, const das::StructurePtr& St, const FString& F,
                    const FString& R, das::ModuleGroup& G, das::vector<das::FunctionPtr>& H, TSet<FString>& Paths)
        : Decl(C), Self(S), Source(St), Folder(F), Registry(R), Group(G), GeneratedHelpers(H), HelperPaths(Paths)
    {
    }
    das::ExprField* SelfField(das::Expression* E)
    {
        if (!E || !E->rtti_isField())
            return nullptr;
        auto* F = static_cast<das::ExprField*>(E);
        if (!F->value || !F->value->rtti_isVar() || static_cast<das::ExprVar*>(F->value)->name != Self)
            return nullptr;
        if (NativeOnly && (Source->findField(F->name) || !DSLNativeSelfKnown(Decl, F->name)))
            return nullptr;
        return F;
    }
    const FDasDeclaredProperty* Property(das::ExprField* F)
    {
        const FString N = DSLText(F->name);
        const auto* P = Decl.Properties.FindByPredicate([&](const auto& X) { return X.Name == N; });
        if (!P && !Source->findField(F->name) && DSLNativeSelfKnown(Decl, F->name))
        {
            auto Native = DSLNativeSelfProperty(Decl, F->name);
            if (auto* Existing = NativeProperties.Find(N))
                return Existing;
            return &NativeProperties.Add(N, MoveTemp(Native));
        }
        if (!P)
            Error = Decl.Components.ContainsByPredicate([&](const auto& C) { return C.Name == N; })
                ? TEXT("Cannot replace BlueprintReadOnly component reference")
                : TEXT("Unknown reflected self property: ")+ N;
        return P;
    }
    das::TypeDeclPtr SelfPropertyType(das::ExprField* F, const FDasDeclaredProperty* P)
    {
        if (const auto* Field = Source->findField(F->name))
            return Field->type;
        if (P->Type.Kind == TEXT("map") || P->Type.Kind == TEXT("set"))
        {
            auto* Type = new das::TypeDecl(das::Type::tTable, F->at);
            auto Convert = [&](const auto& Value)
            {
                FDasDeclaredProperty Child;
                Child.Type = *Value;
                auto Result = SelfPropertyType(F, &Child);
                return Result ? Result : DSLPropertyType(Value->Kind, Group, Source->module, F->at);
            };
            Type->firstType = Convert(P->Type.Kind == TEXT("map")? P->Type.Key : P->Type.Element);
            if ((P->Type.Kind == TEXT("map")? P->Type.Key : P->Type.Element)->Kind == TEXT("object"))
                Type->firstType = DSLMakeObjectKeyType(Type->firstType, Group, Source->module, F->at);
            Type->secondType =
                P->Type.Kind == TEXT("map")? Convert(P->Type.Value)
                                           : static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tVoid, F->at));
            DSLEnsureStructHelpers(Type, PropertyPath(P), Group, Source->module, GeneratedHelpers, HelperPaths);
            return Type;
        }
        if (P->Type.Kind == TEXT("array"))
        {
            auto* Type = new das::TypeDecl(das::Type::tArray, F->at);
            FDasDeclaredProperty Element;
            Element.Type = *P->Type.Element;
            Type->firstType = SelfPropertyType(F, &Element);
            if (!Type->firstType)
                Type->firstType = DSLPropertyType(P->Type.Element->Kind, Group, Source->module, F->at);
            DSLEnsureStructHelpers(Type, PropertyPath(P), Group, Source->module, GeneratedHelpers, HelperPaths);
            return Type;
        }
        if (P->Type.Kind == TEXT("struct"))
            return DSLHandleType(Group, Source->module,
                                 P->Type.TypePath ==
                                     TEXT("/Script/CoreUObject.Vector")
                                          ? "UEVector"
                                          : P->Type.TypePath ==
                                                TEXT("/Script/CoreUObject.Vector2D")
                                                     ? "UEVector2D"
                                                     : P->Type.TypePath ==
                                                           TEXT("/Script/CoreUObject.Rotator")
                                                                ? "UERotator"
                                                                : P->Type.TypePath ==
                                                                      TEXT("/Script/CoreUObject.Transform")
                                                                           ? "UETransform"
                                                                           : "UEStructValue");
        if (P->Type.Kind == TEXT("enum"))
        {
            das::EnumerationPtr Match = nullptr;
            TSet<das::Module*> Seen;
            auto InspectEnum = [&](das::Module* Module)
            {
                if (!Module || Seen.Contains(Module) || !Source->module->isVisibleDirectly(Module))
                    return;
                Seen.Add(Module);
                Module->enumerations.foreach (
                    [&](const das::EnumerationPtr& E)
                    {
                        if (E->isPrivate && Module != Source->module)
                            return;
                        const auto* A = DSLAnnotation(E->annotations, "ue_native_enum");
                        if (!A || DSLString(A->arguments, "path") != P->Type.TypePath)
                            return;
                        if (Match && Match != E)
                            DSLFail(TEXT("Ambiguous native self enum declaration: ")+ P->Type.TypePath);
                        Match = E;
                    });
            };
            InspectEnum(Source->module);
            for (auto* Module : Group.getModules())
                InspectEnum(Module);
            if (!Match)
                DSLFail(TEXT("Missing ue_native_enum declaration for ")+ P->Type.TypePath);
            auto* Type = new das::TypeDecl(das::Type::tEnumeration, F->at);
            Type->enumType = Match;
            return Type;
        }
        if (P->Type.Kind != TEXT("object") && P->Type.Kind != TEXT("class"))
            return nullptr;
        das::StructurePtr Match = nullptr;
        TSet<das::Module*> Seen;
        auto Inspect = [&](das::Module* Module)
        {
            if (!Module || Seen.Contains(Module) || !Source->module->isVisibleDirectly(Module))
                return;
            Seen.Add(Module);
            Module->structures.foreach (
                [&](const das::StructurePtr& S)
                {
                    if (S->privateStructure && Module != Source->module)
                        return;
                    const auto* A = DSLAnnotation(S->annotations,
                                                  P->Type.Kind == TEXT("class")? "ue_class_ref" : "ue_native_object");
                    if (!A || DSLString(A->arguments, "path") != P->Type.TypePath)
                        return;
                    if (Match && Match != S)
                        DSLFail(TEXT("Ambiguous native self reference wrapper for ")+ P->Type.TypePath +
                                     TEXT("; import one canonical wrapper"));
                    Match = S;
                });
        };
        Inspect(Source->module);
        for (auto* Module : Group.getModules())
            Inspect(Module);
        if (!Match)
            DSLFail(TEXT("Missing native self reference wrapper for ")+ P->Type.TypePath +
                         TEXT("; import its typed wrapper module"));
        auto* Type = new das::TypeDecl(das::Type::tStructure, F->at);
        Type->structType = Match;
        return Type;
    }
    das::TypeDeclPtr RootFieldType(das::ExprField* F)
    {
        if (!F)
            return nullptr;
        if (const auto* Field = Source->findField(F->name))
            return Field->type;
        if (!DSLNativeSelfKnown(Decl, F->name))
            return nullptr;
        const auto* P = Property(F);
        return P ? SelfPropertyType(F, P) : nullptr;
    }
    FString PropertyPath(const FDasDeclaredProperty* P)
    {
        if (!Source->findField(DSLUtf8(P->Name)) &&
            (P->Type.Kind == TEXT("array") || P->Type.Kind == TEXT("map") || P->Type.Kind == TEXT("set")))
        {
            auto* Carrier = DasContainerCarrierForProperty(DSLNativeSelfField(Decl, DSLUtf8(P->Name)));
            if (!Carrier)
                DSLFail(TEXT("Native container has no exact carrier"));
            return Carrier->GetPathName();
        }
        auto* Carrier = DasContainerCarrier(P->Type);
        return Carrier ? Carrier->GetPathName() : P->Type.TypePath;
    }
    das::ExpressionPtr Read(das::ExprField* F)
    {
        if (Decl.Components.ContainsByPredicate([&](const auto& C) { return C.Name == DSLText(F->name); }))
        {
            const auto* Field = Source->findField(F->name);
            return DSLReferenceValue(F->at, Field->type,
                                     DSLCall(F->at, "ue_get_self_object", {new das::ExprConstString(F->at, F->name)}));
        }
        const auto* P = Property(F);
        if (!P)
            return F;
        if (!Source->findField(F->name) && P->Name == TEXT("ActorTransform"))
            return DSLCall(F->at, "__ue_scene_world_transform",
                           {DSLCall(F->at, "__ue_scene_actor_root", {DSLCall(F->at, "ue_self_object", {})})});
        if (!Source->findField(F->name) && DasActorNativeParent(Decl.NativeParent) && P->Name == TEXT("RootComponent"))
            return DSLDirectUnpack(F->at, SelfPropertyType(F, P),
                                   DSLCall(F->at, "__ue_scene_actor_root", {DSLCall(F->at, "ue_self_object", {})}),
                                   P->Type.TypePath);
        if (!Source->findField(F->name) && DasActorNativeParent(Decl.NativeParent) && P->Name == TEXT("Owner"))
            return DSLDirectUnpack(F->at, SelfPropertyType(F, P),
                                   DSLCall(F->at, "__ue_actor_owner", {DSLCall(F->at, "ue_self_object", {})}),
                                   P->Type.TypePath);
        if (P->Type.Kind == TEXT("delegate"))
        {
            const auto* Field = Source->findField(F->name);
            return DSLDelegateFieldRead(F->at, Field->type, DSLCall(F->at, "ue_self_object", {}), P->Name);
        }
        if (P->Type.Kind == TEXT("event"))
        {
            const auto* Field = Source->findField(F->name);
            return DSLDelegateView(F->at, Field->type, DSLCall(F->at, "ue_self_object", {}), P->Name);
        }
        auto* C = new das::ExprCall(F->at, DSLSelfAccessor("ue_get_self_", P->Type));
        C->arguments.push_back(new das::ExprConstString(F->at, DSLUtf8(P->Name)));
        const auto Type = SelfPropertyType(F, P);
        return Type && (DSLTypedClass(Type) || DSLContainer(Type) || DSLDirectEnum(Type) || DSLDirectStruct(Type) ||
                        DSLTypedReference(Type) || !DSLMathPath(Type).IsEmpty())
                   ? DSLDirectUnpack(F->at, Type, C, PropertyPath(P))
                   : C;
    }
    das::ExpressionPtr Write(das::ExprOp2* E, das::ExprField* F, bool Compound)
    {
        const auto* P = Property(F);
        if (!P)
            return E;
        if (P->Type.Kind == TEXT("delegate"))
        {
            if (P->bReadOnly)
            {
                Error = TEXT("Cannot write readonly delegate field");
                return E;
            }
            if (Compound)
            {
                Error = TEXT("Delegate fields only support assignment");
                return E;
            }
            const auto* Field = Source->findField(F->name);
            return DSLDelegateFieldWrite(E->at, Field->type, DSLCall(E->at, "ue_self_object", {}), P->Name, E->right);
        }
        if (P->Type.Kind == TEXT("event"))
        {
            Error = TEXT("Cannot assign an event; use AddUFunction/RemoveUFunction");
            return E;
        }
        if (P->bReadOnly)
        {
            Error = TEXT("Cannot write BlueprintReadOnly self property: ")+ P->Name;
            return E;
        }
        if (!Source->findField(F->name) && P->Name == TEXT("ActorTransform"))
        {
            if (Compound)
                DSLFail(TEXT("ActorTransform requires whole-value assignment"));
            return DSLCall(E->at, "__ue_scene_set_world_transform",
                           {DSLCall(E->at, "__ue_scene_actor_root", {DSLCall(E->at, "ue_self_object", {})}), E->right,
                            new das::ExprConstBool(E->at, false)});
        }
        auto* C = new das::ExprCall(E->at, DSLSelfAccessor("ue_set_self_", P->Type));
        C->arguments.push_back(new das::ExprConstString(E->at, DSLUtf8(P->Name)));
        auto* Value = Compound ? static_cast<das::ExpressionPtr>(
                                     new das::ExprOp2(E->at, E->op.substr(0, E->op.size() - 1), Read(F), E->right))
                               : E->right;
        const auto* Field = Source->findField(DSLUtf8(P->Name));
        if (Field && DSLAuthoredView(Field->type))
        {
            if (Compound)
            {
                Error = TEXT("Authored reference fields support assignment only");
                return E;
            }
            const das::string Name = DSLUtf8(TEXT("__ue_set_authored_field_")+
                FMD5::HashAnsiString(*(P->Type.AuthoredId + TEXT(":")+ DSLText(Field->type->structType->module->name) +
                                                                 TEXT("::")+ DSLText(Field->type->structType->name))));
            if (!Source->module->functionsByName.find(das::hash64z(Name.c_str())))
            {
                auto* Fn = new das::Function();
                Fn->at = E->at;
                Fn->name = Name;
                Fn->generated = true;
                Fn->privateFunction = false;
                Fn->result = new das::TypeDecl(das::Type::tVoid, E->at);
                for (const auto* ArgName : {"property", "value"})
                {
                    auto* Arg = new das::Variable();
                    Arg->at = E->at;
                    Arg->name = ArgName;
                    Arg->type = Arg->name == "value"
                                    ? DSLCopyType(Field->type)
                                    : static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tString, E->at));
                    Arg->type->ref = false;
                    Arg->type->constant = true;
                    Fn->arguments.push_back(Arg);
                }
                auto* Block = new das::ExprBlock();
                Block->at = E->at;
                Block->list.push_back(DSLCall(E->at, "ue_set_self_object",
                                              {new das::ExprVar(E->at, "property"),
                                               DSLAuthoredPack(E->at, Field->type, new das::ExprVar(E->at, "value"))}));
                Fn->body = Block;
                if (!Source->module->addFunction(Fn, true))
                    DSLFail(TEXT("Authored field setter helper collision"));
            }
            return DSLCall(E->at, Name, {new das::ExprConstString(E->at, DSLUtf8(P->Name)), Value});
        }
        const auto Type = SelfPropertyType(F, P);
        if (!Field && Compound && (P->Type.Kind == TEXT("object") || P->Type.Kind == TEXT("class")))
            DSLFail(TEXT("Native reference fields support assignment only"));
        // Keep a nominal parameter boundary before DSLDirectPack extracts
        // .handle. Native field assignment must not become an implicit cast.
        if (!Field && DSLTypedObject(Type))
        {
            const das::string Name = DSLUtf8(
                TEXT("__ue_native_reference_write_")+
                     FMD5::HashAnsiString(*(P->Type.TypePath + TEXT(":")+ DSLText(Type->structType->module->name) +
                                                                    TEXT("::")+ DSLText(Type->structType->name))));
            if (!Source->module->functionsByName.find(das::hash64z(Name.c_str())))
            {
                auto* Fn = new das::Function();
                Fn->at = E->at;
                Fn->name = Name;
                Fn->generated = true;
                Fn->privateFunction = false;
                Fn->result = new das::TypeDecl(das::Type::tVoid, E->at);
                for (const auto* ArgName : {"property", "value"})
                {
                    auto* Arg = new das::Variable();
                    Arg->at = E->at;
                    Arg->name = ArgName;
                    Arg->type = Arg->name == "value"
                                    ? DSLCopyType(Type)
                                    : static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tString, E->at));
                    Arg->type->ref = false;
                    Arg->type->constant = true;
                    Fn->arguments.push_back(Arg);
                }
                auto* Body = new das::ExprBlock();
                Body->at = E->at;
                Body->list.push_back(
                    DSLCall(E->at, "ue_set_self_object",
                            {new das::ExprVar(E->at, "property"),
                             DSLDirectPack(E->at, Type, new das::ExprVar(E->at, "value"), PropertyPath(P))}));
                Fn->body = Body;
                if (!Source->module->addFunction(Fn, true))
                    DSLFail(TEXT("Native reference setter helper collision"));
            }
            return DSLCall(E->at, Name, {new das::ExprConstString(E->at, DSLUtf8(P->Name)), Value});
        }
        C->arguments.push_back(Type && (DSLTypedClass(Type) || DSLContainer(Type) || DSLDirectEnum(Type) ||
                                        DSLDirectStruct(Type) || DSLTypedReference(Type) ||
                                        !DSLMathPath(Type).IsEmpty())
                                   ? DSLDirectPack(E->at, Type, Value, PropertyPath(P))
                                   : Value);
        return C;
    }
    struct FNativeSceneRead
    {
        das::TypeDeclPtr Receiver;
        das::string Suffix;
    };
    TMap<const das::ExprField*, FNativeSceneRead> NativeSceneReads;
    das::string NativeSceneSuffix(das::Expression* E)
    {
        if (!E || !E->rtti_isField())
            return {};
        auto* Field = static_cast<das::ExprField*>(E);
        auto* Root = SelfField(Field->value);
        if (!Root || Root->name != "RootComponent")
            return {};
        return Field->name == "RelativeLocation"   ? "translation"
               : Field->name == "RelativeRotation" ? "rotation"
               : Field->name == "RelativeScale3D"  ? "scale"
                                                   : "";
    }
    // Scalar leaves of authored self struct properties (self.Stats.Count), read in place.
    struct FScalarLeafRead
    {
        das::string Root, Path, Suffix;
    };
    TMap<const das::ExprField*, FScalarLeafRead> ScalarLeafReads;
    bool ScalarLeaf(das::ExprField* Leaf, FScalarLeafRead& Out)
    {
        auto* Root = StructRoot(Leaf);
        if (!Root || Root == Leaf || !Source->findField(Root->name) || !Property(Root))
            return false;
        das::vector<das::string> Names;
        for (auto* Current = Leaf; Current != Root; Current = static_cast<das::ExprField*>(Current->value))
        {
            if (!Current->rtti_isField())
                return false;
            Names.push_back(Current->name);
        }
        auto Type = RootFieldType(Root);
        das::string Path;
        for (auto It = Names.rbegin(); It != Names.rend(); ++It)
        {
            if (!Type)
                return false;
            if (!DSLMathPath(Type).IsEmpty())
            {
                const bool Vector = DSLMathPath(Type) == TEXT("/Script/CoreUObject.Vector");
                const bool Rotator = DSLMathPath(Type) == TEXT("/Script/CoreUObject.Rotator");
                if (It + 1 != Names.rend() || !(Vector ? (*It == "x" || *It == "y" || *It == "z")
                                                       : Rotator && (*It == "pitch" || *It == "yaw" || *It == "roll")))
                    return false;
                Path += (Path.empty() ? "" : ".") + *It;
                Out = {Root->name, Path, "double"};
                return true;
            }
            if (!DSLDirectStruct(Type))
                return false;
            const auto* Field = Type->structType->findField(*It);
            if (!Field)
                return false;
            Path += (Path.empty() ? "" : ".") + *It;
            Type = Field->type;
        }
        if (!Type || DSLContainer(Type) || DSLDirectStruct(Type) || DSLDirectEnum(Type) || !DSLMathPath(Type).IsEmpty())
            return false;
        const char* Suffix = DasScalarKindSuffix(DSLTypeName(Type));
        if (!Suffix)
            return false;
        Out = {Root->name, Path, Suffix};
        return true;
    }
    virtual void preVisit(das::ExprField* F) override
    {
        FScalarLeafRead Leaf;
        if (!LeftFields.Contains(F) && ScalarLeaf(F, Leaf))
            ScalarLeafReads.Add(F, Leaf);
        const auto Suffix = NativeSceneSuffix(F);
        if (Suffix.empty())
            return;
        auto* Root = SelfField(F->value);
        const auto* P = Property(Root);
        auto* Native = FindFProperty<FStructProperty>(USceneComponent::StaticClass(), *DSLText(F->name));
        if (!P || !Native || !Native->HasAllPropertyFlags(CPF_BlueprintVisible | CPF_BlueprintReadOnly))
            DSLFail(TEXT("Native scene math property policy differs"));
        NativeSceneReads.Add(F, {SelfPropertyType(Root, P), Suffix});
    }
    das::ExprField* StructRoot(das::Expression* E)
    {
        if (auto* F = SelfField(E))
        {
            const auto Type = RootFieldType(F);
            if (Type && DSLContainer(Type))
                return F;
        }
        for (int32 Depth = 0; E && E->rtti_isField() && Depth < 18; ++Depth)
        {
            auto* F = static_cast<das::ExprField*>(E);
            if (auto* Root = SelfField(F->value))
            {
                const auto Type = RootFieldType(Root);
                return Type && (DSLDirectStruct(Type) || !DSLMathPath(Type).IsEmpty()) ? Root : nullptr;
            }
            E = F->value;
        }
        return nullptr;
    }

    das::ExprField* IndexedRoot(das::Expression* E)
    {
        for (int Depth = 0; E && Depth < 64; ++Depth)
        {
            if (auto* F = SelfField(E))
            {
                const auto Type = RootFieldType(F);
                return Type && (DSLDirectStruct(Type) || DSLContainer(Type)) ? F : nullptr;
            }
            if (E->rtti_isField())
                E = static_cast<das::ExprField*>(E)->value;
            else if (E->rtti_isAt())
                E = static_cast<das::ExprAt*>(E)->subexpr;
            else
                break;
        }
        return nullptr;
    }
    das::ExprAt* IndexedSelf(das::Expression* E)
    {
        auto* Check = E;
        das::ExprAt* Index = nullptr;
        for (int Depth = 0; Check && Depth < 64; ++Depth)
        {
            if (SelfField(Check))
                break;
            if (Check->rtti_isAt())
            {
                if (!Index)
                    Index = static_cast<das::ExprAt*>(Check);
                Check = static_cast<das::ExprAt*>(Check)->subexpr;
            }
            else if (Check->rtti_isField())
                Check = static_cast<das::ExprField*>(Check)->value;
            else
                break;
        }
        return Index && IndexedRoot(E) ? Index : nullptr;
    }
    void MarkLeft(das::Expression* E)
    {
        for (auto* Field = E; Field && Field->rtti_isField(); Field = static_cast<das::ExprField*>(Field)->value)
            if (!NativeSceneSuffix(Field).empty())
                DSLFail(TEXT("Cannot write BlueprintReadOnly native component struct"));
        if (IndexedSelf(E))
        {
            for (auto* Current = E; Current;)
            {
                if (Current->rtti_isField())
                {
                    LeftFields.Add(Current);
                    if (SelfField(Current))
                        break;
                    Current = static_cast<das::ExprField*>(Current)->value;
                }
                else if (Current->rtti_isAt())
                    Current = static_cast<das::ExprAt*>(Current)->subexpr;
                else
                    break;
            }
            return;
        }
        if (SelfField(E))
            LeftFields.Add(E);
        if (auto* Root = StructRoot(E))
            for (auto* Current = E; Current && Current->rtti_isField();)
            {
                LeftFields.Add(Current);
                if (Current == Root)
                    break;
                Current = static_cast<das::ExprField*>(Current)->value;
            }
    }
    das::ExpressionPtr MutateIndexed(das::Expression* Original, das::Expression* Target, das::ExprOp2* Assignment,
                                     das::ExprCall* Operation)
    {
        auto* Root = IndexedRoot(Target);
        try
        {
            const auto* P = Property(Root);
            if (!P || P->bReadOnly)
                DSLFail(TEXT("Cannot write readonly self container"));
            auto RootType = RootFieldType(Root);
            auto Type = RootType;
            das::vector<das::Expression*> Steps;
            for (auto* Current = Target; Current != Root;)
            {
                Steps.push_back(Current);
                Current = Current->rtti_isField() ? static_cast<das::ExprField*>(Current)->value
                                                  : static_cast<das::ExprAt*>(Current)->subexpr;
            }
            const das::string Name = "__ue_index_write_" + das::to_string(Source->module->functions.unlocked_size()) +
                                     "_" + das::to_string(Source->module->generics.unlocked_size()) + "_" +
                                     das::to_string(NextIndexedHelper++);
            das::FunctionPtr Fn = new das::Function();
            Fn->at = Original->at;
            Fn->name = Name;
            Fn->generated = true;
            Fn->privateFunction = false;
            auto* Call = DSLCall(Original->at, Name, {new das::ExprConstString(Original->at, Root->name)});
            auto Add = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Input)
            {
                auto* A = new das::Variable();
                A->at = Original->at;
                A->name = N;
                A->type = DSLCopyType(T);
                A->type->ref = false;
                A->type->constant = true;
                Fn->arguments.push_back(A);
                if (Input)
                    Call->arguments.push_back(Input);
            };
            auto IndexType = [&]()
            {
                auto* T = new das::TypeDecl(das::Type::option, Original->at);
                T->argTypes.push_back(new das::TypeDecl(das::Type::tInt, Original->at));
                T->argTypes.push_back(new das::TypeDecl(das::Type::tInt64, Original->at));
                return T;
            };
            Add("property", new das::TypeDecl(das::Type::tString, Original->at), nullptr);
            das::ExpressionPtr Left = new das::ExprVar(Original->at, "snapshot");
            for (auto It = Steps.rbegin(); It != Steps.rend(); ++It)
            {
                auto* Step = *It;
                if (Step->rtti_isField())
                {
                    const auto N = static_cast<das::ExprField*>(Step)->name;
                    if (!DSLMathPath(Type).IsEmpty())
                    {
                        const auto Math = DSLMathPath(Type);
                        if (!((Math == TEXT("/Script/CoreUObject.Vector") && (N == "x" || N == "y" || N == "z")) ||
                               (Math == TEXT("/Script/CoreUObject.Rotator") &&
                                             (N == "pitch" || N == "yaw" || N == "roll"))))
                            DSLFail(TEXT("Unsupported indexed math member"));
                        Type = new das::TypeDecl(das::Type::tDouble, Original->at);
                    }
                    else
                    {
                        if (!DSLDirectStruct(Type))
                            DSLFail(TEXT("Indexed write crosses unsupported field type"));
                        const auto* F = Type->structType->findField(N);
                        if (!F)
                            DSLFail(TEXT("Unknown indexed member"));
                        for (const auto& A : F->annotation)
                            if (A.name == "ue_property" && A.type == das::Type::tString &&
                                A.sValue == "BlueprintReadOnly")
                                DSLFail(TEXT("Cannot write readonly indexed member"));
                        Type = F->type;
                    }
                    Left = new das::ExprField(Original->at, Left, N);
                }
                else
                {
                    if (!DSLContainer(Type) || DSLSet(Type))
                        DSLFail(TEXT("Indexed assignment requires array or map"));
                    const bool Array = Type->baseType == das::Type::tArray;
                    const auto N = "index_" + das::to_string(Fn->arguments.size());
                    Add(N, Array ? static_cast<das::TypeDeclPtr>(IndexType()) : Type->firstType,
                        static_cast<das::ExprAt*>(Step)->index);
                    Left = new das::ExprAt(Original->at, Left, new das::ExprVar(Original->at, N));
                    Type = Array ? Type->firstType : Type->secondType;
                }
            }
            bool Result = false;
            das::ExpressionPtr Mutation;
            if (Operation)
            {
                if (!DSLContainer(Type))
                    DSLFail(TEXT("Indexed operation requires supported container"));
                const bool Array = Type->baseType == das::Type::tArray, Set = DSLSet(Type),
                           Push = Operation->name == "push" || Operation->name == "push_clone";
                const auto Count = Operation->arguments.size();
                if (Operation->name == "clear" ? Count != 1
                    : Operation->name == "pop" ? (!Array || Count != 1)
                    : Push                     ? (!Array || (Count != 2 && Count != 3))
                    : (Operation->name == "insert" || Operation->name == "insert_clone")
                        ? (Array || (Operation->name == "insert_clone" && Set) || Count != (Set ? 2 : 3))
                        : (Array ? (Count != 2 && Count != 3) : Count != 2))
                    DSLFail(TEXT("Unsupported indexed container operation/arity"));
                auto* Op = DSLCall(Original->at, Operation->name, {Left});
                for (size_t I = 1; I < Count; ++I)
                {
                    const auto N = "arg_" + das::to_string(I);
                    Add(N,
                        Array    ? (Push && I == 1 ? Type->firstType : static_cast<das::TypeDeclPtr>(IndexType()))
                        : I == 1 ? Type->firstType
                                 : Type->secondType,
                        Operation->arguments[I]);
                    Op->arguments.push_back(new das::ExprVar(Original->at, N));
                }
                Mutation = Op;
                Result = Operation->name == "erase" && !Array;
            }
            else
            {
                Add("value", Type, Assignment->right);
                auto* Value = new das::ExprVar(Original->at, "value");
                if (Assignment->rtti_isCopy())
                    Mutation = new das::ExprCopy(Original->at, Left, Value);
                else if (Assignment->rtti_isClone())
                    Mutation = new das::ExprClone(Original->at, Left, Value);
                else if (Assignment->__rtti == das::string("ExprMove"))
                    DSLFail(TEXT("Indexed native writes use copy or clone, not consuming move"));
                else
                    Mutation = new das::ExprOp2(Original->at, Assignment->op, Left, Value);
            }
            Fn->result = new das::TypeDecl(Result ? das::Type::tBool : das::Type::tVoid, Original->at);
            auto* Body = new das::ExprBlock();
            Body->at = Original->at;
            auto Local = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Init, bool Move)
            {
                auto* V = new das::Variable();
                V->at = Original->at;
                V->name = N;
                V->type = DSLCopyType(T);
                V->type->ref = false;
                V->type->constant = false;
                V->init = Init;
                V->init_via_move = Move;
                auto* L = new das::ExprLet();
                L->at = Original->at;
                L->variables.push_back(V);
                Body->list.push_back(L);
            };
            Local("snapshot", RootType,
                  DSLDirectUnpack(
                      Original->at, RootType,
                      DSLCall(Original->at, "ue_get_self_struct", {new das::ExprVar(Original->at, "property")}),
                      PropertyPath(P)),
                  true);
            if (Result)
                Local("result", Fn->result, Mutation, false);
            else
                Body->list.push_back(Mutation);
            Body->list.push_back(DSLCall(
                Original->at, "ue_set_self_struct",
                {new das::ExprVar(Original->at, "property"),
                 DSLDirectPack(Original->at, RootType, new das::ExprVar(Original->at, "snapshot"), PropertyPath(P))}));
            if (Result)
                Body->list.push_back(new das::ExprReturn(Original->at, new das::ExprVar(Original->at, "result")));
            Fn->body = Body;
            if (!(Fn->isGeneric() ? Source->module->addGeneric(Fn, true) : Source->module->addFunction(Fn, true)))
                DSLFail(TEXT("Indexed mutation helper collision"));
            return Call;
        }
        catch (const std::exception& X)
        {
            Error = UTF8_TO_TCHAR(X.what());
            return Original;
        }
    }
    das::ExpressionPtr WriteIndexed(das::ExprOp2* E, bool) { return MutateIndexed(E, E->left, E, nullptr); }
    uint32 NextIndexedHelper = 0;
    das::ExpressionPtr WriteLeaf(das::ExprOp2* E, bool Compound)
    {
        auto* Leaf = static_cast<das::ExprField*>(E->left);
        auto* Root = StructRoot(Leaf);
        if (Root && Root->name == "ActorTransform" && !Source->findField(Root->name))
            DSLFail(TEXT("ActorTransform requires whole-value assignment"));
        const auto* P = Property(Root);
        if (!P)
            return E;
        if (P->bReadOnly)
        {
            Error = TEXT("Cannot write BlueprintReadOnly self struct: ")+ P->Name;
            return E;
        }
        das::vector<das::string> Names;
        for (auto* Current = Leaf; Current != Root; Current = static_cast<das::ExprField*>(Current->value))
            Names.push_back(Current->name);
        auto Type = RootFieldType(Root);
        das::string Path;
        for (auto It = Names.rbegin(); It != Names.rend(); ++It)
        {
            if (!DSLMathPath(Type).IsEmpty())
            {
                const bool Vector = DSLMathPath(Type) == TEXT("/Script/CoreUObject.Vector");
                const bool Rotator = DSLMathPath(Type) == TEXT("/Script/CoreUObject.Rotator");
                if (It + 1 != Names.rend() || (Vector    ? (*It != "x" && *It != "y" && *It != "z")
                                               : Rotator ? (*It != "pitch" && *It != "yaw" && *It != "roll")
                                                         : true))
                {
                    Error = TEXT("Math scalar writes support Vector x/y/z and Rotator pitch/yaw/roll");
                    return E;
                }
                if (!Path.empty())
                    Path += ".";
                Path += *It;
                Type = new das::TypeDecl(das::Type::tDouble, E->at);
                break;
            }
            if (!DSLDirectStruct(Type))
            {
                Error = TEXT("Nested write crosses non-authored struct");
                return E;
            }
            const auto* Field = Type->structType->findField(*It);
            if (!Field)
            {
                Error = TEXT("Unknown self struct field");
                return E;
            }
            for (const auto& A : Field->annotation)
                if (A.name == "ue_property" && A.type == das::Type::tString && A.sValue == "BlueprintReadOnly")
                {
                    Error = TEXT("Cannot write BlueprintReadOnly struct member");
                    return E;
                }
            if (!Path.empty())
                Path += ".";
            Path += *It;
            Type = Field->type;
        }
        const FString K = DSLTypeName(Type);
        if (DSLDirectEnum(Type) || DSLTypedReference(Type) || DSLDirectStruct(Type) || !DSLMathPath(Type).IsEmpty() ||
            DSLContainer(Type))
        {
            if (Compound)
            {
                Error = TEXT("Typed value struct members support assignment, not compound arithmetic");
                return E;
            }
            const das::string Kind = DSLDirectEnum(Type) ? "enum" : DSLTypedReference(Type) ? "object" : "struct";
            const FString ValuePath = DSLDirectEnum(Type) || DSLDirectStruct(Type) || DSLContainer(Type)
                                          ? DSLDirectPath(Type, Folder, Registry)
                                      : DSLTypedReference(Type) ? DSLTypedReferencePath(Type)
                                                                : DSLMathPath(Type);
            if (DSLContainer(Type))
                DSLEnsureStructHelpers(Type, ValuePath, Group, Source->module, GeneratedHelpers, HelperPaths);
            const das::string Name =
                DSLUtf8(TEXT("__ue_commit_typed_leaf_")+ FMD5::HashAnsiString(*(DSLText(Kind) + TEXT(":")+ ValuePath)));
            if (!Source->module->functionsByName.find(das::hash64z(Name.c_str())))
            {
                auto* F = new das::Function();
                F->at = E->at;
                F->name = Name;
                F->generated = true;
                F->privateFunction = false;
                F->result = new das::TypeDecl(das::Type::tVoid, E->at);
                for (const auto* ArgName : {"property", "member", "value"})
                {
                    auto* Arg = new das::Variable();
                    Arg->at = E->at;
                    Arg->name = ArgName;
                    Arg->type = Arg->name == "value"
                                    ? DSLCopyType(Type)
                                    : static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tString, E->at));
                    Arg->type->ref = false;
                    Arg->type->constant = true;
                    F->arguments.push_back(Arg);
                }
                auto* Body = new das::ExprBlock();
                Body->at = E->at;
                Body->list.push_back(
                    DSLCall(E->at, "ue_dsl_commit_struct_member",
                            {new das::ExprVar(E->at, "property"), new das::ExprVar(E->at, "member"),
                             DSLDirectPack(E->at, Type, new das::ExprVar(E->at, "value"), ValuePath)}));
                F->body = Body;
                if (!Source->module->addFunction(F, true))
                    DSLFail(TEXT("Typed leaf commit helper collision"));
            }
            return DSLCall(
                E->at, Name,
                {new das::ExprConstString(E->at, Root->name), new das::ExprConstString(E->at, Path), E->right});
        }
        if (DSLContainer(Type) || K == TEXT("struct") || K == TEXT("enum") || K == TEXT("object") || K.IsEmpty())
        {
            Error = TEXT("Nested member assignment supports scalar leaves; copy and assign the whole self property for nested values");
            return E;
        }
        // Scalar leaves are read and written in place on the self property.
        const char* Suffix = DSLScalarSuffix(K);
        auto* Value =
            Compound
                ? static_cast<das::ExpressionPtr>(new das::ExprOp2(
                      E->at, E->op.substr(0, E->op.size() - 1),
                      DSLCall(E->at, das::string("__ue_self_struct_get_") + Suffix,
                              {new das::ExprConstString(E->at, Root->name), new das::ExprConstString(E->at, Path)}),
                      E->right))
                : E->right;
        return DSLCall(E->at, das::string("__ue_self_struct_set_") + Suffix,
                       {new das::ExprConstString(E->at, Root->name), new das::ExprConstString(E->at, Path), Value});
    }
    bool ContainerOperation(const das::string& N)
    {
        return N == "insert" || N == "insert_clone" || N == "erase" || N == "clear" || N == "push" ||
               N == "push_clone" || N == "pop";
    }
    bool OwnMethod(das::ExprField* Field) const
    {
        const auto* Member = Field ? Source->findField(Field->name) : nullptr;
        return (Member && Member->classMethod) || (Field && LocalTargets.Contains(DSLText(Field->name)));
    }
    // Callback arguments accept a method reference (self.Method) or its name as a string literal.
    // A reference is replaced by the literal, so the checks and lowering below are shared.
    das::ExprConstString* CallbackName(das::ExpressionPtr& Arg)
    {
        if (!Arg)
            return nullptr;
        if (Arg->__rtti == das::string("ExprConstString"))
            return static_cast<das::ExprConstString*>(Arg);
        auto* Field = SelfField(Arg);
        if (!OwnMethod(Field))
            return nullptr;
        auto* Literal = new das::ExprConstString(Arg->at, Field->name);
        Arg = Literal;
        return Literal;
    }
    das::ExprField* NativeSelfMethod(das::Expression* E)
    {
        if (!E || !E->rtti_isField())
            return nullptr;
        auto* F = static_cast<das::ExprField*>(E);
        if (!F->value || !F->value->rtti_isVar() || static_cast<das::ExprVar*>(F->value)->name != Self ||
            Source->findField(F->name) || !DasNativeGameplayMethod(Decl.NativeParent, DSLText(F->name)))
            return nullptr;
        return F;
    }
    das::ExpressionPtr NativeSelfCall(das::ExprInvoke* E)
    {
        if (!E->isInvokeMethod || E->arguments.empty())
            return E;
        auto* F = NativeSelfMethod(E->arguments[0]);
        if (!F)
            return E;
        try
        {
            const TCHAR* View = nullptr;
            for (const auto& V : DasNativeGameplayViews)
                if (Decl.NativeParent == V.Class)
                {
                    View = V.Source;
                    break;
                }
            if (!View)
                DSLFail(TEXT("Missing generated native self view"));
            auto* Receiver = DSLCall(E->at, DSLUtf8(View) + "_from", {DSLCall(E->at, "ue_self_object", {})});
            auto* Call = new das::ExprInvoke();
            Call->at = E->at;
            Call->isInvokeMethod = true;
            Call->arguments.push_back(new das::ExprField(E->at, Receiver, F->name));
            for (size_t I = 1; I < E->arguments.size(); ++I)
                Call->arguments.push_back(E->arguments[I]);
            return Call;
        }
        catch (const std::exception& X)
        {
            Error = UTF8_TO_TCHAR(X.what());
            return E;
        }
    }
    virtual void preVisit(das::ExprInvoke* E) override
    {
        if (E->isInvokeMethod && !E->arguments.empty())
            if (auto* F = NativeSelfMethod(E->arguments[0]))
                LeftFields.Add(F);
        if (E->isInvokeMethod && !E->arguments.empty())
            if (auto* Field = SelfField(E->arguments[0]); OwnMethod(Field))
                LeftFields.Add(Field);
    }
    das::ExpressionPtr SuperCall(das::ExprInvoke* E)
    {
        if (!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())
            return E;
        auto* Field = static_cast<das::ExprField*>(E->arguments[0]);
        if (!Field->value->rtti_isVar() || static_cast<das::ExprVar*>(Field->value)->name != "Super")
            return E;
        if (const auto* Target = LocalSupers.Find(DSLText(Field->name)))
        {
            auto* Call = DSLCall(E->at, *Target, {});
            for (size_t I = 2; I < E->arguments.size(); ++I)
                Call->arguments.push_back(E->arguments[I]);
            return Call;
        }
        if (!SuperReference)
        {
            Error = TEXT("Super requires an authored parent");
            return E;
        }
        if (Field->name == "ConstructionScript" || Field->name == "Initialize" || Field->name == "BeginPlay" ||
            Field->name == "Tick" || Field->name == "EndPlay")
        {
            const int Kind = Field->name == "BeginPlay"    ? 0
                             : Field->name == "Tick"       ? 1
                             : Field->name == "EndPlay"    ? 2
                             : Field->name == "Initialize" ? 3
                                                           : 4;
            const size_t Count = (Kind == 0 || Kind == 3 || Kind == 4) ? 2 : 3;
            if (E->arguments.size() != Count)
            {
                Error = TEXT("Super lifecycle argument count mismatch");
                return E;
            }
            return DSLCall(
                E->at, "__ue_parent_lifecycle",
                {new das::ExprConstString(E->at, DSLUtf8(Decl.Id)), new das::ExprConstInt(E->at, Kind),
                 Kind == 1 ? E->arguments[2] : static_cast<das::ExpressionPtr>(new das::ExprConstFloat(E->at, 0.f)),
                 Kind == 2 ? E->arguments[2] : static_cast<das::ExpressionPtr>(new das::ExprConstInt(E->at, 0))});
        }
        auto* Call = new das::ExprInvoke();
        Call->at = E->at;
        Call->isInvokeMethod = true;
        auto* Receiver = DSLReferenceValue(E->at, SuperReference, DSLCall(E->at, "ue_self_object", {}));
        Call->arguments.push_back(new das::ExprField(E->at, Receiver, Field->name));
        for (size_t I = 1; I < E->arguments.size(); ++I)
            Call->arguments.push_back(E->arguments[I]);
        return Call;
    }
    virtual das::ExpressionPtr visit(das::ExprInvoke* E) override
    {
        auto Super = SuperCall(E);
        if (Super != E)
            return Super;
        auto NativeCall = NativeSelfCall(E);
        if (NativeCall != E)
            return NativeCall;
        if (!E->isInvokeMethod || E->arguments.empty())
            return E;
        auto* Field = SelfField(E->arguments[0]);
        if (!OwnMethod(Field))
            return E;
        const FString Name = DSLText(Field->name);
        const auto* Method = Decl.Methods.FindByPredicate([&](const auto& M) { return M.Name == Name; });
        if (Method && (DSLVirtualMethod(Decl, *Method) ||
                       Method->ScriptFile != FPaths::GetCleanFilename(DSLText(Source->at.fileInfo->name))))
        {
            if (!SelfReference)
            {
                Error = TEXT("Missing self event reference");
                return E;
            }
            auto* Call = new das::ExprInvoke();
            Call->at = E->at;
            Call->isInvokeMethod = true;
            auto* Receiver = DSLReferenceValue(E->at, SelfReference, DSLCall(E->at, "ue_self_object", {}));
            Call->arguments.push_back(new das::ExprField(E->at, Receiver, Field->name));
            for (size_t I = 1; I < E->arguments.size(); ++I)
                Call->arguments.push_back(E->arguments[I]);
            return Call;
        }
        const auto* Local = LocalTargets.Find(Name);
        const auto Target = Method ? DSLUtf8(TEXT("__impl_")+ Method->EntryPoint)
    : Local                        ? *Local
                                   : DSLUtf8(TEXT("__ue_local_")+ Decl.Name + TEXT("_")+ Name);
        auto* Call = new das::ExprCall(E->at, Target);
        for (size_t I = 2; I < E->arguments.size(); ++I)
            Call->arguments.push_back(E->arguments[I]);
        return Call;
    }
    virtual void preVisit(das::ExprCall* E) override
    {
        // Method references must become names before the field itself is lowered as a property read.
        const int32 Callback = DSLCallbackArgument(E->name, E->arguments.size());
        if (Callback >= 0)
            CallbackName(E->arguments[Callback]);
        if ((ContainerOperation(E->name)) && !E->arguments.empty() &&
            (StructRoot(E->arguments[0]) || IndexedSelf(E->arguments[0])))
            MarkLeft(E->arguments[0]);
        if ((E->name == "keys" || E->name == "values") && !E->arguments.empty() &&
            (SelfField(E->arguments[0]) || StructRoot(E->arguments[0])))
            Error = TEXT("Container iterators over self require a local clone");
    }
    das::ExpressionPtr RewriteTimer(das::ExprCall* E)
    {
        const bool NextTick = E->name == "ue_timer_next_tick" || E->name == "ue_foundation::ue_timer_next_tick";
        const size_t BaseCount = NextTick ? 2 : 4, HandlerIndex = NextTick ? 1 : 2, PayloadIndex = NextTick ? 2 : 4;
        const bool HasArgument = E->arguments.size() == BaseCount + 1;
        if (E->arguments.size() > HandlerIndex)
            CallbackName(E->arguments[HandlerIndex]);
        if (E->arguments.size() >= BaseCount + 2)
        {
            try
            {
                return DSLMultiTimer(E, Source, Decl, Folder, Registry, Group, GeneratedHelpers, HelperPaths, NextTick);
            }
            catch (const std::exception& X)
            {
                Error = UTF8_TO_TCHAR(X.what());
                return E;
            }
        }
        if ((E->arguments.size() != BaseCount && !HasArgument) || !CallbackName(E->arguments[HandlerIndex]))
        {
            Error = TEXT("Class timer requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[HandlerIndex]);
        const FString MethodName = DSLText(Name->text);
        bool Found = false;
        das::TypeDeclPtr CaptureType = nullptr;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                if (N != MethodName || !DSLAnnotation(F->annotations, "ue_function") || F->isStaticClassMethod ||
                    !F->result || F->result->baseType != das::Type::tVoid ||
                    F->arguments.size() != (HasArgument ? 2u : 1u))
                    return;
                if (!HasArgument)
                {
                    Found = true;
                    return;
                }
                const auto T = F->arguments[1]->type;
                const auto* Capability = FindDasCallbackType(DSLCallbackKind(T));
                Found = T && !T->ref && !F->arguments[1]->init && Capability && Capability->bTimer &&
                        Capability->Signature != EDasMethodSignature::Void;
                if (Found && Capability->bDynamic)
                {
                    const bool Class = DSLTypedClass(T), Object = DSLTypedReference(T), Enum = DSLDirectEnum(T),
                               Struct = DSLDirectStruct(T) || DSLContainer(T);
                    FString Issue;
                    Found = (Class || Object || Enum || Struct) &&
                            ValidateDasCallbackTypePath(Class ? TEXT("class")
                                                              : Object
                                                        ? TEXT("object")
                                                        : Enum
                                                        ? TEXT("enum")
                                                        : TEXT("struct"),
                                                               Class    ? DSLTypedClassPath(T)
                                                               : Object ? DSLTypedReferencePath(T)
                                                                        : DSLDirectPath(T, Folder, Registry),
                                                               Issue);
                    if (!Found && !Issue.IsEmpty())
                        Error = Issue;
                }
                if (Found)
                    CaptureType = T;
            });
        if (!Found)
        {
            Error = TEXT("Timer callback requires an annotated void instance method matching zero/one supported value argument");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ MethodName);
        if (CaptureType && DSLTypedClass(CaptureType))
        {
            const FString Path = DSLTypedClassPath(CaptureType);
            E->name = NextTick ? "__ue_timer_next_tick_class" : "__ue_timer_start_class";
            E->arguments[PayloadIndex] = DSLClassPack(E->at, CaptureType, E->arguments[PayloadIndex]);
            E->arguments.push_back(new das::ExprConstString(E->at, DSLUtf8(Path)));
        }
        else if (CaptureType && DSLTypedReference(CaptureType))
        {
            const FString Path = DSLTypedReferencePath(CaptureType);
            E->name = NextTick ? "__ue_timer_next_tick_object" : "__ue_timer_start_object";
            E->arguments[PayloadIndex] = DSLDirectPack(E->at, CaptureType, E->arguments[PayloadIndex], Path);
            E->arguments.push_back(new das::ExprConstString(E->at, DSLUtf8(Path)));
        }
        else if (CaptureType && (DSLContainer(CaptureType) || DSLDirectStruct(CaptureType)))
        {
            const FString Path = DSLDirectPath(CaptureType, Folder, Registry);
            DSLEnsureStructHelpers(CaptureType, Path, Group, Source->module, GeneratedHelpers, HelperPaths);
            E->name = NextTick ? "__ue_timer_next_tick_array" : "__ue_timer_start_array";
            E->arguments[PayloadIndex] = DSLDirectPack(E->at, CaptureType, E->arguments[PayloadIndex], Path);
        }
        else if (CaptureType && DSLDirectEnum(CaptureType))
        {
            const FString Path = DSLDirectPath(CaptureType, Folder, Registry);
            E->arguments[PayloadIndex] = DSLDirectPack(E->at, CaptureType, E->arguments[PayloadIndex], Path);
        }
        return E;
    }
    das::ExpressionPtr RewriteInput(das::ExprCall* E)
    {
        const bool Axis = E->name == "ue_input_bind_axis" || E->name == "ue_foundation::ue_input_bind_axis";
        const size_t HandlerIndex = Axis ? 1 : 2;
        if (E->arguments.size() != (Axis ? 4u : 5u) || !CallbackName(E->arguments[HandlerIndex]))
        {
            Error = TEXT("Class input binding requires a callback method (self.Method or its name) and consume/paused flags");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[HandlerIndex]);
        const FString Method = DSLText(Name->text);
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                if (N != Method || !DSLAnnotation(F->annotations, "ue_function") || F->isStaticClassMethod ||
                    !F->result || F->result->baseType != das::Type::tVoid || F->arguments.size() != (Axis ? 2u : 1u))
                    return;
                Found = !Axis || (F->arguments[1]->type && !F->arguments[1]->type->ref && !F->arguments[1]->init &&
                                  DSLTypeName(F->arguments[1]->type) == TEXT("float"));
            });
        if (!Found)
        {
            Error = TEXT("Input requires annotated instance action():void or axis(value:float):void");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ Method);
        return E;
    }
    das::ExpressionPtr RewriteEnhancedInput(das::ExprCall* E)
    {
        if (E->arguments.size() != 3 || !CallbackName(E->arguments[2]))
        {
            Error = TEXT("Enhanced Input class binding requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[2]);
        const FString Method = DSLText(Name->text);
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                if (N != Method || !DSLAnnotation(F->annotations, "ue_function") || F->isStaticClassMethod ||
                    !F->result || F->result->baseType != das::Type::tVoid || F->arguments.size() != 2)
                    return;
                const auto T = F->arguments[1]->type;
                Found = T && !T->ref && !F->arguments[1]->init &&
                        (DSLTypeName(T) ==
                         TEXT("bool") || DSLTypeName(T) ==
                                             TEXT("float") || (DSLMathPath(T) ==
                                                               TEXT("/Script/EnhancedInput.InputActionValue") ||
                                                                    DSLMathPath(T) ==
                                                                        TEXT("/Script/CoreUObject.Vector") ||
                                                                             DSLMathPath(T) ==
                                                                                 TEXT("/Script/CoreUObject.Vector2D")));
            });
        if (!Found)
        {
            Error = TEXT("Enhanced Input requires annotated void handler(value:InputActionValue/bool/float/UEVector2D/UEVector)");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ Method);
        return E;
    }
    das::ExpressionPtr RewriteLatent(das::ExprCall* E)
    {
        if (E->arguments.size() != 3 || !CallbackName(E->arguments[2]))
        {
            Error = TEXT("Class latent delay requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[2]);
        const FString Method = DSLText(Name->text);
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                if (N == Method && DSLAnnotation(F->annotations, "ue_function") && !F->isStaticClassMethod &&
                    F->result && F->result->baseType == das::Type::tVoid && F->arguments.size() == 1)
                    Found = true;
            });
        if (!Found)
        {
            Error = TEXT("Latent completion requires annotated instance method():void");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ Method);
        return E;
    }
    das::ExpressionPtr RewriteEffect(das::ExprCall* E)
    {
        const bool Single = E->name == "ue_sound_play" || E->name == "ue_effects::ue_sound_play" ||
                            E->name == "ue_niagara_spawn" || E->name == "ue_effects::ue_niagara_spawn";
        const size_t Index = Single ? 3 : 4;
        if (E->arguments.size() <= Index)
            return E;
        if (!CallbackName(E->arguments[Index]))
        {
            Error = TEXT("Effect completion requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[Index]);
        if (Name->text.empty())
            return E;
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent == Source && DSLAnnotation(F->annotations, "ue_function") &&
                    !F->isStaticClassMethod && F->result && F->result->baseType == das::Type::tVoid &&
                    F->arguments.size() == 1 && DSLMethodName(F) == Name->text)
                    Found = true;
            });
        if (!Found)
        {
            Error = TEXT("Effect completion requires ue_function void Handler()");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ DSLText(Name->text));
        return E;
    }
    das::ExpressionPtr RewriteAIMove(das::ExprCall* E)
    {
        if (E->arguments.size() < 4)
            return E;
        if (E->arguments[3]->__rtti == das::string("ExprConstString") &&
            static_cast<das::ExprConstString*>(E->arguments[3])->text.empty())
            return E;
        if (E->arguments.size() < 4 || !CallbackName(E->arguments[3]))
        {
            Error = TEXT("MoveTo requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[3]);
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source || !DSLAnnotation(F->annotations, "ue_function") ||
                    F->isStaticClassMethod || !F->result || F->result->baseType != das::Type::tVoid ||
                    F->arguments.size() != 2)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                const auto T = F->arguments[1]->type;
                if (N == DSLText(Name->text) && T && !T->ref && !F->arguments[1]->init && DSLDirectStruct(T) &&
                    DSLDirectPath(T, Folder, Registry) == TEXT("/Script/DasRuntime.DasMoveResult"))
                    Found = true;
            });
        if (!Found)
        {
            Error = TEXT("MoveTo callback requires ue_function void(result:MoveResult)");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ DSLText(Name->text));
        return E;
    }
    das::ExpressionPtr RewriteMontage(das::ExprCall* E)
    {
        if (E->arguments.size() < 4 || !CallbackName(E->arguments[3]))
        {
            Error = TEXT("Montage requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[3]);
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source || !DSLAnnotation(F->annotations, "ue_function") ||
                    F->isStaticClassMethod || !F->result || F->result->baseType != das::Type::tVoid ||
                    F->arguments.size() != 2)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                const auto T = F->arguments[1]->type;
                if (N == DSLText(Name->text) && T && !T->ref && !F->arguments[1]->init && DSLDirectStruct(T) &&
                    DSLDirectPath(T, Folder, Registry) == TEXT("/Script/DasRuntime.DasMontageEvent"))
                    Found = true;
            });
        if (!Found)
        {
            Error = TEXT("Montage callback requires ue_function void(event:MontageEvent)");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ DSLText(Name->text));
        return E;
    }
    // Button handlers are Blueprint functions found by name at runtime; check them here when known.
    das::ExpressionPtr RewriteWidgetClick(das::ExprCall* E)
    {
        if (E->arguments.size() != 3)
            return E;
        auto* Name = CallbackName(E->arguments[2]);
        if (!Name)
            return E;
        bool Found = false;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent == Source && DSLMethodName(F) == Name->text &&
                    DSLAnnotation(F->annotations, "ue_function") && !F->isStaticClassMethod && F->result &&
                    F->result->baseType == das::Type::tVoid && F->arguments.size() == 1)
                    Found = true;
            });
        if (!Found)
            Error = TEXT("Button handler requires ue_function(BlueprintCallable) void Handler()");
        return E;
    }
    das::ExpressionPtr RewriteAssetLoad(das::ExprCall* E)
    {
        if (E->arguments.size() != 3 || !CallbackName(E->arguments[2]))
        {
            Error = TEXT("Class async load requires a callback method (self.Method or its name)");
            return E;
        }
        auto* Name = static_cast<das::ExprConstString*>(E->arguments[2]);
        const FString Method = DSLText(Name->text);
        FString Path;
        Source->module->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (F->classParent != Source)
                    return;
                const FString N = DSLText(DSLMethodName(F));
                if (N != Method || !DSLAnnotation(F->annotations, "ue_function") || F->isStaticClassMethod ||
                    !F->result || F->result->baseType != das::Type::tVoid || F->arguments.size() != 2)
                    return;
                const auto T = F->arguments[1]->type;
                if (T && !T->ref && !F->arguments[1]->init && DSLTypedObject(T))
                    Path = DSLTypedObjectPath(T);
            });
        if (Path.IsEmpty())
        {
            Error = TEXT("Async asset load requires annotated void callback with typed native object parameter");
            return E;
        }
        Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ Method);
        E->name = "__ue_asset_load_async";
        E->arguments.push_back(new das::ExprConstString(E->at, DSLUtf8(Path)));
        return E;
    }
    virtual das::ExpressionPtr visit(das::ExprCall* E) override
    {
        if (const auto* Local = QualifiedLocals.Find(DSLText(E->name));
            Local && !E->arguments.empty() && E->arguments[0]->rtti_isVar() &&
            static_cast<das::ExprVar*>(E->arguments[0])->name == Self)
        {
            auto* Call = DSLCall(E->at, *Local, {});
            for (size_t I = 1; I < E->arguments.size(); ++I)
                Call->arguments.push_back(E->arguments[I]);
            return Call;
        }
        if (const auto* Renamed = LocalRenames.Find(DSLText(E->name)))
            E->name = *Renamed;
        if (E->name == "ue_soft_load_async" || E->name == "ue_soft_references::ue_soft_load_async")
        {
            if (E->arguments.size() != 3 || !CallbackName(E->arguments[2]))
                DSLFail(TEXT("Soft async callback requires a callback method (self.Method or its name)"));
            auto* Name = static_cast<das::ExprConstString*>(E->arguments[2]);
            bool Found = false;
            Source->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent == Source && DSLMethodName(F) == Name->text &&
                        DSLAnnotation(F->annotations, "ue_function") && F->result &&
                        F->result->baseType == das::Type::tVoid && F->arguments.size() == 2 &&
                        !F->arguments[1]->type->ref &&
                        (DSLTypedClass(F->arguments[1]->type) || DSLTypedObject(F->arguments[1]->type)))
                        Found = true;
                });
            if (!Found)
                DSLFail(TEXT("Soft async handler requires an annotated void method with typed object/class input"));
            Name->text = DSLUtf8(TEXT("ue_")+ Decl.Name + TEXT("_")+ DSLText(Name->text));
            return E;
        }
        if (E->name == "ue_widget_bind_click" || E->name == "ue_gameplay_services::ue_widget_bind_click")
            return RewriteWidgetClick(E);
        if (E->name == "ue_effect_play" || E->name == "ue_effects::ue_effect_play" || E->name == "ue_effect_at" ||
            E->name == "ue_effects::ue_effect_at" || E->name == "ue_sound_play" ||
            E->name == "ue_effects::ue_sound_play" || E->name == "ue_niagara_spawn" ||
            E->name == "ue_effects::ue_niagara_spawn")
            return RewriteEffect(E);
        if (E->name == "ue_ai_move_to_actor" || E->name == "ue_ai::ue_ai_move_to_actor" ||
            E->name == "ue_ai_move_to_location" || E->name == "ue_ai::ue_ai_move_to_location")
            return RewriteAIMove(E);
        if (E->name == "ue_montage_play" || E->name == "ue_animation::ue_montage_play")
            return RewriteMontage(E);
        if (E->name == "ue_asset_load_async" || E->name == "ue_foundation::ue_asset_load_async")
            return RewriteAssetLoad(E);
        if (E->name == "ue_latent_delay" || E->name == "ue_foundation::ue_latent_delay")
            return RewriteLatent(E);
        if (E->name == "ue_enhanced_bind" || E->name == "ue_enhanced_input::ue_enhanced_bind")
            return RewriteEnhancedInput(E);
        if (E->name == "ue_input_bind_action" || E->name == "ue_foundation::ue_input_bind_action" ||
            E->name == "ue_input_bind_axis" || E->name == "ue_foundation::ue_input_bind_axis")
            return RewriteInput(E);
        if (E->name == "ue_timer_start" || E->name == "ue_foundation::ue_timer_start" ||
            E->name == "ue_timer_next_tick" || E->name == "ue_foundation::ue_timer_next_tick")
            return RewriteTimer(E);
        if (!ContainerOperation(E->name) || E->arguments.empty())
            return E;
        if (IndexedSelf(E->arguments[0]))
            return MutateIndexed(E, E->arguments[0], nullptr, E);
        auto* Root = StructRoot(E->arguments[0]);
        if (!Root)
            return E;
        try
        {
            const auto* P = Property(Root);
            if (!P || P->bReadOnly)
                DSLFail(TEXT("Cannot mutate readonly self container"));
            das::vector<das::string> Names;
            for (auto* Current = E->arguments[0]; Current != Root;
                 Current = static_cast<das::ExprField*>(Current)->value)
                Names.push_back(static_cast<das::ExprField*>(Current)->name);
            auto Type = RootFieldType(Root);
            das::string Path;
            for (auto It = Names.rbegin(); It != Names.rend(); ++It)
            {
                if (!DSLDirectStruct(Type))
                    DSLFail(TEXT("Container mutation path must cross authored structs"));
                const auto* F = Type->structType->findField(*It);
                if (!F)
                    DSLFail(TEXT("Unknown self container member"));
                for (const auto& A : F->annotation)
                    if (A.name == "ue_property" && A.type == das::Type::tString && A.sValue == "BlueprintReadOnly")
                        DSLFail(TEXT("Cannot mutate readonly struct member"));
                if (!Path.empty())
                    Path += ".";
                Path += *It;
                Type = F->type;
            }
            if (!DSLContainer(Type))
                DSLFail(TEXT("Expected supported self container member"));
            const bool Array = Type->baseType == das::Type::tArray;
            const bool Set = DSLSet(Type);
            const size_t Count = E->arguments.size();
            const bool Push = E->name == "push" || E->name == "push_clone";
            if ((E->name == "clear" ? Count != 1
                 : E->name == "pop" ? (!Array || Count != 1)
                 : Push             ? (!Array || (Count != 2 && Count != 3))
                 : (E->name == "insert" || E->name == "insert_clone")
                     ? (Array || (E->name == "insert_clone" && Set) || Count != (Set ? 2 : 3))
                     : (Array ? (Count != 2 && Count != 3) : Count != 2)))
                DSLFail(TEXT("Unsupported self container operation/arity"));
            const FString Carrier = Path.empty() ? PropertyPath(P) : DSLDirectPath(Type, Folder, Registry);
            DSLEnsureStructHelpers(Type, Carrier, Group, Source->module, GeneratedHelpers, HelperPaths);
            const das::string Name =
                DSLUtf8(TEXT("__ue_container_op_")+
                             FMD5::HashAnsiString(*(DSLSourceValueKey(Type, Carrier) +
                                                    TEXT(":")+ DSLText(E->name) + FString::FromInt(int32(Count)) +
                                                         (Path.empty() ? TEXT(":root"): TEXT(":member")))));
            const bool Result = E->name == "erase" && !Array;
            if (!Source->module->functionsByName.find(das::hash64z(Name.c_str())))
            {
                auto* Fn = new das::Function();
                Fn->at = E->at;
                Fn->name = Name;
                Fn->generated = true;
                Fn->privateFunction = false;
                Fn->result = new das::TypeDecl(Result ? das::Type::tBool : das::Type::tVoid, E->at);
                auto AddArg = [&](const das::string& N, const das::TypeDeclPtr& T)
                {
                    auto* A = new das::Variable();
                    A->at = E->at;
                    A->name = N;
                    A->type = DSLCopyType(T);
                    A->type->ref = false;
                    A->type->constant = true;
                    Fn->arguments.push_back(A);
                };
                AddArg("property", new das::TypeDecl(das::Type::tString, E->at));
                AddArg("member", new das::TypeDecl(das::Type::tString, E->at));
                for (size_t I = 1; I < Count; ++I)
                    AddArg("arg_" + das::to_string(I),
                           Array    ? (Push && I == 1
                                           ? Type->firstType
                                           : static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tInt, E->at)))
                           : I == 1 ? Type->firstType
                                    : Type->secondType);
                auto* Body = new das::ExprBlock();
                Body->at = E->at;
                auto Local = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Init, bool Move)
                {
                    auto* V = new das::Variable();
                    V->at = E->at;
                    V->name = N;
                    V->type = DSLCopyType(T);
                    V->type->ref = false;
                    V->type->constant = false;
                    V->init = Init;
                    V->init_via_move = Move;
                    auto* L = new das::ExprLet();
                    L->at = E->at;
                    L->variables.push_back(V);
                    Body->list.push_back(L);
                };
                Local("container", Type,
                      DSLDirectUnpack(
                          E->at, Type,
                          Path.empty()
                              ? static_cast<das::ExpressionPtr>(
                                    DSLCall(E->at, "ue_get_self_struct", {new das::ExprVar(E->at, "property")}))
                              : static_cast<das::ExpressionPtr>(DSLCall(
                                    E->at, "ue_struct_nested_get",
                                    {DSLCall(E->at, "ue_get_self_struct", {new das::ExprVar(E->at, "property")}),
                                     new das::ExprVar(E->at, "member"), new das::ExprConstInt(E->at, -1)})),
                          Carrier),
                      true);
                auto* Op = DSLCall(E->at, E->name, {new das::ExprVar(E->at, "container")});
                for (size_t I = 1; I < Count; ++I)
                    Op->arguments.push_back(new das::ExprVar(E->at, "arg_" + das::to_string(I)));
                if (Result)
                    Local("result", Fn->result, Op, false);
                else
                    Body->list.push_back(Op);
                Body->list.push_back(
                    Path.empty()
                        ? DSLCall(E->at, "ue_set_self_struct",
                                  {new das::ExprVar(E->at, "property"),
                                   DSLDirectPack(E->at, Type, new das::ExprVar(E->at, "container"), Carrier)})
                        : DSLCall(E->at, "ue_dsl_commit_struct_member",
                                  {new das::ExprVar(E->at, "property"), new das::ExprVar(E->at, "member"),
                                   DSLDirectPack(E->at, Type, new das::ExprVar(E->at, "container"), Carrier)}));
                if (Result)
                    Body->list.push_back(new das::ExprReturn(E->at, new das::ExprVar(E->at, "result")));
                Fn->body = Body;
                if (!Source->module->addFunction(Fn, true))
                    DSLFail(TEXT("Self container operation helper collision"));
                if (Array && ((Push && Count == 3) || (E->name == "erase" && Count > 1)))
                {
                    auto Wide = Fn->clone();
                    for (size_t I = Push ? 3 : 2; I < Wide->arguments.size(); ++I)
                    {
                        Wide->arguments[I]->type = new das::TypeDecl(das::Type::tInt64, E->at);
                        Wide->arguments[I]->type->constant = true;
                    }
                    if (!Source->module->addFunction(Wide, true))
                        DSLFail(TEXT("Self container wide helper collision"));
                }
            }
            auto* Call = DSLCall(E->at, Name,
                                 {new das::ExprConstString(E->at, Root->name), new das::ExprConstString(E->at, Path)});
            for (size_t I = 1; I < Count; ++I)
                Call->arguments.push_back(E->arguments[I]);
            return Call;
        }
        catch (const std::exception& X)
        {
            Error = UTF8_TO_TCHAR(X.what());
            return E;
        }
    }
    virtual void preVisit(das::ExprCopy* E) override { MarkLeft(E->left); }
    virtual void preVisit(das::ExprMove* E) override { MarkLeft(E->left); }
    virtual void preVisit(das::ExprClone* E) override { MarkLeft(E->left); }
    virtual void preVisit(das::ExprOp2* E) override
    {
        if (E->op == "+=" || E->op == "-=" || E->op == "*=" || E->op == "/=")
            MarkLeft(E->left);
    }
    virtual das::ExpressionPtr visit(das::ExprField* E) override
    {
        if (const auto* Leaf = ScalarLeafReads.Find(E))
            return DSLCall(E->at, "__ue_self_struct_get_" + Leaf->Suffix,
                           {new das::ExprConstString(E->at, Leaf->Root), new das::ExprConstString(E->at, Leaf->Path)});
        if (const auto* Native = NativeSceneReads.Find(E))
            return DSLCall(E->at, "__ue_scene_relative_" + Native->Suffix,
                           {DSLDirectPack(E->at, Native->Receiver, E->value, DSLTypedReferencePath(Native->Receiver))});
        return SelfField(E) && !LeftFields.Contains(E) ? Read(E) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprCopy* E) override
    {
        if (IndexedSelf(E->left))
            return WriteIndexed(E, false);
        if (auto* F = SelfField(E->left))
            return Write(E, F, false);
        return StructRoot(E->left) ? WriteLeaf(E, false) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprMove* E) override
    {
        if (IndexedSelf(E->left))
            return WriteIndexed(E, false);
        if (auto* F = SelfField(E->left))
            return Write(E, F, false);
        return StructRoot(E->left) ? WriteLeaf(E, false) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprClone* E) override
    {
        if (IndexedSelf(E->left))
            return WriteIndexed(E, false);
        if (auto* F = SelfField(E->left))
            return Write(E, F, false);
        return StructRoot(E->left) ? WriteLeaf(E, false) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprOp2* E) override
    {
        if ((E->op == "+=" || E->op == "-=" || E->op == "*=" || E->op == "/=") && IndexedSelf(E->left))
            return WriteIndexed(E, true);
        if (LeftFields.Contains(E->left) && StructRoot(E->left))
            return WriteLeaf(E, true);
        auto* F = SelfField(E->left);
        return F && LeftFields.Contains(F) ? Write(E, F, true) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprWith* E) override
    {
        return E->with && E->with->rtti_isVar() && static_cast<das::ExprVar*>(E->with)->name == Self ? E->body : E;
    }
};
