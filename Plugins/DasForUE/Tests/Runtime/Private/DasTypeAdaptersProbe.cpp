#include "DasTypeAdapters.h"
#include "DasSoftReferences.h"
#include "DasDefinedEnum.h"
#include "Engine/EngineTypes.h"
#include "UObject/StrongObjectPtr.h"
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
        if (Adapter->GetKind() == EDasTypeKind::Enum)
        {
            TStrongObjectPtr<UDasDefinedEnum> Authored(NewObject<UDasDefinedEnum>());
            const FString Prefix = Authored->GetName() + TEXT("::");
            TArray<TPair<FName, int64>> Members = {{FName(*(Prefix + TEXT("Zero"))), 0},
                                                    {FName(*(Prefix + TEXT("Last"))), 254}};
            Authored->SetEnums(Members, UEnum::ECppForm::EnumClass, UEnum::EUnderlyingType::uint8, EEnumFlags::None,
                               UEnum::EAddMaxKeyIfMissing::No);
            UEnum* Native = StaticEnum<ESpawnActorCollisionHandlingMethod>();
            for (UEnum* Enum : {Native, static_cast<UEnum*>(Authored.Get())})
                for (auto Storage : {EDasEnumStorage::Automatic, EDasEnumStorage::Byte, EDasEnumStorage::Enum8})
                {
                    FDasDeclaredType Type;
                    Type.Kind = TEXT("enum");
                    Type.TypePath = Enum->GetPathName();
                    Type.EnumStorage = Storage;
                    auto* Property =
                        CreateDasProperty(FFieldVariant(), TEXT("EnumValue"), Type, EDasPropertyUsage::Value, Error);
                    const bool Strong =
                        Storage == EDasEnumStorage::Enum8 || (Storage == EDasEnumStorage::Automatic && Enum == Native);
                    FDasDeclaredType RoundTrip;
                    const bool Good =
                        Property && Registry.Find(Type) == Adapter && Registry.FindProperty(Property) == Adapter &&
                        DasPropertyKind(Property) == EDasTypeKind::Enum &&
                        bool(CastField<FEnumProperty>(Property)) == Strong &&
                        DescribeDasProperty(Property, RoundTrip, Error) && RoundTrip.TypePath == Type.TypePath &&
                        RoundTrip.EnumStorage == (Strong ? EDasEnumStorage::Enum8 : EDasEnumStorage::Byte);
                    delete Property;
                    if (!Good)
                    {
                        Error = TEXT("Enum adapter lost nominal identity or byte/strong storage");
                        return false;
                    }
                }
            FDasDeclaredType Missing;
            Missing.Kind = TEXT("enum");
            Missing.TypePath = TEXT("/Script/DasRuntime.MissingAdapterEnum");
            if (ValidateDasPropertyType(Missing, EDasPropertyUsage::Value, Error))
            {
                Error = TEXT("Enum adapter accepted an unknown enum identity");
                return false;
            }
            Members.Last().Value = 255;
            Authored->SetEnums(Members, UEnum::ECppForm::EnumClass, UEnum::EUnderlyingType::uint8, EEnumFlags::None,
                               UEnum::EAddMaxKeyIfMissing::No);
            Missing.TypePath = Authored->GetPathName();
            if (ValidateDasPropertyType(Missing, EDasPropertyUsage::Value, Error))
            {
                Error = TEXT("Enum adapter accepted a value above 254");
                return false;
            }
            Error.Reset();
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
