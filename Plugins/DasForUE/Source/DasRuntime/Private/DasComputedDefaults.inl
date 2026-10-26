bool DSLDefaultScalar(das::Type T)
{
    FDasDeclaredType Type;
    Type.Kind = T == das::Type::tInt ? TEXT("int")
                                     : T == das::Type::tFloat
        ? TEXT("float")
        : T == das::Type::tDouble
        ? TEXT("double")
        : T == das::Type::tBool
        ? TEXT("bool")
        : T == das::Type::tString
        ? TEXT("string")
        : TEXT("invalid");
    return QueryDasTypeUse(Type, EDasTypeUse::ComputedDefaultLeaf).IsSupported();
}
FString DSLDefaultLocation(const das::LineInfo& At)
{
    return DSLText(At.fileInfo ? At.fileInfo->name : das::string()) +
           FString::Printf(TEXT(":%u:%u"), At.line, At.column);
}
struct FDSLDefaultSelfVisitor : das::Visitor
{
    TMap<FString, das::ExpressionPtr>& Fields;
    TSet<FString> Stack;
    int32 Nodes = 0;
    explicit FDSLDefaultSelfVisitor(TMap<FString, das::ExpressionPtr>& F) : Fields(F) {}
    virtual das::ExpressionPtr visit(das::ExprField* E) override
    {
        if (!E->value->rtti_isVar() || static_cast<das::ExprVar*>(E->value)->name != "self")
            return E;
        const FString Name = DSLText(E->name);
        const auto* Init = Fields.Find(Name);
        if (!Init || !*Init)
            DSLFail(TEXT("Computed default cannot read this source field: ")+ Name);
        if (Stack.Contains(Name) || ++Nodes > 1024)
            DSLFail(TEXT("Cyclic/oversized computed default dependency: ")+ Name);
        Stack.Add(Name);
        auto Result = (*Init)->clone()->visit(*this);
        Stack.Remove(Name);
        return Result;
    }
};
das::ExpressionPtr DSLComputeDefaultLeaf(const das::StructurePtr& St, das::ExpressionPtr E,
                                         const das::TypeDeclPtr& Type, const FString& Key,
                                         TMap<FString, das::ExpressionPtr>& Fields)
{
    if (!E || E->rtti_isConstant())
        return E;
    if (!DSLComputedDefaults)
        DSLFail(TEXT("Computed defaults require a declaration compilation scope"));
    if (DSLComputedDefaults->Resolved)
    {
        const auto* Found = DSLComputedDefaults->Values.Find(Key);
        if (!Found)
            DSLFail(TEXT("Computed default changed between compilation passes: ")+ Key);
        auto Result = (*Found)->clone();
        Result->at = E->at;
        return Result;
    }
    // RunFolding can execute pure calls during compilation. The collection pass
    // must retain their bodies for the bounded call-graph audit before any eval.
    das::daScriptEnvironment::getBound()->g_Program->policies.no_optimizations = true;
    das::daScriptEnvironment::getBound()->g_Program->policies.no_fold_unsafe = true;
    FDSLDefaultSelfVisitor Self(Fields);
    auto Expression = E->clone()->visit(Self);
    das::FunctionPtr Fn = new das::Function();
    Fn->at = E->at;
    Fn->name = "__ue_computed_default_" + DSLUtf8(FMD5::HashAnsiString(*Key));
    Fn->generated = true;
    Fn->exports = true;
    Fn->result = Type ? DSLCopyType(Type) : new das::TypeDecl(das::Type::autoinfer, E->at);
    Fn->result->ref = false;
    Fn->result->constant = false;
    auto* Body = new das::ExprBlock();
    Body->at = E->at;
    Body->list.push_back(new das::ExprReturn(E->at, Expression));
    Fn->body = Body;
    if (!St->module->addFunction(Fn, true))
        DSLFail(TEXT("Duplicate computed default evaluator: ")+ Key);
    DSLComputedDefaults->Pending.Add({Key, Fn});
    const auto T = Type ? Type->baseType : das::Type::tInt;
    if (T == das::Type::tString)
        return new das::ExprConstString(E->at, "");
    if (T == das::Type::tBool)
        return new das::ExprConstBool(E->at, false);
    if (T == das::Type::tFloat)
        return new das::ExprConstFloat(E->at, 0.f);
    if (T == das::Type::tDouble)
        return new das::ExprConstDouble(E->at, 0.);
    return new das::ExprConstInt(E->at, 0);
}
das::ExpressionPtr DSLComputeDefaultExpression(const das::StructurePtr& St, das::ExpressionPtr E,
                                               const das::TypeDeclPtr& Type, const FString& Key,
                                               TMap<FString, das::ExpressionPtr>& Fields)
{
    if (!E || E->rtti_isConstant())
        return E;
    if (Type && DSLContainer(Type))
    {
        auto Copy = E->clone();
        auto* Items = DSLDefaultItems(Copy, Type);
        if (!Items)
            return Copy;
        for (size_t I = 0; I < Items->values.size(); ++I)
        {
            auto& Item = Items->values[I];
            const FString ItemKey = Key + FString::Printf(TEXT("/item/%u"), uint32(I));
            if (Type->baseType == das::Type::tTable && !DSLSet(Type))
            {
                if (Item->__rtti != das::string("ExprMakeTuple"))
                    DSLFail(TEXT("Map defaults require key => value entries"));
                auto* Pair = static_cast<das::ExprMakeTuple*>(Item);
                if (Pair->values.size() != 2)
                    DSLFail(TEXT("Map default entry requires two values"));
                Pair->values[0] =
                    DSLComputeDefaultExpression(St, Pair->values[0], Type->firstType, ItemKey + TEXT("/key"), Fields);
                Pair->values[1] = DSLComputeDefaultExpression(St, Pair->values[1], Type->secondType,
                                                              ItemKey + TEXT("/value"), Fields);
            }
            else
                Item = DSLComputeDefaultExpression(St, Item, Type->firstType, ItemKey, Fields);
        }
        return Copy;
    }
    if (E->rtti_isCall())
    {
        auto* C = static_cast<das::ExprCall*>(E);
        const auto N = C->name;
        const bool Math = N == "ue_vector" || N == "ue_rotator" || N == "ue_vector2d" || N == "ue_transform" ||
                          N == "ue_transform_identity";
        const bool Marker = N == "ue_asset_default" || N == "ue_class_default" || N == "ue_default_text" ||
                            N == "ue_name_value" || N == "ue_struct_default";
        if (Math || Marker)
        {
            auto Copy = E->clone();
            auto* Call = static_cast<das::ExprCall*>(Copy);
            for (size_t I = 0; I < Call->arguments.size(); ++I)
            {
                das::TypeDeclPtr ArgumentType =
                    new das::TypeDecl(Marker ? das::Type::tString : das::Type::tDouble, E->at);
                const bool Coordinate = Math && N != "ue_transform" && N != "ue_transform_identity";
                if (Math && !Coordinate)
                    ArgumentType = nullptr;
                if (Coordinate)
                {
                    auto* A = Call->arguments[I];
                    if (A->__rtti == das::string("ExprConstInt"))
                        Call->arguments[I] =
                            new das::ExprConstDouble(A->at, static_cast<das::ExprConstInt*>(A)->getValue());
                    else if (A->__rtti == das::string("ExprConstFloat"))
                        Call->arguments[I] =
                            new das::ExprConstDouble(A->at, static_cast<das::ExprConstFloat*>(A)->getValue());
                    else if (!A->rtti_isConstant())
                        Call->arguments[I] = DSLCall(A->at, "double", {A});
                }
                if (N == "ue_struct_default" && I == 0 && !Call->arguments[I]->rtti_isConstant())
                    DSLFail(TEXT("Computed defaults cannot change a struct type identity"));
                const bool ComputedArgument = !Call->arguments[I]->rtti_isConstant();
                Call->arguments[I] = DSLComputeDefaultExpression(St, Call->arguments[I], ArgumentType,
                                                                 Key + FString::Printf(TEXT("/%u"), uint32(I)), Fields);
                if (ComputedArgument && !DSLComputedDefaults->Resolved &&
                    (N == "ue_asset_default" || N == "ue_class_default"))
                    DSLComputedDefaults->PendingReferences.Add(Copy);
            }
            return Copy;
        }
        // Existing declaration-only markers and enum constructors retain their established parser.
        if (N == "ue_object_value" || N == "ue_struct_value" || N == "ue_enum_value")
            return E;
    }
    if (Type && !DSLDefaultScalar(Type->baseType))
        return E;
    return DSLComputeDefaultLeaf(St, E, Type, Key, Fields);
}
void DSLPrepareComputedDefaults(const das::StructurePtr& St)
{
    if (CollectTypesOnly || !DSLComputedDefaults)
        return;
    TMap<FString, das::ExpressionPtr> Fields;
    // Source reads of native scalar defaults use the native host's CDO, never a live object.
    if (auto* Base = DasNativeDefaultBase(DSLNativeDefaultsParentHint))
    {
        UObject* Profile = DasNativeScriptHost(DSLNativeDefaultsParentHint)->GetDefaultObject();
        for (TFieldIterator<FProperty> It(Base); It; ++It)
        {
            auto* P = *It;
            if (!P->HasAnyPropertyFlags(CPF_Edit) || P->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnTemplate))
                continue;
            auto* Data = P->ContainerPtrToValuePtr<void>(Profile);
            das::ExpressionPtr E = nullptr;
            if (auto* Int = CastField<FIntProperty>(P))
                E = new das::ExprConstInt(St->at, Int->GetPropertyValue(Data));
            else if (auto* Float = CastField<FFloatProperty>(P))
                E = new das::ExprConstFloat(St->at, Float->GetPropertyValue(Data));
            else if (auto* Double = CastField<FDoubleProperty>(P))
                E = new das::ExprConstDouble(St->at, Double->GetPropertyValue(Data));
            else if (auto* Bool = CastField<FBoolProperty>(P))
                E = new das::ExprConstBool(St->at, Bool->GetPropertyValue(Data));
            else if (auto* String = CastField<FStrProperty>(P))
                E = new das::ExprConstString(St->at, DSLUtf8(String->GetPropertyValue(Data)));
            if (E)
                Fields.Add(P->GetName(), E);
        }
    }
    auto FieldKey = [](const das::StructurePtr& Owner, const das::string& Name)
    {
        return DSLDefaultLocation(Owner->at) + TEXT(":")+ DSLText(Owner->name) + TEXT("/source/")+ DSLText(Name);
    };
    const FString Prefix = DSLDefaultLocation(St->at) + TEXT(":")+ DSLText(St->name);
    for (auto& Pair : Fields)
        for (auto* Parent = St->parent; Parent; Parent = Parent->parent)
            if (const auto* Original = DSLComputedDefaults->SourceFields.Find(FieldKey(Parent, DSLUtf8(Pair.Key))))
            {
                Pair.Value = *Original;
                break;
            }
    for (const auto& F : St->fields)
        if (!F.classMethod && !F.generated && F.name != "__rtti")
        {
            auto E = F.init;
            if (F.inherited)
                for (auto* Parent = St->parent; Parent; Parent = Parent->parent)
                    if (const auto* Original = DSLComputedDefaults->SourceFields.Find(FieldKey(Parent, F.name)))
                    {
                        E = *Original;
                        break;
                    }
            if (!E && F.type && DSLDefaultScalar(F.type->baseType))
            {
                if (F.type->baseType == das::Type::tString)
                    E = new das::ExprConstString(F.at, "");
                else if (F.type->baseType == das::Type::tBool)
                    E = new das::ExprConstBool(F.at, false);
                else if (F.type->baseType == das::Type::tFloat)
                    E = new das::ExprConstFloat(F.at, 0.f);
                else if (F.type->baseType == das::Type::tDouble)
                    E = new das::ExprConstDouble(F.at, 0.);
                else
                    E = new das::ExprConstInt(F.at, 0);
            }
            Fields.Add(DSLText(F.name), E);
        }
    TSet<FString> Overridden;
    for (const auto& F : St->fields)
    {
        auto* Call = F.init && F.init->rtti_isCall() ? static_cast<das::ExprCall*>(F.init) : nullptr;
        if (!Call || (Call->name != "ue_defaults" && Call->name != "ue_dsl::ue_defaults") ||
            Call->arguments.size() != 1 || !Call->arguments[0]->rtti_isMakeBlock())
            continue;
        auto* Closure = static_cast<das::ExprMakeBlock*>(Call->arguments[0]);
        if (!Closure->block || !Closure->block->rtti_isBlock())
            continue;
        for (auto* E : static_cast<das::ExprBlock*>(Closure->block)->list)
            if (E->rtti_isCopy())
            {
                auto* L = static_cast<das::ExprCopy*>(E)->left;
                if (L->rtti_isField())
                {
                    auto* P = static_cast<das::ExprField*>(L);
                    if (P->value->rtti_isVar() && static_cast<das::ExprVar*>(P->value)->name == "self")
                        Overridden.Add(DSLText(P->name));
                }
            }
    }
    for (auto& F : St->fields)
        if (!F.classMethod && !F.generated && !F.inherited && F.init && !Overridden.Contains(DSLText(F.name)))
        {
            auto* Call = F.init->rtti_isCall() ? static_cast<das::ExprCall*>(F.init) : nullptr;
            if (Call && (Call->name == "ue_defaults" || Call->name == "ue_dsl::ue_defaults"))
                continue;
            FDSLDefaultSelfVisitor Self(Fields);
            auto Source = F.init->clone()->visit(Self);
            F.init = DSLComputeDefaultExpression(St, Source, F.type, Prefix + TEXT("/field/")+ DSLText(F.name), Fields);
            Fields.Add(DSLText(F.name), Source);
        }
    for (auto& F : St->fields)
    {
        auto* Call = F.init && F.init->rtti_isCall() ? static_cast<das::ExprCall*>(F.init) : nullptr;
        if (!Call || (Call->name != "ue_defaults" && Call->name != "ue_dsl::ue_defaults") ||
            Call->arguments.size() != 1 || !Call->arguments[0]->rtti_isMakeBlock())
            continue;
        auto* Closure = static_cast<das::ExprMakeBlock*>(Call->arguments[0]);
        if (!Closure->block || !Closure->block->rtti_isBlock())
            continue;
        auto* Block = static_cast<das::ExprBlock*>(Closure->block);
        uint32 Index = 0;
        for (auto* E : Block->list)
        {
            if (!E->rtti_isCopy())
                continue;
            auto* A = static_cast<das::ExprCopy*>(E);
            auto* P = A->left->rtti_isField() ? static_cast<das::ExprField*>(A->left) : nullptr;
            if (!P)
                continue;
            const bool Own = P->value->rtti_isVar() && static_cast<das::ExprVar*>(P->value)->name == "self";
            das::TypeDeclPtr Type = nullptr;
            const auto* Field = Own ? St->findField(P->name) : nullptr;
            if (Field)
                Type = Field->type;
            FDSLDefaultSelfVisitor Self(Fields);
            auto Source = A->right->clone()->visit(Self);
            const int32 PendingBefore = DSLComputedDefaults->Pending.Num();
            A->right = DSLComputeDefaultExpression(St, Source, Type,
                                                   Prefix + TEXT("/defaults/")+ DSLText(P->name) +
                                                                 FString::Printf(TEXT("/%u"), Index++), Fields);
            const auto* Receiver = P->value->rtti_isField() ? static_cast<das::ExprField*>(P->value) : nullptr;
            const bool Native = Own ? !Field : (Receiver && !St->findField(Receiver->name));
            if (Native && !DSLComputedDefaults->Resolved && DSLComputedDefaults->Pending.Num() != PendingBefore)
                DSLComputedDefaults->PendingNativeExpressions.Add(A->right);
            if (Own)
                Fields.Add(DSLText(P->name), Source);
        }
    }
    // Removed initializers are not reachable from Module's AST root. Preserve
    // independent clones across imported-module sweeps and both compiler passes.
    {
        das::gc_active_scope Keep(&DSLComputedDefaults->SavedExpressions);
        for (const auto& Pair : Fields)
        {
            auto* E = Pair.Value;
            if (E && E->rtti_isCall())
            {
                const auto& Name = static_cast<das::ExprCall*>(E)->name;
                if (Name == "ue_defaults" || Name == "ue_dsl::ue_defaults")
                    continue;
            }
            DSLComputedDefaults->SourceFields.Add(FieldKey(St, DSLUtf8(Pair.Key)), E ? E->clone() : nullptr);
        }
    }
}
// The evaluator admits scalar calculations only. UObject access, global state,
// indirect calls, loops and recursive helpers cannot participate in CDO defaults.
struct FDSLDefaultAudit : das::Visitor
{
    TSet<const das::Function*> Active;
    int32 Nodes = 0;
    void Check(const das::FunctionPtr& F)
    {
        if (!F || Active.Contains(F))
            DSLFail(TEXT("Computed defaults reject recursive or unresolved calls"));
        if (Active.Num() > 64)
            DSLFail(TEXT("Computed defaults exceed helper depth budget"));
        if (F->sideEffectFlags || F->hasUnsafe || F->classParent || F->generator)
            DSLFail(TEXT("Computed defaults require pure scalar helpers: ")+ DSLText(F->name));
        if (!F->result || !DSLDefaultScalar(F->result->baseType))
            DSLFail(TEXT("Computed defaults require scalar helper results"));
        for (const auto& A : F->arguments)
        {
            if (F->builtIn && A->type &&
                (A->type->baseType == das::Type::fakeContext || A->type->baseType == das::Type::fakeLineInfo))
                continue;
            if (!A->type || !DSLDefaultScalar(A->type->baseType) || A->type->ref)
                DSLFail(TEXT("Computed default helper arguments must be scalar values"));
        }
        if (F->builtIn)
        {
            const auto M = F->module ? F->module->name : das::string();
            if (M != "$" && M != "math" && M != "strings")
                DSLFail(TEXT("Computed default native call is not supported: ")+ DSLText(F->name));
        }
        else
        {
            Active.Add(F);
            if (F->body)
                F->body->visit(*this);
            Active.Remove(F);
        }
    }
    virtual void preVisitExpression(das::Expression* E) override
    {
        if (++Nodes > 8192)
            DSLFail(TEXT("Computed defaults exceed expression budget"));
        if (E->rtti_isCallFunc())
            Check(static_cast<das::ExprCallFunc*>(E)->func);
        const das::string N = E->__rtti;
        if (N == "ExprWhile" || N == "ExprFor" || N == "ExprInvoke" || N == "ExprMakeBlock" || N == "ExprNew" ||
            N == "ExprDelete" || N == "ExprUnsafe" || N == "ExprTryCatch" || N == "ExprGoto" ||
            N == "ExprMakeGenerator")
            DSLFail(TEXT("Unsupported computed default operation: ")+ DSLText(N));
    }
    virtual void preVisit(das::ExprCall* E) override { preVisitExpression(E); }
    virtual void preVisit(das::ExprVar* E) override
    {
        preVisitExpression(E);
        if (E->variable && E->variable->global)
            DSLFail(TEXT("Computed defaults cannot read global variables"));
    }
};
void DSLEvaluateComputedDefaults(const das::ProgramPtr& Program, das::TextWriter& Output)
{
    FDSLDefaultAudit Audit;
    for (const auto& P : DSLComputedDefaults->Pending)
    {
        try
        {
            Audit.Check(P.Function);
        }
        catch (const std::exception& X)
        {
            DSLFail(P.Key + TEXT(": ")+ UTF8_TO_TCHAR(X.what()));
        }
    }
    das::Context Context(Program->getContextStackSize());
    Context.skipInitShutdownScript = true;
    // Folding simulation creates executable nodes but deliberately skips global initializers.
    TGuardValue<bool> Folding(Program->folding, true);
    if (!Program->simulate(Context, Output))
        DSLFail(TEXT("Could not simulate computed defaults"));
    Context.heap->setLimit(4 * 1024 * 1024);
    Context.stringHeap->setLimit(4 * 1024 * 1024);
    for (const auto& P : DSLComputedDefaults->Pending)
    {
        auto* Fn = Context.findFunction(P.Function->name.c_str());
        if (!Fn)
            DSLFail(TEXT("Missing computed default evaluator"));
        const auto Value = Context.evalWithCatch(Fn, nullptr);
        if (const char* Error = Context.getException())
            DSLFail(P.Key + TEXT(": computed default failed: ")+ UTF8_TO_TCHAR(Error));
        das::gc_active_scope Keep(&DSLComputedDefaults->SavedExpressions);
        const auto At = P.Function->at;
        das::ExpressionPtr Literal = nullptr;
        switch (P.Function->result->baseType)
        {
        case das::Type::tInt:
            Literal = new das::ExprConstInt(At, das::cast<int32>::to(Value));
            break;
        case das::Type::tFloat:
        {
            const auto V = das::cast<float>::to(Value);
            if (!FMath::IsFinite(V))
                DSLFail(TEXT("Non-finite computed default"));
            Literal = new das::ExprConstFloat(At, V);
            break;
        }
        case das::Type::tDouble:
        {
            const auto V = das::cast<double>::to(Value);
            if (!FMath::IsFinite(V))
                DSLFail(TEXT("Non-finite computed default"));
            Literal = new das::ExprConstDouble(At, V);
            break;
        }
        case das::Type::tBool:
            Literal = new das::ExprConstBool(At, das::cast<bool>::to(Value));
            break;
        case das::Type::tString:
        {
            const char* V = das::cast<char*>::to(Value);
            if (V && FCStringAnsi::Strlen(V) > 65536)
                DSLFail(TEXT("Computed default string too long"));
            Literal = new das::ExprConstString(At, V ? V : "");
            break;
        }
        default:
            DSLFail(TEXT("Unsupported computed default result"));
        }
        DSLComputedDefaults->Values.Add(P.Key, Literal);
    }
}
