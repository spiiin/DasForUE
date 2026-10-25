#include "DasTypeAdapters.h"
#include "UObject/Class.h"
#include "UObject/UnrealType.h"

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
