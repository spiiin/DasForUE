#include "DasTypeContracts.h"
#include "DasCallbackTypes.h"
#include "DasDelegateSignatures.h"
#include "UObject/Package.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Widget.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "PhysicsEngine/ConstraintInstanceBlueprintLibrary.h"
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
        if (const auto Slots = Adapter->GetContainerSlots(); !Slots.IsEmpty())
        {
            FDasDeclaredType Type;
            Type.Kind = Name;
            for (const auto& Slot : Slots)
                Type.*Slot.Descriptor = MakeShared<FDasDeclaredType>(); // int children
            auto* Property = Adapter->MakeProperty(FFieldVariant(), TEXT("Container"), Type, EDasPropertyUsage::Value);
            FDasDeclaredType Described;
            bool Good = Property && Registry.Find(Type) == Adapter && Registry.FindProperty(Property) == Adapter &&
                        !Adapter->SupportsContainerValue() && DescribeDasProperty(Property, Described, Error) &&
                        Described.StorageIdentity() == Type.StorageIdentity();
            if (Property)
                for (const auto& Slot : Slots)
                {
                    const auto* Child = Slot.ReadProperty(Property);
                    Good &= Child && (!Slot.bHashKey || Child->HasAnyPropertyFlags(CPF_HasGetValueTypeHash));
                }
            delete Property;
            auto* Delegate =
                Adapter->MakeProperty(FFieldVariant(), TEXT("Input"), Type, EDasPropertyUsage::DelegateParameter);
            Good &= Delegate != nullptr;
            if (Delegate)
                for (const auto& Slot : Slots)
                {
                    const auto* Child = Slot.ReadProperty(Delegate);
                    const FName Expected =
                        Adapter->GetKind() == EDasTypeKind::Array ? TEXT("Inner")
                                                                  : Slot.Descriptor == &FDasDeclaredType::Key
                        ? TEXT("Key")
                        : Slot.Descriptor == &FDasDeclaredType::Value
                        ? TEXT("Value")
                        : TEXT("Element");
                    Good &= Child && Child->GetFName() == Expected;
                }
            delete Delegate;
            if (!Good)
            {
                Error = TEXT("Container adapter lost descriptor slots or child hash policy: ") + Name;
                return false;
            }
            continue;
        }
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


