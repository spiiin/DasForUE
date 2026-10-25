#include "DasPropertyTypes.h"
#include "DasTypeAdapters.h"
#include "DasSoftReferences.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"

namespace
{
bool Validate(const FDasDeclaredType& T, EDasPropertyUsage Usage, FString& Error, int32 Depth, int32& Nodes)
{
    auto Fail = [&](const TCHAR* Why)
    {
        Error = FString(Why) + TEXT(": ")+ T.Kind + TEXT(" ")+ T.TypePath;
        return false;
    };
    if (Depth > 16 || ++Nodes > 1024)
        return Fail(TEXT("Property type exceeds depth/node limit"));
    const auto K = T.GetKind();
    if (T.WeakReference && K != EDasTypeKind::Object)
        return Fail(TEXT("Weak storage requires a concrete object class"));
    if (K == EDasTypeKind::Invalid || K == EDasTypeKind::Void || K == EDasTypeKind::Delegate ||
        K == EDasTypeKind::Event)
        return Fail(TEXT("Unsupported property kind"));
    if (K != EDasTypeKind::Enum && T.EnumStorage != EDasEnumStorage::Automatic)
        return Fail(TEXT("Enum storage on a non-enum type"));
    if (!T.AuthoredId.IsEmpty() && K != EDasTypeKind::Object)
        return Fail(TEXT("Authored object identity on a non-object type"));
    if (!T.DelegateWire.IsEmpty())
        return Fail(TEXT("Unexpected delegate signature"));
    const auto* Adapter = FDasTypeAdapters::Get().Find(T);
    // The kind adapter remains available for malformed identities so shape errors stay explicit.
    const auto* ShapeAdapter = Adapter ? Adapter : FDasTypeAdapters::Get().FindKind(K);
    const auto Slots = ShapeAdapter ? ShapeAdapter->GetContainerSlots() : TConstArrayView<FDasContainerSlot>();
    if (!Slots.IsEmpty())
    {
        if (!T.TypePath.IsEmpty())
            return Fail(TEXT("Container type_path must be empty"));
        const int32 Present = int32(bool(T.Element)) + int32(bool(T.Key)) + int32(bool(T.Value));
        if (Present != Slots.Num())
            return Fail(TEXT("Unsupported container shape"));
        for (const auto& Slot : Slots)
        {
            const auto& Child = T.*Slot.Descriptor;
            if (!Child)
                return Fail(TEXT("Missing container child"));
            if (Child->WeakReference)
                return Fail(TEXT("Direct weak-reference containers need a storage-preserving carrier"));
            const auto* ChildAdapter = FDasTypeAdapters::Get().Find(*Child);
            if (ChildAdapter && !ChildAdapter->SupportsContainerValue())
                return Fail(TEXT("Unsupported container child type"));
            if (!Validate(*Child, Usage, Error, Depth + 1, Nodes))
                return false;
            if (Slot.bHashKey && (!ChildAdapter || !ChildAdapter->CanBeContainerKey(*Child)))
                return Fail(TEXT("Unsupported container hash key"));
        }
        return true;
    }
    if (T.Element || T.Key || T.Value)
        return Fail(TEXT("Leaf has container members"));
    if (K == EDasTypeKind::Struct)
    {
        auto* S = FindObject<UScriptStruct>(nullptr, *T.TypePath);
        if (!S)
            return Fail(TEXT("Missing loaded struct"));
        return Usage != EDasPropertyUsage::Container || ValidateDasValueStruct(T.TypePath, Error);
    }
    if (Adapter)
        if (const auto Result = Adapter->ValidateDeclared(T, Usage, Error); Result.IsSet())
        {
            if (!Result.GetValue())
                Error += TEXT(": ") + T.Kind + TEXT(" ") + T.TypePath;
            return Result.GetValue();
        }
    return T.TypePath.IsEmpty() ? true : Fail(TEXT("Primitive has a type identity"));
}
bool Describe(const FProperty* P, FDasDeclaredType& Out, FString& Error, int32 Depth, int32& Nodes)
{
    if (!P || P->ArrayDim != 1 || Depth > 16 || ++Nodes > 1024)
    {
        Error = TEXT("Unsupported fixed array or property nesting");
        return false;
    }
    if (DasHasDirectWeakContainer(P))
    {
        Error = TEXT("Direct weak-reference containers need a storage-preserving carrier");
        return false;
    }
    Out = {};
    const auto K = DasPropertyKind(P);
    Out.Kind = DasTypeKindName(K);
    if (K == EDasTypeKind::Invalid)
    {
        Error = TEXT("Unsupported reflected property: ")+ P->GetClass()->GetName();
        return false;
    }
    if (const auto* Adapter = FDasTypeAdapters::Get().FindProperty(P))
    {
        const auto Slots = Adapter->GetContainerSlots();
        if (!Slots.IsEmpty())
        {
            for (const auto& Slot : Slots)
            {
                auto Child = MakeShared<FDasDeclaredType>();
                Out.*Slot.Descriptor = Child;
                if (!Describe(Slot.ReadProperty(P), *Child, Error, Depth + 1, Nodes))
                    return false;
            }
            return true;
        }
    }
    if (const auto* Adapter = FDasTypeAdapters::Get().FindProperty(P))
        if (const auto Result = Adapter->DescribeProperty(P, Out, Error); Result.IsSet())
            return Result.GetValue();
    if (auto* Soft = DasSoftCarrierForProperty(P))
        Out.TypePath = Soft->GetPathName();
    else if (auto* S = CastField<FStructProperty>(P))
        Out.TypePath = S->Struct->GetPathName();
    if (K == EDasTypeKind::Delegate || K == EDasTypeKind::Event)
    {
        Error = TEXT("Delegate signature requires its explicit authoring contract");
        return false;
    }
    return true;
}
FProperty* Build(FFieldVariant Owner, FName Name, const FDasDeclaredType& T, EDasPropertyUsage Usage)
{
    // Scalar kinds (and any adapter with its own property) build through the type adapter.
    if (const auto* Adapter = FDasTypeAdapters::Get().Find(T))
        if (FProperty* P = Adapter->MakeProperty(Owner, Name, T, Usage))
            return P;
    if (T.GetKind() == EDasTypeKind::Struct)
    {
        auto* P = new FStructProperty(Owner, Name);
        P->Struct = FindObject<UScriptStruct>(nullptr, *T.TypePath);
        return P;
    }
    return nullptr;
}
}
FString FDasDeclaredType::StorageIdentity() const
{
    TFunction<FString(const FDasDeclaredType&, int32)> StorageKey;
    StorageKey = [&](const FDasDeclaredType& T, int32 Depth)
    {
        if (Depth > 16)
            return FString(TEXT("invalid-depth"));
        FString S = T.Kind + FString::Printf(TEXT("[%d]:%d:"), int32(T.EnumStorage), T.TypePath.Len()) + T.TypePath;
        if (T.WeakReference)
            S += TEXT("#weak");
        if (T.Element)
            S += TEXT("<")+ StorageKey(*T.Element, Depth + 1) + TEXT(">");
        if (T.Key)
            S += TEXT("{")+ StorageKey(*T.Key, Depth + 1) +
                      TEXT(";")+ (T.Value ? StorageKey(*T.Value, Depth + 1) : TEXT("missing")) + TEXT("}");
        return S;
    };
    return StorageKey(*this, 0);
}
EDasTypeKind DasPropertyKind(const FProperty* P)
{
    if (!P || P->ArrayDim != 1)
        return EDasTypeKind::Invalid;
    if (const auto* Adapter = FDasTypeAdapters::Get().FindProperty(P); Adapter && !Adapter->GetStruct())
        return Adapter->GetKind();
    if (CastField<FSoftObjectProperty>(P))
        return EDasTypeKind::Struct;
    if (CastField<FStructProperty>(P))
        return EDasTypeKind::Struct;
    if (CastField<FDelegateProperty>(P))
        return EDasTypeKind::Delegate;
    if (CastField<FMulticastDelegateProperty>(P))
        return EDasTypeKind::Event;
    return EDasTypeKind::Invalid;
}
bool DescribeDasProperty(const FProperty* P, FDasDeclaredType& Out, FString& Error)
{
    Error.Reset();
    FDasDeclaredType Result;
    int32 Nodes = 0;
    if (!Describe(P, Result, Error, 0, Nodes))
    {
        Out = {};
        Out.Kind.Reset();
        return false;
    }
    Out = MoveTemp(Result);
    return true;
}
bool ValidateDasPropertyType(const FDasDeclaredType& Type, EDasPropertyUsage Usage, FString& Error)
{
    Error.Reset();
    int32 Nodes = 0;
    return Validate(Type, Usage, Error, 0, Nodes);
}
FProperty* CreateDasProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType& Type, EDasPropertyUsage Usage,
                             FString& Error)
{
    if (!IsInGameThread())
    {
        Error = TEXT("Property creation requires the game thread");
        return nullptr;
    }
    if (!ValidateDasPropertyType(Type, Usage, Error))
        return nullptr;
    return Build(Owner, Name, Type, Usage);
}
