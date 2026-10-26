// Part of DasDSL.inl: daScript views of adapted value types (DasTypeAdapters.h). Lowering asks these
// helpers instead of per-type predicates; the facets themselves live next to their adapters.
const FDasTypeAdapters::FEntry* DSLValueAdapter(const das::TypeDeclPtr& T)
{
    if (!T)
        return nullptr;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (Entry.Script && Entry.Script->MatchesScriptType(T))
            return &Entry;
    return nullptr;
}
bool DSLTypedValue(const das::TypeDeclPtr& T, EDasTypeKind Kind)
{
    const auto* Entry = DSLValueAdapter(T);
    return T && T->baseType != das::Type::tHandle && Entry && Entry->Adapter->GetKind() == Kind;
}
bool DSLNativeValue(const das::TypeDeclPtr& T, EDasTypeKind Kind)
{
    const auto* Entry = DSLValueAdapter(T);
    return Entry && Entry->Adapter->GetKind() == Kind && Entry->Script->IsNativeType(T);
}
const IDasScriptTypeFacet* DSLKindFacet(EDasTypeKind Kind)
{
    FDasDeclaredType Type;
    Type.Kind = DasTypeKindName(Kind);
    const auto* Adapter = FDasTypeAdapters::Get().Find(Type);
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (&*Entry.Adapter == Adapter)
            return Entry.Script;
    return nullptr;
}
const FDasTypeAdapters::FEntry* DSLAdapted(const das::TypeDeclPtr& T)
{
    if (!T || T->baseType != das::Type::tStructure || !T->structType)
        return nullptr;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (Entry.Adapter->GetKind() == EDasTypeKind::Struct && Entry.Script && Entry.Script->MatchesScriptType(T))
            return &Entry;
    return nullptr;
}
const FDasTypeAdapters::FEntry* DSLAdaptedReference(const das::TypeDeclPtr& T)
{
    const auto* Entry = DSLAdapted(T);
    return Entry && Entry->Adapter->IsReferenceValue() ? Entry : nullptr;
}
// Struct path of an adapted view, or empty.
FString DSLAdaptedPath(const das::TypeDeclPtr& T)
{
    const auto* Entry = DSLAdapted(T);
    if (!Entry)
        return {};
    return Entry->Adapter->GetStruct() ? Entry->Adapter->GetStruct()->GetPathName() : Entry->Script->GetTypePath(T);
}
bool DSLAdaptedImplicitDefault(const das::TypeDeclPtr& T)
{
    const auto* Entry = DSLAdapted(T);
    return Entry && Entry->Adapter->GetDefaultPolicy() == EDasAdapterDefault::ImplicitEmpty;
}
das::ExpressionPtr DSLAdaptedPack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLAdapted(T)->Script->PackTyped(At, T, V);
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
        if (Entry.Adapter->GetKind() == EDasTypeKind::Struct && Entry.Script && Entry.Script->MatchesScriptType(T))
            return &Entry;
    return nullptr;
}
// Scalar member of an adapted handle view (Vector x/y/z, Rotator pitch/yaw/roll) usable in place.
bool DSLAdaptedMember(const das::TypeDeclPtr& T, const das::string& Name, EDasMemberUse Use)
{
    const auto* Entry = DSLAdaptedHandle(T);
    return Entry && Entry->Script->HasMember(Name, Use);
}
// Suffix of the handle view's own call transport, or nullptr (also for non-adapted types).
const char* DSLAdaptedBridge(const das::TypeDeclPtr& T, EDasCallBridge Bridge)
{
    const auto* Entry = DSLAdaptedHandle(T);
    return Entry ? Entry->Script->GetCallBridge(Bridge) : nullptr;
}
// Handle view that crosses this kind of call as a struct snapshot (Vector2D; InputActionValue in frames).
bool DSLAdaptedSnapshot(const das::TypeDeclPtr& T, EDasCallBridge Bridge)
{
    return DSLAdaptedHandle(T) && !DSLAdaptedBridge(T, Bridge);
}
// Script facet of a handle view by its annotation name, or of an adapter.
const IDasScriptTypeFacet* DSLAdaptedHandleNamed(const das::string& Name)
{
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (Entry.Script && Entry.Script->GetHandleName() && Name == Entry.Script->GetHandleName())
            return Entry.Script;
    return nullptr;
}
const IDasScriptTypeFacet* DSLAdaptedFacet(const IDasTypeAdapter* Adapter)
{
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
        if (&*Entry.Adapter == Adapter)
            return Entry.Script;
    return nullptr;
}
das::ExpressionPtr DSLAdaptedHandlePack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLAdaptedHandle(T)->Script->PackTyped(At, T, V);
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
            if (auto View = Entry.Script->MakeViewForPath(Path, Group, Module, At))
                return View;
            DSLFail(FString::Printf(TEXT("Import %s for native %s properties"), Entry.Script->GetViewModule(), *Path));
        }
    return nullptr;
}
das::TypeDeclPtr DSLAdaptedNativeView(const FString& Path, das::ModuleGroup& Group, das::Module* Module,
                                      const das::LineInfo& At)
{
    return DSLAdaptedView(Path, EDasScriptViewUse::NativeProperty, Group, Module, At);
}

das::TypeDeclPtr DSLAdaptedMemberType(const das::TypeDeclPtr& T, const das::string& Name)
{
    auto Field = T->annotation->makeFieldType(Name, false);
    if (!Field)
        DSLFail(TEXT("Unknown adapted scalar member"));
    return new das::TypeDecl(Field->baseType, Field->at);
}