// Validate the report's representative positives through the real property factory too. A negative
// validator decision must not create a property. No independent table of expected per-kind answers.
bool CheckDasTypeAdapterSupport(FString& Error)
{
    const auto& Registry = FDasTypeAdapters::Get();
    int32 Positive = 0, Negative = 0;
    for (const auto& Entry : Registry.Entries())
    {
        TArray<FDasDeclaredType> Samples;
        Entry.Adapter->GetSupportSamples(Samples);
        if (Samples.IsEmpty())
        {
            Error = TEXT("Built-in adapter has no support samples");
            return false;
        }
        for (const auto& Sample : Samples)
        {
            if (Registry.Find(Sample) != &Entry.Adapter.Get())
            {
                Error = TEXT("Support sample resolves to a different adapter: ") + Sample.StorageIdentity();
                return false;
            }
            TArray<FDasDeclaredType> Types{Sample};
            for (auto Kind : {EDasTypeKind::Array, EDasTypeKind::Set, EDasTypeKind::Map})
                for (bool Key : {false, true})
                {
                    if (Kind != EDasTypeKind::Map && Key)
                        continue;
                    FDasDeclaredType Type;
                    Type.Kind = DasTypeKindName(Kind);
                    for (const auto& Slot : Registry.FindKind(Kind)->GetContainerSlots())
                        Type.*Slot.Descriptor = MakeShared<FDasDeclaredType>(
                            Kind != EDasTypeKind::Map || Slot.bHashKey == Key ? Sample : FDasDeclaredType());
                    Types.Add(MoveTemp(Type));
                }
            for (const auto& Type : Types)
                for (auto Usage :
                     {EDasPropertyUsage::Value, EDasPropertyUsage::Container, EDasPropertyUsage::DelegateParameter})
                {
                    FString Why;
                    const bool Valid = ValidateDasPropertyType(Type, Usage, Why);
                    auto* P = CreateDasProperty(FFieldVariant(), TEXT("SupportProbe"), Type, Usage, Error);
                    const bool Good = Valid == (P != nullptr) && (Valid || !Why.IsEmpty());
                    delete P;
                    if (!Good)
                    {
                        Error = TEXT("Support report disagrees with property factory: ") + Type.StorageIdentity();
                        return false;
                    }
                    Valid ? ++Positive : ++Negative;
                }
        }
    }
    // Direction, surface and literal boundaries must remain distinct even for the same scalar.
    FDasDeclaredType Int;
    FDasDeclaredType Void;
    Void.Kind = TEXT("void");
    FDasDeclaredType Name;
    Name.Kind = TEXT("name");
    FDasDeclaredType Text;
    Text.Kind = TEXT("text");
    auto Expect = [&](bool Good, const TCHAR* Why)
    {
        if (!Good)
            Error = Why;
        return Good;
    };
    if (!Expect(!QueryDasTypeUse(Void, EDasTypeUse::AuthoredInput).IsSupported() &&
                    QueryDasTypeUse(Void, EDasTypeUse::AuthoredReturn).IsSupported() &&
                    QueryDasTypeUse(Name, EDasTypeUse::PropertyDefault).IsSupported() &&
                    !QueryDasTypeUse(Name, EDasTypeUse::ParameterDefault).IsSupported() &&
                    QueryDasTypeUse(Text, EDasTypeUse::ParameterDefault).IsSupported() &&
                    !QueryDasTypeUse(Text, EDasTypeUse::ComputedDefaultLeaf).IsSupported(),
                TEXT("Type contexts collapsed distinct signature/default boundaries")))
        return false;
    FDasDeclaredMethod M;
    M.ReturnType = Void;
    FDasDeclaredParameter Param;
    Param.Name = TEXT("Value");
    Param.Type = Int;
    Param.Direction = TEXT("out");
    M.Parameters.Add(Param);
    FString Why;
    if (!Expect(ValidateDasSignatureContract(M, EDasSignatureUse::Method, Why) &&
                    ValidateDasSignatureContract(M, EDasSignatureUse::Interface, Why) &&
                    ValidateDasSignatureContract(M, EDasSignatureUse::Delegate, Why) &&
                    !ValidateDasSignatureContract(M, EDasSignatureUse::Multicast, Why),
                TEXT("Signature contract lost out-only multicast restriction")))
        return false;
    M.Parameters[0].Direction = TEXT("inout");
    TStrongObjectPtr<UFunction> Sig(MakeDasDelegateSignature(M, true, GetTransientPackage(), NAME_None, Why));
    if (!Expect(Sig.IsValid(), TEXT("Multicast mutable-ref signature rejected")))
        return false;
    M.ReturnType = Int;
    if (!Expect(!ValidateDasSignatureContract(M, EDasSignatureUse::Multicast, Why),
                TEXT("Multicast return value accepted")))
        return false;
    M.ReturnType = Void;
    M.Parameters[0].Direction = TEXT("in");
    M.Parameters[0].Default.Kind = TEXT("number");
    M.Parameters[0].Default.Number = 1;
    if (!Expect(ValidateDasParameterDefaults(M, true, Why) &&
                    !ValidateDasSignatureContract(M, EDasSignatureUse::Interface, Why) &&
                    ValidateDasSignatureContract(M, EDasSignatureUse::Delegate, Why),
                TEXT("Interface defaults or delegate signature projection changed")))
        return false;
    M.Parameters.Add(Param);
    M.Parameters.Last().Direction = TEXT("in");
    if (!Expect(!ValidateDasParameterDefaults(M, true, Why), TEXT("Non-trailing parameter default accepted")))
        return false;
    M.Parameters.Pop();
    M.WorldContext = M.Parameters[0].Name;
    if (!Expect(!ValidateDasParameterDefaults(M, true, Why), TEXT("WorldContext default accepted")))
        return false;
    M.WorldContext.Reset();
    M.Parameters[0].Default.Kind.Reset();
    M.Parameters[0].Direction = TEXT("invalid");
    if (!Expect(!ValidateDasSignatureContract(M, EDasSignatureUse::Method, Why), TEXT("Unknown direction accepted")))
        return false;
    FDasDeclaredLiteral V;
    V.Kind = TEXT("number");
    V.Number = 2147483648.0;
    if (!Expect(!ValidateDasAuthoredDefault(Int, V, EDasTypeUse::ParameterDefault, Why),
                TEXT("Out-of-range parameter default accepted")))
        return false;
    V.Number = 42;
    if (!Expect(ValidateDasAuthoredDefault(Int, V, EDasTypeUse::ParameterDefault, Why),
                TEXT("Valid parameter default rejected")))
        return false;
    FDasDeclaredType String;
    String.Kind = TEXT("string");
    V.Kind = TEXT("string");
    V.String = FString::ChrN(4097, 'a');
    if (!Expect(!ValidateDasAuthoredDefault(String, V, EDasTypeUse::ParameterDefault, Why) &&
                    ValidateDasAuthoredDefault(String, V, EDasTypeUse::PropertyDefault, Why),
                TEXT("Property and parameter string budgets merged")))
        return false;
    FDasDeclaredType Array;
    Array.Kind = TEXT("array");
    Array.Element = MakeShared<FDasDeclaredType>(Int);
    V.Kind = TEXT("number");
    V.Number = 1;
    if (!Expect(!ValidateDasAuthoredDefault(Array, V, EDasTypeUse::StructMemberDefault, Why),
                TEXT("Struct container accepted an explicit initializer")))
        return false;
    V.Kind.Reset();
    if (!Expect(ValidateDasAuthoredDefault(Array, V, EDasTypeUse::StructMemberDefault, Why) &&
                    QueryDasTypeUse(Array, EDasTypeUse::QueuedEvent).IsSupported() &&
                    QueryDasTypeUse(Array, EDasTypeUse::Timer).IsSupported(),
                TEXT("Empty struct container or owned callback carrier rejected")))
        return false;
    for (const auto& Entry : Registry.Entries())
    {
        TArray<FDasDeclaredType> Samples;
        Entry.Adapter->GetSupportSamples(Samples);
        for (const auto& Type : Samples)
            for (const auto& Use : DasTypeUses())
            {
                auto Result = QueryDasTypeUse(Type, Use.Use);
                if (!Expect(Result.IsSupported() == Result.Reason.IsEmpty(),
                            TEXT("Context report lost rejection/context reason")))
                    return false;
                if (const auto* N = Entry.Adapter->GetNativePolicy(); N && N->StorageError)
                    if (!Expect(!QueryDasTypeUse(Type, EDasTypeUse::PropertyDefault).IsSupported() &&
                                    !QueryDasTypeUse(Type, EDasTypeUse::QueuedEvent).IsSupported() &&
                                    !QueryDasTypeUse(Type, EDasTypeUse::Timer).IsSupported(),
                                TEXT("Opaque transport opened generic defaults/queued callbacks")))
                        return false;
            }
    }
    Error.Reset();
    return Positive > 0 && Negative > 0;
}


