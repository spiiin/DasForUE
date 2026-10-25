#include "DasTypeAdapters.h"
#include "DasValueKinds.h"
#include "DasInterfaceContracts.h"
#include "DasAuthoredInterfaceClass.h"
#include "DasSoftReferences.h"
#include "DasDefinedEnum.h"
#include "Engine/EngineTypes.h"
#include "UObject/StrongObjectPtr.h"
#include "GameFramework/Actor.h"
#include "UObject/Interface.h"
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
        if (Adapter->GetKind() == EDasTypeKind::Object || Adapter->GetKind() == EDasTypeKind::Class)
        {
            const bool Class = Adapter->GetKind() == EDasTypeKind::Class;
            for (UClass* Base : {UObject::StaticClass(), AActor::StaticClass(), UInterface::StaticClass()})
                for (bool Weak : {false, true})
                {
                    const bool Interface = Base == UInterface::StaticClass();
                    FDasDeclaredType Type;
                    Type.Kind = Class ? TEXT("class") : TEXT("object");
                    Type.TypePath = Base->GetPathName();
                    Type.WeakReference = Weak;
                    auto* Property =
                        CreateDasProperty(FFieldVariant(), TEXT("Reference"), Type, EDasPropertyUsage::Value, Error);
                    if ((Class && (Weak || Interface)) || (!Class && Weak && Interface))
                    {
                        const bool Rejected = !Property && !Error.IsEmpty();
                        delete Property;
                        if (!Rejected)
                        {
                            Error = TEXT("Reference adapter accepted weak class/interface storage");
                            return false;
                        }
                        continue;
                    }
                    FDasDeclaredType RoundTrip;
                    const bool Good =
                        Property && Registry.Find(Type) == Adapter && Registry.FindProperty(Property) == Adapter &&
                        DasPropertyKind(Property) == Type.GetKind() &&
                        bool(CastField<FClassProperty>(Property)) == Class &&
                        bool(CastField<FWeakObjectProperty>(Property)) == Weak &&
                        bool(CastField<FInterfaceProperty>(Property)) == (!Class && Interface) &&
                        DescribeDasProperty(Property, RoundTrip, Error) &&
                        RoundTrip.StorageIdentity() == Type.StorageIdentity() &&
                        DasPropertyMatchesContract(Property,
                                                   Class ? EDasMethodSignature::Class : EDasMethodSignature::Object,
                                                   Type.TypePath) &&
                        !DasPropertyMatchesContract(Property,
                                                    Class ? EDasMethodSignature::Object : EDasMethodSignature::Class,
                                                    Type.TypePath) &&
                        !DasPropertyMatchesContract(Property,
                                                    Class ? EDasMethodSignature::Class : EDasMethodSignature::Object,
                                                    TEXT("/Wrong.Base"));
                    delete Property;
                    if (!Good)
                    {
                        Error = TEXT("Reference adapter lost kind, base constraint or strong/weak storage");
                        return false;
                    }
                }
            FDasDeclaredType Missing;
            Missing.Kind = Class ? TEXT("class") : TEXT("object");
            Missing.TypePath = TEXT("/Script/DasRuntime.MissingReferenceAdapterClass");
            if (ValidateDasPropertyType(Missing, EDasPropertyUsage::Value, Error))
            {
                Error = TEXT("Reference adapter accepted an unknown base class");
                return false;
            }
            Error.Reset();
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
                        RoundTrip.EnumStorage == (Strong ? EDasEnumStorage::Enum8 : EDasEnumStorage::Byte) &&
                        DasPropertyMatchesContract(Property, EDasMethodSignature::Enum, Type.TypePath) &&
                        !DasPropertyMatchesContract(Property, EDasMethodSignature::Byte, Type.TypePath) &&
                        !DasPropertyMatchesContract(Property, EDasMethodSignature::Enum, TEXT("/Wrong.Enum"));
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
        const bool Reachable =
            Registry.Find(Declared) == Adapter && Property && Registry.FindProperty(Property) == Adapter &&
            DasPropertyKind(Property) == Adapter->GetKind() &&
            (!DasSignatureKind(Adapter->GetKind()).IsSet() ||
             DasPropertyMatchesContract(Property, DasSignatureKind(Adapter->GetKind()).GetValue(), TEXT("")));
        delete Property;
        if (!Reachable)
        {
            Error = TEXT("Kind adapter is not reachable: ")+ Name;
            return false;
        }
    }
    // Native fixed arrays and wide enum storage must fail before any class is changed.
    auto* Fixed = new FIntProperty(FFieldVariant(), TEXT("Fixed"));
    Fixed->ArrayDim = 2;
    const bool FixedRejected = !DasPropertyMatchesContract(Fixed, EDasMethodSignature::Int, TEXT(""));
    delete Fixed;
    auto* Wide = new FEnumProperty(FFieldVariant(), TEXT("Wide"));
    Wide->SetEnum(StaticEnum<ESpawnActorCollisionHandlingMethod>());
    Wide->AddCppProperty(new FIntProperty(Wide, TEXT("UnderlyingType")));
    const bool WideRejected =
        !DasPropertyMatchesContract(Wide, EDasMethodSignature::Enum, Wide->GetEnum()->GetPathName());
    delete Wide;
    if (!FixedRejected || !WideRejected)
    {
        Error = TEXT("Interface contract accepted fixed arrays or wide enums");
        return false;
    }
    TStrongObjectPtr<UDasAuthoredInterfaceClass> Interface(NewObject<UDasAuthoredInterfaceClass>());
    Interface->ClassFlags |= CLASS_Interface;
    const FString StableId = TEXT("adapter.Contract");
    const FString NativeId;
    if (DasInterfaceIdentityMatches(Interface.Get()))
    {
        Error = TEXT("Authored interface accepted an empty identity");
        return false;
    }
    Interface->StableId = StableId;
    const bool IdentityAccepted = DasInterfaceIdentityMatches(Interface.Get(), &StableId) &&
                                  !DasInterfaceIdentityMatches(Interface.Get(), &NativeId);
    Interface->StableId = TEXT("adapter.Replaced");
    if (!IdentityAccepted || DasInterfaceIdentityMatches(Interface.Get(), &StableId) ||
        DasInterfaceIdentityMatches(AActor::StaticClass()) ||
        !DasInterfaceIdentityMatches(UInterface::StaticClass(), &NativeId))
    {
        Error = TEXT("Interface contract lost native/authored nominal identity");
        return false;
    }
    Error.Reset();
    return true;
}
