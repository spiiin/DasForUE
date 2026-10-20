// Part of DasDSL.inl: daScript views of adapted value types (DasTypeAdapters.h). Lowering asks these
// helpers instead of per-type predicates; the facets themselves live next to their adapters.
const FDasTypeAdapters::FEntry* DSLAdapted(const das::TypeDeclPtr& T)
{
    if (!T || T->baseType != das::Type::tStructure || !T->structType)
        return nullptr;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (Entry.Script && Entry.Script->MatchesScriptType(T))
            return &Entry;
    return nullptr;
}
// Struct path of an adapted view, or empty.
FString DSLAdaptedPath(const das::TypeDeclPtr& T)
{
    const auto* Entry = DSLAdapted(T);
    return Entry ? Entry->Adapter->GetStruct()->GetPathName() : FString();
}
bool DSLAdaptedImplicitDefault(const das::TypeDeclPtr& T)
{
    const auto* Entry = DSLAdapted(T);
    return Entry && Entry->Adapter->GetDefaultPolicy() == EDasAdapterDefault::ImplicitEmpty;
}
das::ExpressionPtr DSLAdaptedPack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLAdapted(T)->Script->Pack(At, V);
}
das::ExpressionPtr DSLAdaptedUnpack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLAdapted(T)->Script->Unpack(At, T, V);
}
// View of a native struct property whose struct has a script facet; nullptr when the struct is not adapted.
das::TypeDeclPtr DSLAdaptedNativeView(const FString& Path, das::ModuleGroup& Group, das::Module* Module,
                                      const das::LineInfo& At)
{
    const auto* Adapter = FDasTypeAdapters::Get().FindStructPath(Path);
    const IDasScriptTypeFacet* Script = nullptr;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (&*Entry.Adapter == Adapter)
            Script = Entry.Script;
    if (!Script)
        return nullptr;
    auto Views = Group.findStructure(Script->GetViewType(), Module);
    if (Views.size() != 1)
        DSLFail(FString::Printf(TEXT("Import %s for native %s properties"), Script->GetViewModule(),
                                     *Adapter->GetStruct()->GetName()));
    auto* T = new das::TypeDecl(das::Type::tStructure, At);
    T->structType = Views[0];
    return T;
}
