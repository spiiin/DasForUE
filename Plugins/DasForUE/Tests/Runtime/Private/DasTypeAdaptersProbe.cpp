#include "DasTypeAdapters.h"
#include "DasSoftReferences.h"
#include "GameFramework/Actor.h"
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
        if (Adapter->IsStructFamily())
        {
            const int32 EntryCount = Registry.Entries().Num();
            for (UClass* Base : {UObject::StaticClass(), AActor::StaticClass()})
                for (bool ClassReference : {false, true})
                {
                    auto* Carrier = DasSoftCarrier(Base, ClassReference, Error);
                    if (!Carrier)
                        return false;
                    FDasDeclaredType Type;
                    Type.Kind = TEXT("struct");
                    Type.TypePath = Carrier->GetPathName();
                    FProperty* Property =
                        Adapter->MakeProperty(FFieldVariant(), TEXT("Reference"), Type, EDasPropertyUsage::Value);
                    auto* Soft = CastField<FSoftObjectProperty>(Property);
                    auto* SoftClass = CastField<FSoftClassProperty>(Property);
                    FDasDeclaredType RoundTrip;
                    const bool Good =
                        Registry.Find(Type) == Adapter && Registry.FindStruct(Carrier) == Adapter &&
                        Registry.FindProperty(Property) == Adapter && Soft && bool(SoftClass) == ClassReference &&
                        (SoftClass ? SoftClass->MetaClass.Get() : Soft->PropertyClass.Get()) == Base &&
                        Property->HasAnyPropertyFlags(CPF_UObjectWrapper) &&
                        DescribeDasProperty(Property, RoundTrip, Error) && RoundTrip.TypePath == Type.TypePath &&
                        Adapter->GetCarrierProperty(Type.TypePath) == DasSoftProperty(Type.TypePath);
                    delete Property;
                    if (!Good)
                    {
                        Error = TEXT("Soft adapter lost carrier identity or native constraint");
                        return false;
                    }
                }
            if (Registry.Entries().Num() != EntryCount ||
                Registry.FindStructPath(TEXT("/Script/DasRuntime.DasSoftObject_fake")) ||
                                        Registry.FindStructPath(TEXT("/Script/Engine.Actor")))
            {
                Error = TEXT("Soft family mutated registry or accepted an unregistered carrier");
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
