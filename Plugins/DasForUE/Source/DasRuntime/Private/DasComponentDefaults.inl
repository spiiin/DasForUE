
struct FDSLComponentDefault
{
    bool Native = false;
    FString Component, Property;
    FDasDeclaredLiteral Value;
    das::ExpressionPtr Expression;
};
double DSLDefaultNumber(das::ExpressionPtr E)
{
    double V = 0;
    if (E->__rtti == das::string("ExprConstInt"))
        V = static_cast<das::ExprConstInt*>(E)->getValue();
    else if (E->__rtti == das::string("ExprConstFloat"))
        V = static_cast<das::ExprConstFloat*>(E)->getValue();
    else if (E->__rtti == das::string("ExprConstDouble"))
        V = static_cast<das::ExprConstDouble*>(E)->getValue();
    else
        DSLFail(TEXT("Component defaults require numeric literals"));
    if (!FMath::IsFinite(V))
        DSLFail(TEXT("Component defaults require finite numbers"));
    return V;
}
// These constructors are parsed as data. No daScript code executes during import.
FDasDeclaredLiteral DSLDefaultMath(das::ExpressionPtr E)
{
    if (!E || !E->rtti_isCall())
        DSLFail(TEXT("Math defaults require literal constructors"));
    auto* Call = static_cast<das::ExprCall*>(E);
    FDasDeclaredLiteral V;
    UScriptStruct* Type = nullptr;
    FVector Vector;
    FVector2D Vector2;
    FRotator Rotation;
    FTransform Transform = FTransform::Identity;
    const void* Memory = nullptr;
    if (Call->name == "ue_vector" || Call->name == "ue_rotator")
    {
        if (Call->arguments.size() != 3)
            DSLFail(TEXT("Expected three numeric coordinates"));
        Vector = FVector(DSLDefaultNumber(Call->arguments[0]), DSLDefaultNumber(Call->arguments[1]),
                         DSLDefaultNumber(Call->arguments[2]));
        if (Call->name == "ue_vector")
        {
            V.Kind = TEXT("vector");
            Type = TBaseStructure<FVector>::Get();
            Memory = &Vector;
        }
        else
        {
            V.Kind = TEXT("rotator");
            Rotation = FRotator(Vector.X, Vector.Y, Vector.Z);
            Type = TBaseStructure<FRotator>::Get();
            Memory = &Rotation;
        }
    }
    else if (Call->name == "ue_vector2d")
    {
        if (Call->arguments.size() != 2)
            DSLFail(TEXT("Expected two numeric coordinates"));
        V.Kind = TEXT("vector2d");
        Vector2 = FVector2D(DSLDefaultNumber(Call->arguments[0]), DSLDefaultNumber(Call->arguments[1]));
        Type = TBaseStructure<FVector2D>::Get();
        Memory = &Vector2;
    }
    else if (Call->name == "ue_transform" || Call->name == "ue_transform_identity")
    {
        V.Kind = TEXT("transform");
        Type = TBaseStructure<FTransform>::Get();
        Memory = &Transform;
        if (Call->name == "ue_transform_identity")
        {
            if (!Call->arguments.empty())
                DSLFail(TEXT("Identity transform takes no arguments"));
        }
        else
        {
            if (Call->arguments.size() != 3)
                DSLFail(TEXT("Transform requires literal translation, rotation and scale"));
            auto T = DSLDefaultMath(Call->arguments[0]), R = DSLDefaultMath(Call->arguments[1]),
                 S = DSLDefaultMath(Call->arguments[2]);
            if (T.Kind != TEXT("vector") || R.Kind != TEXT("rotator") || S.Kind != TEXT("vector"))
                DSLFail(TEXT("Transform requires vector, rotator, vector"));
            FInstancedStruct Translation, Scale, Rotator;
            FString Issue;
            if (!ParseDasStructDefault(
                    TEXT("/Script/CoreUObject.Vector"), T.String, Translation, Issue) ||
                    !ParseDasStructDefault(TEXT("/Script/CoreUObject.Vector"), S.String, Scale, Issue) ||
                                           !ParseDasStructDefault(TEXT("/Script/CoreUObject.Rotator"), R.String,
                                                                       Rotator, Issue))
                DSLFail(Issue);
            Transform = FTransform(Rotator.Get<FRotator>(), Translation.Get<FVector>(), Scale.Get<FVector>());
            if (Transform.ContainsNaN() || !Transform.IsRotationNormalized())
                DSLFail(TEXT("Invalid transform default"));
        }
    }
    else
        DSLFail(TEXT("Unsupported default constructor: ")+ DSLText(Call->name));
    Type->ExportText(V.String, Memory, nullptr, nullptr, PPF_None, nullptr);
    return V;
}
FString DSLMathPropertyDefault(const das::Structure::FieldDeclaration& Field)
{
    const FString Path = DSLMathPath(Field.type);
    if (Field.init)
    {
        const auto V = DSLDefaultMath(Field.init);
        const FString Expected =
            Path == TEXT("/Script/CoreUObject.Vector")
            ? TEXT("vector")
            : Path == TEXT("/Script/CoreUObject.Rotator")
                  ? TEXT("rotator")
                  : Path == TEXT("/Script/CoreUObject.Vector2D")? TEXT("vector2d"): TEXT("transform");
        if (V.Kind != Expected)
            DSLFail(TEXT("Math default constructor does not match property type"));
        return V.String;
    }
    FVector Vector = FVector::ZeroVector;
    FRotator Rotation = FRotator::ZeroRotator;
    FVector2D Vector2 = FVector2D::ZeroVector;
    FTransform Transform = FTransform::Identity;
    UScriptStruct* Type = Path == TEXT("/Script/CoreUObject.Vector")
                                       ? TBaseStructure<FVector>::Get()
                                       : Path == TEXT("/Script/CoreUObject.Rotator")
                                                      ? TBaseStructure<FRotator>::Get()
                                                      : Path == TEXT("/Script/CoreUObject.Vector2D")
                                                                     ? TBaseStructure<FVector2D>::Get()
                                                                     : TBaseStructure<FTransform>::Get();
    const void* Memory = Path == TEXT("/Script/CoreUObject.Vector")
                                      ? static_cast<const void*>(&Vector)
                                      : Path == TEXT("/Script/CoreUObject.Rotator")
                                                     ? static_cast<const void*>(&Rotation)
                                                     : Path == TEXT("/Script/CoreUObject.Vector2D")
                                                                    ? static_cast<const void*>(&Vector2)
                                                                    : static_cast<const void*>(&Transform);
    FString Text;
    Type->ExportText(Text, Memory, nullptr, nullptr, PPF_None, nullptr);
    return Text;
}
TArray<FDSLComponentDefault> DSLCollectComponentDefaults(const das::StructurePtr& S)
{
    TArray<FDSLComponentDefault> Result;
    das::vector<das::Structure::FieldDeclaration> Fields;
    for (const auto& F : S->fields)
    {
        auto* Call = F.init && F.init->rtti_isCall() ? static_cast<das::ExprCall*>(F.init) : nullptr;
        if (!Call || (Call->name != "ue_defaults" && Call->name != "ue_dsl::ue_defaults"))
        {
            Fields.push_back(F);
            continue;
        }
        if (!F.annotation.empty() || Call->arguments.size() != 1 || !Call->arguments[0]->rtti_isMakeBlock())
            DSLFail(TEXT("ue_defaults requires a plain trailing block"));
        auto* Closure = static_cast<das::ExprMakeBlock*>(Call->arguments[0]);
        auto* Block =
            Closure->block && Closure->block->rtti_isBlock() ? static_cast<das::ExprBlock*>(Closure->block) : nullptr;
        if (!Block || !Block->arguments.empty() || !Block->finalList.empty() || Block->list.empty())
            DSLFail(TEXT("ue_defaults requires nonempty assignments"));
        for (auto* E : Block->list)
        {
            if (!E->rtti_isCopy())
                DSLFail(TEXT("ue_defaults accepts only component property assignments"));
            auto* Assignment = static_cast<das::ExprCopy*>(E);
            auto* Property =
                Assignment->left->rtti_isField() ? static_cast<das::ExprField*>(Assignment->left) : nullptr;
            auto* Component =
                Property && Property->value->rtti_isField() ? static_cast<das::ExprField*>(Property->value) : nullptr;
            const bool Own = Property && Property->value->rtti_isVar() &&
                             static_cast<das::ExprVar*>(Property->value)->name == "self";
            if (!Own && (!Component || !Component->value->rtti_isVar() ||
                         static_cast<das::ExprVar*>(Component->value)->name != "self"))
                DSLFail(TEXT("Expected self.Property or self.Component.Property in ue_defaults"));
            FDSLComponentDefault D;
            D.Property = DSLText(Property->name);
            D.Expression = Assignment->right;
            D.Native = Own ? !S->findField(Property->name) : !S->findField(Component->name);
            if (Own && !D.Native)
            {
                Result.Add(MoveTemp(D));
                continue;
            }
            if (!Own)
                D.Component = DSLText(Component->name);
            auto* V = Assignment->right;
            if (DSLComputedDefaults && !DSLComputedDefaults->Resolved &&
                DSLComputedDefaults->PendingNativeExpressions.Contains(V))
                continue;
            // Type/interface discovery erases class-default syntax without evaluating it.
            if (CollectTypesOnly)
            {
                Result.Add(MoveTemp(D));
                continue;
            }
            if (DSLComputedDefaults && !DSLComputedDefaults->Resolved &&
                DSLComputedDefaults->PendingReferences.Contains(V))
                continue;
            if (V->__rtti == das::string("ExprConstBool"))
            {
                D.Value.Kind = TEXT("bool");
                D.Value.Boolean = static_cast<das::ExprConstBool*>(V)->getValue();
            }
            else if (V->__rtti == das::string("ExprConstString"))
            {
                D.Value.Kind = TEXT("string");
                D.Value.String = DSLText(static_cast<das::ExprConstString*>(V)->text);
            }
            else if (V->rtti_isCall())
            {
                auto* CallValue = static_cast<das::ExprCall*>(V);
                if (CallValue->name == "ue_class_default" || CallValue->name == "ue_asset_default" ||
                    CallValue->name == "ue_default_text")
                {
                    if (CallValue->arguments.size() != 1 ||
                        CallValue->arguments[0]->__rtti != das::string("ExprConstString"))
                        DSLFail(TEXT("Reference defaults require a literal object path"));
                    D.Value.Kind = CallValue->name == "ue_class_default" ? TEXT("class")
                                                                         : CallValue->name == "ue_default_text"
                        ? TEXT("struct")
                        : TEXT("asset");
                    D.Value.String = DSLText(static_cast<das::ExprConstString*>(CallValue->arguments[0])->text);
                }
                else
                {
                    D.Value = DSLDefaultMath(V);
                }
            }
            else
            {
                D.Value.Kind = TEXT("number");
                D.Value.Number = DSLDefaultNumber(V);
            }
            Result.Add(MoveTemp(D));
        }
    }
    S->fields = std::move(Fields);
    return Result;
}
void DSLApplyComponentDefaults(FDasDeclaredClass& C, const TArray<FDSLComponentDefault>& Defaults,
                               const TArray<FDasDeclaredComponent>& Inherited)
{
    TSet<FString> Seen;
    for (const auto& D : Defaults)
    {
        if (D.Native)
        {
            auto* G = C.NativeDefaults.FindByPredicate([&](const auto& V) { return V.Component == D.Component; });
            if (!G)
            {
                FDasNativeDefaultGroup N;
                N.Component = D.Component;
                C.NativeDefaults.Add(N);
                G = &C.NativeDefaults.Last();
            }
            if (G->Defaults.Contains(D.Property))
            {
                DSLFail(TEXT("Duplicate native default: ")+ D.Component + TEXT(".")+ D.Property);
            }
            G->Defaults.Add(D.Property, D.Value);
            continue;
        }
        if (D.Component.IsEmpty())
            continue;
        auto* Component = C.Components.FindByPredicate([&](const auto& V) { return V.Name == D.Component; });
        if (!Component)
            DSLFail(TEXT("Unknown defaults component: ")+ D.Component);
        const FString Key = D.Component + TEXT(".")+ D.Property;
        if (Seen.Contains(Key.ToLower()))
            DSLFail(TEXT("Duplicate component default: ")+ Key);
        Seen.Add(Key.ToLower());
        if (Inherited.ContainsByPredicate([&](const auto& V) { return V.Id == Component->Id; }))
        {
            auto* Override = C.ComponentOverrides.FindByPredicate([&](const auto& V) { return V.Id == Component->Id; });
            if (!Override)
            {
                FDasComponentOverride New;
                New.Id = Component->Id;
                New.Name = Component->Name;
                C.ComponentOverrides.Add(New);
                Override = &C.ComponentOverrides.Last();
            }
            Override->Defaults.Add(D.Property, D.Value);
        }
        else if (Component->Defaults.Contains(D.Property))
            DSLFail(TEXT("Duplicate component default: ")+ Key);
        Component->Defaults.Add(D.Property, D.Value);
    }
}

