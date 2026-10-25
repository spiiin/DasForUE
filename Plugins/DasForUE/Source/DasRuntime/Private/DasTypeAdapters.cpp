#include "DasTypeAdapters.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

// Defined with the built-in adapters (DasTypeAdaptersBuiltin.inl), where the scalar kinds and the DSL
// facets are visible.
void DasRegisterBuiltinTypeAdapters(FDasTypeAdapters& Registry);

const FDasTypeAdapters& FDasTypeAdapters::Get()
{
    static const FDasTypeAdapters* Registry = []
    {
        auto* R = new FDasTypeAdapters();
        DasRegisterBuiltinTypeAdapters(*R);
        return R;
    }();
    return *Registry;
}
void FDasTypeAdapters::Register(TSharedRef<const IDasTypeAdapter> Adapter, const IDasScriptTypeFacet* Script)
{
    const int32 Index = EntryList.Add({Adapter, Script});
    if (const UScriptStruct* Struct = Adapter->GetStruct())
    {
        check(Adapter->GetKind() == EDasTypeKind::Struct && !ByStruct.Contains(Struct));
        ByStruct.Add(Struct, Index);
        ByPath.Add(Struct->GetPathName(), Index);
    }
    else if (Adapter->IsStructFamily())
    {
        check(Adapter->GetKind() == EDasTypeKind::Struct);
        StructFamilies.Add(Index);
    }
    else
    {
        check(!ByKind.Contains(Adapter->GetKind()));
        ByKind.Add(Adapter->GetKind(), Index);
    }
}
const IDasTypeAdapter* FDasTypeAdapters::Find(const FDasDeclaredType& Type) const
{
    const EDasTypeKind Kind = Type.GetKind();
    if (Kind == EDasTypeKind::Struct)
        return FindStructPath(Type.TypePath);
    const int32* Index = ByKind.Find(Kind);
    if (!Index)
        return nullptr;
    const auto* Adapter = &*EntryList[*Index].Adapter;
    return Type.TypePath.IsEmpty() || Adapter->UsesTypeIdentity() ? Adapter : nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindStruct(const UScriptStruct* Type) const
{
    const int32* Index = Type ? ByStruct.Find(Type) : nullptr;
    return Index ? &*EntryList[*Index].Adapter : Type ? FindStructPath(Type->GetPathName()) : nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindStructPath(const FString& Path) const
{
    if (const int32* Index = ByPath.Find(Path))
        return &*EntryList[*Index].Adapter;
    for (int32 Index : StructFamilies)
        if (EntryList[Index].Adapter->MatchesStructPath(Path))
            return &*EntryList[Index].Adapter;
    return nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindProperty(const FProperty* Property) const
{
    if (auto* Struct = CastField<FStructProperty>(Property))
        return FindStruct(Struct->Struct);
    for (const auto& Entry : EntryList)
        if (!Entry.Adapter->GetStruct() && Entry.Adapter->MatchesProperty(Property))
            return &*Entry.Adapter;
    return nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindDefaultKind(const FString& Kind) const
{
    for (const auto& Entry : EntryList)
        if (const TCHAR* Own = Entry.Adapter->GetDefaultKind(); Own && Kind == Own)
            return &*Entry.Adapter;
    return nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindSignature(EDasMethodSignature Signature) const
{
    for (const auto& Entry : EntryList)
        if (Entry.Adapter->GetSignature() == Signature)
            return &*Entry.Adapter;
    return nullptr;
}
const IDasTypeAdapter* DasMathAdapter(const FString& StructPath)
{
    const auto* Adapter = FDasTypeAdapters::Get().FindStructPath(StructPath);
    return Adapter && Adapter->IsMath() ? Adapter : nullptr;
}
const IDasTypeAdapter* DasLeafValueAdapter(const FString& StructPath)
{
    const auto* Adapter = FDasTypeAdapters::Get().FindStructPath(StructPath);
    return Adapter && Adapter->IsLeafValue() ? Adapter : nullptr;
}
