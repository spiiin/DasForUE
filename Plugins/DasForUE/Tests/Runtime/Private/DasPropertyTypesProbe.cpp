#include "DasPropertyTypes.h"
#include "DasTypeAdapters.h"
#include "DasSnapshotContainers.h"
#include "UObject/StructOnScope.h"
#include "DasContainerRegistry.h"
#include "DasContainerTypes.h"
#include "DasDefaultValueProbe.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "UObject/Interface.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/Package.h"
#include "UObject/TextProperty.h"

bool CheckDasPropertyTypes(FString& Error)
{
    if (!CheckDasContainerRegistry(Error))
        return false;
    auto Leaf =
        [](const TCHAR* Kind, const FString& Path = FString(), EDasEnumStorage Storage = EDasEnumStorage::Automatic)
    {
        FDasDeclaredType T;
        T.Kind = Kind;
        T.TypePath = Path;
        T.EnumStorage = Storage;
        return T;
    };
    auto Shape =
        [](const TCHAR* Kind, const FDasDeclaredType& First, const FDasDeclaredType& Second = FDasDeclaredType())
    {
        FDasDeclaredType T;
        T.Kind = Kind;
        if (T.GetKind() == EDasTypeKind::Map)
        {
            T.Key = MakeShared<FDasDeclaredType>(First);
            T.Value = MakeShared<FDasDeclaredType>(Second);
        }
        else
            T.Element = MakeShared<FDasDeclaredType>(First);
        return T;
    };
    const FString EnumPath = StaticEnum<ESpawnActorCollisionHandlingMethod>()->GetPathName();
    auto ByteEnum = Leaf(TEXT("enum"), EnumPath, EDasEnumStorage::Byte),
                         StrongEnum = Leaf(TEXT("enum"), EnumPath, EDasEnumStorage::Enum8);
    const auto Object = Leaf(TEXT("object"), UObject::StaticClass()->GetPathName()),
                             Interface = Leaf(TEXT("object"), UInterface::StaticClass()->GetPathName());
    TArray<FDasDeclaredType> Values;
    for (const TCHAR* K :
         {TEXT("byte"), TEXT("text"), TEXT("int"), TEXT("float"), TEXT("double"), TEXT("bool"), TEXT("string"),
                                                                                                     TEXT("name")})
        Values.Add(Leaf(K));
    Values.Append({ByteEnum, StrongEnum, Object, Interface,
                   Leaf(TEXT("struct"), TBaseStructure<FVector>::Get()->GetPathName()),
                        Leaf(TEXT("struct"), FDasDefaultNestedProbe::StaticStruct()->GetPathName())});
    auto Roundtrip = [&](const FDasDeclaredType& Type)
    {
        TStrongObjectPtr<UScriptStruct> A(NewObject<UScriptStruct>(GetTransientPackage())),
            B(NewObject<UScriptStruct>(GetTransientPackage()));
        auto* P = CreateDasProperty(A.Get(), TEXT("Value"), Type, EDasPropertyUsage::Value, Error);
        if (!P)
            return false;
        A->AddCppProperty(P);
        A->Bind();
        A->StaticLink(true);
        {
            FStructOnScope Storage(A.Get());
            if (!DasSnapshotContainers::CheckProperty(P, P->ContainerPtrToValuePtr<void>(Storage.GetStructMemory()),
                                                      Error))
                return false;
        }
        FDasDeclaredType Description;
        if (!DescribeDasProperty(P, Description, Error))
            return false;
        auto* Copy = CreateDasProperty(B.Get(), TEXT("Value"), Description, EDasPropertyUsage::Value, Error);
        if (!Copy)
            return false;
        B->AddCppProperty(Copy);
        B->Bind();
        B->StaticLink(true);
        FDasDeclaredType Again;
        if (!DescribeDasProperty(Copy, Again, Error) || Description.StorageIdentity() != Again.StorageIdentity() ||
            !P->SameType(Copy) || P->GetElementSize() != Copy->GetElementSize() ||
            A->GetMinAlignment() != B->GetMinAlignment())
        {
            Error = TEXT("Property description/build roundtrip mismatch: ")+ Type.Kind;
            return false;
        }
        if (auto* Set = CastField<FSetProperty>(P);
            Set && !Set->ElementProp->HasAnyPropertyFlags(CPF_HasGetValueTypeHash))
        {
            Error = TEXT("Missing set hash flag");
            return false;
        }
        if (auto* Map = CastField<FMapProperty>(P); Map && !Map->KeyProp->HasAnyPropertyFlags(CPF_HasGetValueTypeHash))
        {
            Error = TEXT("Missing map hash flag");
            return false;
        }
        if (Type.GetKind() == EDasTypeKind::Array || Type.GetKind() == EDasTypeKind::Map ||
            Type.GetKind() == EDasTypeKind::Set)
        {
            auto* Carrier = DasContainerCarrierForProperty(P);
            auto* CarrierP = Carrier ? Carrier->FindPropertyByName(TEXT("Values")) : nullptr;
            if (!CarrierP || !P->SameType(CarrierP) || Carrier != DasContainerCarrier(Description) ||
                Carrier != DasContainerCarrierForProperty(P) ||
                DasContainerProperty(Carrier->GetPathName()) != CarrierP ||
                P->GetElementSize() != CarrierP->GetElementSize() ||
                P->GetMinAlignment() != CarrierP->GetMinAlignment())
            {
                Error =
                    TEXT("Factory/native carrier identity or path roundtrip mismatch: ")+ Description.StorageIdentity();
                return false;
            }
        }
        // Assert the existing UHT naming contract independently of the catalog.
        // This catches accidentally replacing a static carrier by an equivalent
        // dynamic property tree, which SameType alone would not detect.
        auto Primitive = [](const TSharedPtr<const FDasDeclaredType>& T)
        {
            return T && T->TypePath.IsEmpty() &&
                   (T->GetKind() == EDasTypeKind::Byte || T->GetKind() == EDasTypeKind::Text ||
                    T->GetKind() == EDasTypeKind::Int || T->GetKind() == EDasTypeKind::Float ||
                    T->GetKind() == EDasTypeKind::Double || T->GetKind() == EDasTypeKind::Bool ||
                    T->GetKind() == EDasTypeKind::String || T->GetKind() == EDasTypeKind::Name);
        };
        auto Title = [](FString K)
        {
            K[0] = FChar::ToUpper(K[0]);
            return K;
        };
        FString Expected;
        if (Type.GetKind() == EDasTypeKind::Array && Primitive(Type.Element))
            Expected = TEXT("DasArray")+ Title(Type.Element->Kind);
        if (Type.GetKind() == EDasTypeKind::Set && Primitive(Type.Element))
            Expected = TEXT("DasSet")+ Title(Type.Element->Kind);
        if (Type.GetKind() == EDasTypeKind::Map && Type.Key &&
            (Type.Key->GetKind() == EDasTypeKind::Int || Type.Key->GetKind() == EDasTypeKind::String) &&
            Primitive(Type.Value))
            Expected = TEXT("DasMap")+ Title(Type.Key->Kind) + Title(Type.Value->Kind);
        if (!Expected.IsEmpty())
        {
            auto* Carrier = DasContainerCarrierForProperty(P);
            if (!Carrier || !Carrier->GetCppStructOps() ||
                Carrier->GetPathName() != TEXT("/Script/DasRuntime.")+ Expected + TEXT("Carrier"))
            {
                Error = TEXT("Static UHT carrier identity changed: ")+ Expected;
                return false;
            }
        }
        return true;
    };
    for (const auto& Type : Values)
        if (!Roundtrip(Type))
            return false;
    if (!Roundtrip(Leaf(TEXT("class"), AActor::StaticClass()->GetPathName())))
        return false;
    for (const auto& Type : Values)
        if (!Roundtrip(Shape(TEXT("array"), Type)))
            return false;
    TArray<FDasDeclaredType> Keys = {
        Leaf(TEXT("int")), Leaf(TEXT("byte")), Leaf(TEXT("string")), Leaf(TEXT("name")), ByteEnum, StrongEnum, Object};
    for (const auto& Key : Keys)
    {
        if (!Roundtrip(Shape(TEXT("set"), Key)))
            return false;
        for (const auto& Value : Values)
            if (!Roundtrip(Shape(TEXT("map"), Key, Value)))
                return false;
    }
    if (ByteEnum.StorageIdentity() == StrongEnum.StorageIdentity())
    {
        Error = TEXT("Enum storage identities collapsed");
        return false;
    }
    TStrongObjectPtr<UScriptStruct> Owner(NewObject<UScriptStruct>(GetTransientPackage()));
    auto Reject = [&](const FDasDeclaredType& T, EDasPropertyUsage Usage)
    {
        FString Why;
        auto* Unexpected = CreateDasProperty(Owner.Get(), TEXT("Rejected"), T, Usage, Why);
        if (Unexpected)
        {
            delete Unexpected;
            Error = TEXT("Unsupported factory type accepted: ")+ T.Kind;
            return false;
        }
        if (Why.IsEmpty() || Owner->ChildProperties)
        {
            Error = TEXT("Factory failure changed owner or omitted diagnostic");
            return false;
        }
        return true;
    };
    auto Invalid = Leaf(TEXT("unknown"));
    if (!Reject(Invalid, EDasPropertyUsage::Value))
        return false;
    Invalid = Leaf(TEXT("int"), TEXT("/Script/Engine.Actor"));
    if (!Reject(Invalid, EDasPropertyUsage::Value))
        return false;
    Invalid = Shape(TEXT("array"), Shape(TEXT("array"), Leaf(TEXT("int"))));
    if (!Reject(Invalid, EDasPropertyUsage::Container))
        return false;
    if (!Reject(Shape(TEXT("set"), Interface), EDasPropertyUsage::Container) ||
                !Reject(Shape(TEXT("map"), Interface, Object), EDasPropertyUsage::Container))
        return false;
    {
        auto* Class = CreateDasProperty(Owner.Get(), TEXT("ClassInput"),
                                                          Leaf(TEXT("class"), AActor::StaticClass()->GetPathName()),
                                                               EDasPropertyUsage::DelegateParameter, Error);
        if (!Class)
            return false;
        delete Class;
    }
    if (!Reject(Leaf(TEXT("object"), TEXT("/Script/Engine.MissingFactoryClass")), EDasPropertyUsage::Value))
        return false;
    for (const TCHAR* K :
         {TEXT("byte"), TEXT("text"), TEXT("int"), TEXT("float"), TEXT("double"), TEXT("bool"), TEXT("string"),
                                                                                                     TEXT("name")})
    {
        auto T = Leaf(K);
        FString Why;
        auto* P = CreateDasProperty(Owner.Get(), TEXT("Input"), T, EDasPropertyUsage::DelegateParameter, Why);
        if (!P)
        {
            Error = Why;
            return false;
        }
        delete P;
    }
    // Malformed descriptors must not match a primitive catalog entry.
    auto BadLeaf = Leaf(TEXT("int"), TEXT("/Script/Engine.Actor"));
    if (DasContainerCarrier(Shape(TEXT("array"), BadLeaf)))
    {
        Error = TEXT("Static carrier accepted an invalid primitive identity");
        return false;
    }
    auto Extra = Shape(TEXT("array"), Leaf(TEXT("int")));
    Extra.Key = MakeShared<FDasDeclaredType>(Leaf(TEXT("int")));
    if (DasContainerCarrier(Extra))
    {
        Error = TEXT("Static carrier accepted unexpected descriptor children");
        return false;
    }
    // Every combinator rejects missing/extra children without attaching anything to its owner.
    for (const TCHAR* Kind : {TEXT("array"), TEXT("set"), TEXT("map")})
    {
        auto Missing = Leaf(Kind);
        if (!Reject(Missing, EDasPropertyUsage::Container))
            return false;
        const auto* Adapter = FDasTypeAdapters::Get().FindKind(Missing.GetKind());
        auto* Direct =
            Adapter ? Adapter->MakeProperty(Owner.Get(), TEXT("Direct"), Missing, EDasPropertyUsage::Value) : nullptr;
        const bool Safe = Adapter && !Direct && !Owner->ChildProperties;
        delete Direct;
        if (!Safe)
        {
            Error = TEXT("Direct container adapter accepted a malformed descriptor");
            return false;
        }
        auto ExtraChild = Shape(Kind, Leaf(TEXT("int")), Leaf(TEXT("string")));
        if (ExtraChild.GetKind() == EDasTypeKind::Map)
            ExtraChild.Element = MakeShared<FDasDeclaredType>();
        else
            ExtraChild.Value = MakeShared<FDasDeclaredType>();
        if (!Reject(ExtraChild, EDasPropertyUsage::Container))
            return false;
    }
    for (const auto& Bad :
         {Leaf(TEXT("float")), Leaf(TEXT("double")),
                                    Leaf(TEXT("bool")), Leaf(TEXT("text")),
                                                             Leaf(TEXT("class"), AActor::StaticClass()->GetPathName())})
        if (!Reject(Shape(TEXT("set"), Bad), EDasPropertyUsage::Container) ||
                    !Reject(Shape(TEXT("map"), Bad, Leaf(TEXT("int"))), EDasPropertyUsage::Container))
            return false;
    auto Cycle = MakeShared<FDasDeclaredType>();
    Cycle->Kind = TEXT("array");
    Cycle->Element = Cycle;
    const bool CycleRejected = Reject(*Cycle, EDasPropertyUsage::Container);
    Cycle->Element.Reset();
    if (!CycleRejected || FDasTypeAdapters::Get().FindProperty(nullptr))
        return false;
    auto RejectDescription = [&](FProperty* Root)
    {
        FDasDeclaredType Out;
        FString Why;
        const bool Rejected = !DescribeDasProperty(Root, Out, Why) && Out.Kind.IsEmpty() && !Why.IsEmpty();
        delete Root;
        if (!Rejected)
            Error = TEXT("Container reflection traversal exceeded its shared budget or published partial output");
        return Rejected;
    };
    auto* Deep = new FArrayProperty(FFieldVariant(), TEXT("Deep"));
    auto* DeepParent = Deep;
    for (int32 I = 0; I < 18; ++I)
    {
        auto* Child = new FArrayProperty(DeepParent, TEXT("Values"));
        DeepParent->Inner = Child;
        DeepParent = Child;
    }
    DeepParent->Inner = new FIntProperty(DeepParent, TEXT("Values"));
    if (!RejectDescription(Deep))
        return false;
    TFunction<FProperty*(FFieldVariant, int32)> WideTree;
    WideTree = [&](FFieldVariant Parent, int32 Depth) -> FProperty*
    {
        if (!Depth)
            return new FIntProperty(Parent, TEXT("Leaf"));
        auto* Map = new FMapProperty(Parent, TEXT("Node"));
        Map->KeyProp = WideTree(Map, Depth - 1);
        Map->ValueProp = WideTree(Map, Depth - 1);
        return Map;
    };
    if (!RejectDescription(WideTree(FFieldVariant(), 10)))
        return false;
    // Reflection lookup must reject unsupported shapes before selecting a carrier.
    auto RejectLookup = [&](FProperty* P)
    {
        const bool Rejected = DasContainerCarrierForProperty(P) == nullptr;
        delete P;
        if (!Rejected)
            Error = TEXT("Unsupported native container lookup accepted");
        return Rejected;
    };
    if (DasContainerCarrierForProperty(nullptr) || !RejectLookup(new FIntProperty(Owner.Get(), TEXT("Scalar"))))
        return false;
    auto* Fixed = new FArrayProperty(Owner.Get(), TEXT("Fixed"));
    Fixed->ArrayDim = 2;
    Fixed->Inner = new FIntProperty(Fixed, TEXT("Values"));
    if (!RejectLookup(Fixed))
        return false;
    auto* Nested = new FArrayProperty(Owner.Get(), TEXT("Nested"));
    auto* InnerArray = new FArrayProperty(Nested, TEXT("Values"));
    InnerArray->Inner = new FIntProperty(InnerArray, TEXT("Values"));
    Nested->Inner = InnerArray;
    if (!RejectLookup(Nested))
        return false;
    auto* TextKeys = new FMapProperty(Owner.Get(), TEXT("TextKeys"));
    TextKeys->KeyProp = new FTextProperty(TextKeys, TEXT("Key"));
    TextKeys->ValueProp = new FIntProperty(TextKeys, TEXT("Value"));
    if (!RejectLookup(TextKeys))
        return false;
    auto* FloatSet = new FSetProperty(Owner.Get(), TEXT("FloatSet"));
    FloatSet->ElementProp = new FFloatProperty(FloatSet, TEXT("Element"));
    if (!RejectLookup(FloatSet))
        return false;
    auto* WideArray = new FArrayProperty(Owner.Get(), TEXT("WideEnum"));
    auto* Wide = new FEnumProperty(WideArray, TEXT("Values"));
    Wide->SetEnum(StaticEnum<ESpawnActorCollisionHandlingMethod>());
    Wide->AddCppProperty(new FIntProperty(Wide, TEXT("UnderlyingType")));
    WideArray->Inner = Wide;
    if (!RejectLookup(WideArray))
        return false;
    Error.Reset();
    return true;
}
