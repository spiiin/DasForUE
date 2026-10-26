das::TypeDeclPtr FDasSoftReferenceScriptFacet::MakeViewForPath(const FString& Path, das::ModuleGroup& Group,
                                                               das::Module* Module, const das::LineInfo& At) const
{
    const TCHAR* Source = nullptr;
    for (const auto& V : DasNativeGameplaySoftTypes)
        if (Path == V.Path)
        {
            Source = V.Source;
            break;
        }
    if (!Source)
        DSLFail(TEXT("Soft type not in generated native registry"));
    auto* Alias = new das::TypeDecl(das::Type::alias, At);
    Alias->alias = DSLUtf8(Source);
    auto T = DSLObjectNormalize(Alias, Group, Module);
    if (!DSLSoft(T) || DSLSoftPath(T) != Path)
        DSLFail(TEXT("Import ue_native_gameplay for native soft type: ")+ Path);
    return T;
}
