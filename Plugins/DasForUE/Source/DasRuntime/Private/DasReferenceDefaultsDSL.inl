// Reference defaults retain the computed-default lifecycle and literal asset wire.
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
bool FDasObjectClassScriptFacet::DeclareProperty(const das::TypeDeclPtr& Type, das::Structure::FieldDeclaration& F,
                                                 const FDasScriptTypeScope& TypeScope, FDasDeclaredProperty& Out) const
{
    if (Class)
    {
        if (DSLTypedClass(Type))
        {
            const FString Inferred = DSLTypedClassPath(Type);
            if (!Out.Type.TypePath.IsEmpty() && Out.Type.TypePath != Inferred)
                DSLFail(TEXT("Conflicting typed class property constraint"));
            Out.Type.TypePath = Inferred;
        }
        auto* Base = FindObject<UClass>(nullptr, *Out.Type.TypePath);
        if (!Out.Type.TypePath.StartsWith(TEXT("/Script/")) || !Base || Base->HasAnyClassFlags(CLASS_Interface))
            DSLFail(TEXT("Class property requires ue_class_base with loaded native base"));
        Out.Default = DSLReferencePropertyDefault(F, true);
    }
    else if (DSLTypedReference(Type))
    {
        Out.Type.TypePath = DSLTypedReferencePath(Type);
        if (DSLAuthoredView(Type))
            Out.Type.AuthoredId = DSLReferenceDeclaration(Type).Id;
        Out.Default = DSLReferencePropertyDefault(F, false);
    }
    else
    {
        if (!F.init || !F.init->rtti_isCall())
            DSLFail(TEXT("Object field requires literal ue_object_value(native_class_path) initializer"));
        auto* Call = static_cast<das::ExprCall*>(F.init);
        if (Call->name != "ue_object_value" || Call->arguments.size() != 1 ||
            Call->arguments[0]->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Object field initializer requires one literal native class path"));
        Out.Type.TypePath = DSLResolveType(
            EDSLTypeKind::Class, DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text), TypeScope);
        static_cast<das::ExprConstString*>(Call->arguments[0])->text = DSLUtf8(Out.Type.TypePath);
        Out.Default.Kind = TEXT("null");
    }
    return true;
}

TSharedPtr<FJsonValue> DSLReferenceAssetDefault(das::ExpressionPtr E)
{
    auto* Call = E->rtti_isCall() ? static_cast<das::ExprCall*>(E) : nullptr;
    if (!Call || Call->name != "ue_asset_default" || Call->arguments.size() != 1 ||
        Call->arguments[0]->__rtti != das::string("ExprConstString"))
        DSLFail(TEXT("Container assets require ue_asset_default(path)"));
    auto Object = MakeShared<FJsonObject>();
    Object->SetStringField(TEXT("asset"), DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text));
    return MakeShared<FJsonValueObject>(Object);
}
TSharedPtr<FJsonValue> FDasObjectClassScriptFacet::DefaultJson(const das::TypeDeclPtr& T, das::ExpressionPtr E,
                                                               das::ModuleGroup&, das::Module*) const
{
    if (Class || !DSLTypedReference(T))
        return nullptr;
    if (DSLAuthoredView(T))
        DSLFail(TEXT("Container asset defaults require native reference types"));
    return DSLReferenceAssetDefault(E);
}
