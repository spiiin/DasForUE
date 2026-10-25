das::TypeDeclPtr FDasEnumScriptFacet::MakeViewForPath(const FString& Path, das::ModuleGroup& Group, das::Module* Module,
                                                      const das::LineInfo& At) const
{
    const TCHAR* Source = nullptr;
    for (const auto& V : DasNativeGameplayEnums)
        if (Path == V.Path)
        {
            Source = V.Source;
            break;
        }
    if (!Source)
        DSLFail(TEXT("Enum not in generated native registry"));
    auto Types = Group.findEnum(DSLUtf8(Source), Module);
    if (Types.size() != 1)
        DSLFail(TEXT("Import ue_native_gameplay for native enum: ")+ Path);
    auto* T = new das::TypeDecl(das::Type::tEnumeration, At);
    T->enumType = Types[0];
    return T;
}
