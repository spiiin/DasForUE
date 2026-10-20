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
    const int32* Index = Type.TypePath.IsEmpty() ? ByKind.Find(Kind) : nullptr;
    return Index ? &*EntryList[*Index].Adapter : nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindStruct(const UScriptStruct* Type) const
{
    const int32* Index = Type ? ByStruct.Find(Type) : nullptr;
    return Index ? &*EntryList[*Index].Adapter : nullptr;
}
const IDasTypeAdapter* FDasTypeAdapters::FindStructPath(const FString& Path) const
{
    const int32* Index = ByPath.Find(Path);
    return Index ? &*EntryList[*Index].Adapter : nullptr;
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
const IDasTypeAdapter* DasLeafValueAdapter(const FString& StructPath)
{
    const auto* Adapter = FDasTypeAdapters::Get().FindStructPath(StructPath);
    return Adapter && Adapter->IsLeafValue() ? Adapter : nullptr;
}

// Every adapter is reachable by its identity, and kind adapters recognise the properties they create.
bool CheckDasTypeAdapters(FString& Error)
{
    const auto& Registry = FDasTypeAdapters::Get();
    for (const auto& Entry : Registry.Entries())
    {
        const IDasTypeAdapter* Adapter = &*Entry.Adapter;
        const FString Name = DasTypeKindName(Adapter->GetKind());
        if (const UScriptStruct* Struct = Adapter->GetStruct())
        {
            FDasDeclaredType Declared;
            Declared.Kind = TEXT("struct");
            Declared.TypePath = Struct->GetPathName();
            if (Registry.FindStruct(Struct) != Adapter || Registry.Find(Declared) != Adapter)
            {
                Error = TEXT("Struct adapter is not reachable: ")+ Struct->GetPathName();
                return false;
            }
            continue;
        }
        FDasDeclaredType Declared;
        Declared.Kind = Name;
        FProperty* Property = Adapter->MakeProperty(FFieldVariant(), TEXT("Probe"), Declared, EDasPropertyUsage::Value);
        const bool Reachable = Registry.Find(Declared) == Adapter && Property &&
                               Registry.FindProperty(Property) == Adapter &&
                               DasPropertyKind(Property) == Adapter->GetKind();
        delete Property;
        if (!Reachable)
        {
            Error = TEXT("Kind adapter is not reachable: ")+ Name;
            return false;
        }
    }
    return true;
}
