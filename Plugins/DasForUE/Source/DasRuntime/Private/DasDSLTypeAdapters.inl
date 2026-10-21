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
// Handle views (math structs, InputActionValue); DSLAdapted keeps to struct views.
const FDasTypeAdapters::FEntry* DSLAdaptedHandle(const das::TypeDeclPtr& T)
{
    if (!T || T->baseType != das::Type::tHandle || !T->annotation)
        return nullptr;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (Entry.Script && Entry.Script->MatchesScriptType(T))
            return &Entry;
    return nullptr;
}
// Scalar member of an adapted handle view (Vector x/y/z, Rotator pitch/yaw/roll) usable in place.
bool DSLAdaptedMember(const das::TypeDeclPtr& T, const das::string& Name, EDasMemberUse Use)
{
    const auto* Entry = DSLAdaptedHandle(T);
    return Entry && Entry->Script->HasMember(Name, Use);
}
das::ExpressionPtr DSLAdaptedHandlePack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLAdaptedHandle(T)->Script->Pack(At, V);
}
das::ExpressionPtr DSLAdaptedHandleUnpack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLAdaptedHandle(T)->Script->Unpack(At, T, V);
}
// View type of an adapted struct property for one use, or nullptr when the struct has no view there.
das::TypeDeclPtr DSLAdaptedView(const FString& Path, EDasScriptViewUse Use, das::ModuleGroup& Group,
                                das::Module* Module, const das::LineInfo& At)
{
    const auto* Adapter = FDasTypeAdapters::Get().FindStructPath(Path);
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (&*Entry.Adapter == Adapter && Entry.Script && EnumHasAnyFlags(Entry.Script->GetViewUses(), Use))
        {
            if (auto View = Entry.Script->MakeView(Group, Module, At))
                return View;
            DSLFail(FString::Printf(TEXT("Import %s for native %s properties"), Entry.Script->GetViewModule(),
                                         *Adapter->GetStruct()->GetName()));
        }
    return nullptr;
}
das::TypeDeclPtr DSLAdaptedNativeView(const FString& Path, das::ModuleGroup& Group, das::Module* Module,
                                      const das::LineInfo& At)
{
    return DSLAdaptedView(Path, EDasScriptViewUse::NativeProperty, Group, Module, At);
}
