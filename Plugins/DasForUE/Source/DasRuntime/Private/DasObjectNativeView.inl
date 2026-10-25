das::TypeDeclPtr FDasObjectClassScriptFacet::MakeViewForPath(const FString& Path, das::ModuleGroup& Group,
                                                             das::Module* Module, const das::LineInfo& At) const
{
    if (Class)
        return MakeView(Group, Module, At);
    const TCHAR* Source = nullptr;
    for (const auto& V : DasNativeGameplayViews)
        if (Path == V.Class)
        {
            Source = V.Source;
            break;
        }
    if (!Source)
        DSLFail(TEXT("Native object type is not in generated registry: ")+ Path);
    auto Types = Group.findStructure(DSLUtf8(Source), Module);
    if (Types.size() != 1)
        DSLFail(TEXT("Import ue_native_gameplay for native view: ")+ FString(Source));
    auto* T = new das::TypeDecl(das::Type::tStructure, At);
    T->structType = Types[0];
    return T;
}