bool CheckDasOpaqueTypeAdapters(FString& Error)
{
    using Role = EDasNativeTypeRole;
    int32 InputEvents = 0, WidgetEvents = 0, Rejected = 0;
    for (const auto& Entry : FDasTypeAdapters::Get().Entries())
    {
        const auto* P = Entry.Adapter->GetNativePolicy();
        if (!P)
            continue;
        if (!P->Lifetime || !P->AllowCall || !P->CallError || Entry.Adapter->IsLeafValue())
        {
            Error = TEXT("Opaque policy lost a gate or became a generic authored leaf");
            return false;
        }
        if (EnumHasAnyFlags(P->Roles, Role::InputEvent))
        {
            ++InputEvents;
            WidgetEvents += EnumHasAnyFlags(P->Roles, Role::WidgetEvent);
            if (!P->Capture || !P->ValidateSnapshot || !P->StorageError)
            {
                Error = TEXT("Detached input lost snapshot validation/storage restrictions");
                return false;
            }
        }
        if (EnumHasAnyFlags(P->Roles, Role::Paint | Role::EventReply | Role::WidgetAnimation | Role::Constraint |
                                          Role::HitResult) &&
            P->Capture)
        {
            Error = TEXT("Scoped/weak/dedicated capability opened generic snapshot capture");
            return false;
        }
        if (EnumHasAnyFlags(P->Roles, Role::Rejected))
        {
            ++Rejected;
            if (P->Transport || P->Handle || P->AllowCall(nullptr, true, true, 0))
            {
                Error = TEXT("Internal handle acquired a native binding contract");
                return false;
            }
        }
    }
    if (InputEvents != 7 || WidgetEvents != 5 || Rejected != 2 || !DasNativeTypePath(Role::InputEvent).IsEmpty())
    {
        Error = TEXT("Opaque families lost exact widget membership or ambiguous-role protection");
        return false;
    }
    for (Role R : {Role::Paint, Role::EventReply, Role::Geometry, Role::InputActionValue})
        if (!DasSynchronousCallbackRole(DasNativeTypePath(R), R))
        {
            Error = TEXT("Specialized synchronous callback lost exact role admission");
            return false;
        }
    for (Role R : {Role::WidgetAnimation, Role::Constraint, Role::HitResult})
        if (DasSynchronousCallbackRole(DasNativeTypePath(R), R))
        {
            Error = TEXT("Non-callback capability opened synchronous callback admission");
            return false;
        }
    bool NativeAgreement = true;
    auto Gate = [&](Role R, UFunction* F, bool In, bool Out, bool Return, int32 Depth, int32 ArrayDim)
    {
        const FString Path = DasNativeTypePath(R);
        const auto* A = FDasTypeAdapters::Get().FindStructPath(Path);
        const auto* P = A ? A->GetNativePolicy() : nullptr;
        if (!F || !A || !P || !P->AllowCall)
            return false;
        auto Field = MakeUnique<FStructProperty>(FFieldVariant(F), TEXT("PolicyProbe"));
        Field->Struct = const_cast<UScriptStruct*>(A->GetStruct());
        Field->ArrayDim = ArrayDim;
        Field->SetPropertyFlags(CPF_Parm | (Return ? CPF_ReturnParm : CPF_None));
        FString Why;
        const bool Expected = P->AllowCall(Field.Get(), In, Out, Depth);
        const bool Actual = ValidateDasNativeCallContract(Field.Get(), In, Out, Depth, Why);
        NativeAgreement &= Expected == Actual && (Actual || !Why.IsEmpty());
        return Actual;
    };
    auto* Geometry = UWidget::StaticClass()->FindFunctionByName(TEXT("GetCachedGeometry"));
    auto* Paint = UWidgetBlueprintLibrary::StaticClass()->FindFunctionByName(TEXT("DrawBox"));
    auto* Reply = UWidgetBlueprintLibrary::StaticClass()->FindFunctionByName(TEXT("Handled"));
    auto* Animation = UUserWidget::StaticClass()->FindFunctionByName(TEXT("PlayAnimation"));
    auto* Constraint = UPhysicsConstraintComponent::StaticClass()->FindFunctionByName(TEXT("GetConstraint"));
    UFunction* Consumer = nullptr;
    for (TFieldIterator<UFunction> It(UConstraintInstanceBlueprintLibrary::StaticClass()); It; ++It)
        if (It->GetOuterUClass() == UConstraintInstanceBlueprintLibrary::StaticClass())
        {
            Consumer = *It;
            break;
        }
    const bool Valid =
        Gate(Role::Geometry, Geometry, false, true, true, 0, 1) &&
        !Gate(Role::Geometry, Geometry, false, true, true, 1, 1) &&
        !Gate(Role::Geometry, Geometry, false, true, true, 0, 2) &&
        !Gate(Role::Geometry, Reply, false, true, true, 0, 1) && Gate(Role::Paint, Paint, true, false, false, 0, 1) &&
        !Gate(Role::Paint, Paint, false, true, true, 0, 1) && Gate(Role::EventReply, Reply, false, true, true, 0, 1) &&
        !Gate(Role::EventReply, Reply, false, true, true, 1, 1) &&
        Gate(Role::WidgetAnimation, Animation, false, true, true, 0, 1) &&
        !Gate(Role::WidgetAnimation, Animation, true, true, true, 0, 1) &&
        !Gate(Role::WidgetAnimation, Animation, false, true, false, 0, 1) &&
        Gate(Role::Constraint, Constraint, false, true, true, 0, 1) &&
        Gate(Role::Constraint, Consumer, true, false, false, 0, 1) &&
        !Gate(Role::Constraint, Consumer, false, true, true, 0, 1) &&
        Gate(Role::HitResult, Geometry, true, false, false, 2, 1) &&
        !Gate(Role::HitResult, Geometry, true, false, false, 0, 2);
    if (!Valid || !NativeAgreement)
    {
        Error = TEXT("Native policy lost exact function/direction/depth/fixed-array restrictions");
        return false;
    }
    Error.Reset();
    return true;
}