void DSLPrepareClassDefaults(const das::StructurePtr& S, const TArray<FDSLComponentDefault>& Defaults)
{
    TSet<FString> Seen;
    for (const auto& D : Defaults)
        if (D.Component.IsEmpty() && !D.Native)
        {
            const FString Key = D.Property.ToLower();
            if (Seen.Contains(Key))
                DSLFail(TEXT("Duplicate class default: ")+ D.Property);
            Seen.Add(Key);
            auto It = std::find_if(S->fields.begin(), S->fields.end(), [&](const auto& F)
                                   { return DSLText(F.name) == D.Property && !F.classMethod && !F.generated; });
            if (It == S->fields.end() || std::any_of(It->annotation.begin(), It->annotation.end(),
                                                     [](const auto& A) { return A.name == "ue_component"; }))
                DSLFail(TEXT("Class default requires a reflected property: ")+ D.Property);
            // A defaults block explicitly takes precedence over the field initializer.
            It->init = D.Expression->clone();
            if (DSLComputedDefaults && DSLComputedDefaults->PendingReferences.Contains(D.Expression))
                DSLComputedDefaults->PendingReferences.Add(It->init);
        }
}
FDasDeclaredLiteral DSLReferencePropertyDefault(das::Structure::FieldDeclaration& F, bool Class)
{
    FDasDeclaredLiteral V;
    V.Kind = TEXT("null");
    if (!F.init)
        return V;
    auto* Call = F.init->rtti_isCall() ? static_cast<das::ExprCall*>(F.init) : nullptr;
    if (!Call || Call->name != (Class ? "ue_class_default" : "ue_asset_default") || Call->arguments.size() != 1 ||
        Call->arguments[0]->__rtti != das::string("ExprConstString"))
        DSLFail(TEXT("Reference defaults require ue_class_default/ue_asset_default with a literal path"));
    if (DSLComputedDefaults && !DSLComputedDefaults->Resolved && DSLComputedDefaults->PendingReferences.Contains(Call))
    {
        F.init = nullptr;
        return V;
    }
    V.Kind = TEXT("string");
    V.String = DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text);
    F.init = nullptr;
    return V;
}
