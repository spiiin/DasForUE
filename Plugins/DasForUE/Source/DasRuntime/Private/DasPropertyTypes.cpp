#include "DasPropertyTypes.h"
#include "DasTypeAdapters.h"
#include "DasSoftReferences.h"
#include "UObject/UnrealType.h"
#include "UObject/StrProperty.h"
#include "UObject/TextProperty.h"

namespace
{
bool HashKey(EDasTypeKind K)
{
    return K == EDasTypeKind::Int || K == EDasTypeKind::Byte || K == EDasTypeKind::String || K == EDasTypeKind::Name ||
           K == EDasTypeKind::Enum || K == EDasTypeKind::Object;
}
bool Container(EDasTypeKind K)
{
    return K == EDasTypeKind::Array || K == EDasTypeKind::Set || K == EDasTypeKind::Map;
}
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
    if (Container(K))
    {
        if ((T.Element && T.Element->WeakReference) || (T.Key && T.Key->WeakReference) ||
            (T.Value && T.Value->WeakReference))
            return Fail(TEXT("Direct weak-reference containers need a storage-preserving carrier"));
        if (!T.TypePath.IsEmpty())
            return Fail(TEXT("Container type_path must be empty"));
        if (K == EDasTypeKind::Map)
        {
            if (!T.Key || !T.Value || T.Element || !HashKey(T.Key->GetKind()) || Container(T.Value->GetKind()) ||
                T.Value->GetKind() == EDasTypeKind::Class)
                return Fail(TEXT("Unsupported map shape"));
            if (!Validate(*T.Key, Usage, Error, Depth + 1, Nodes) ||
                !Validate(*T.Value, Usage, Error, Depth + 1, Nodes))
                return false;
            if (T.Key->GetKind() == EDasTypeKind::Object)
            {
                auto* C = FindObject<UClass>(nullptr, *T.Key->TypePath);
                if (C->HasAnyClassFlags(CLASS_Interface))
                    return Fail(TEXT("Interface object keys are unsupported"));
            }
        }
        else
        {
            if (!T.Element || T.Key || T.Value || Container(T.Element->GetKind()) ||
                T.Element->GetKind() == EDasTypeKind::Class ||
                (K == EDasTypeKind::Set && !HashKey(T.Element->GetKind())))
                return Fail(TEXT("Unsupported element shape"));
            if (!Validate(*T.Element, Usage, Error, Depth + 1, Nodes))
                return false;
            if (K == EDasTypeKind::Set && T.Element->GetKind() == EDasTypeKind::Object)
            {
                auto* C = FindObject<UClass>(nullptr, *T.Element->TypePath);
                if (C->HasAnyClassFlags(CLASS_Interface))
                    return Fail(TEXT("Interface object sets are unsupported"));
            }
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
    if (const auto* Adapter = FDasTypeAdapters::Get().Find(T))
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
    if (auto* A = CastField<FArrayProperty>(P))
    {
        auto Child = MakeShared<FDasDeclaredType>();
        Out.Element = Child;
        return Describe(A->Inner, *Child, Error, Depth + 1, Nodes);
    }
    if (auto* S = CastField<FSetProperty>(P))
    {
        auto Child = MakeShared<FDasDeclaredType>();
        Out.Element = Child;
        return Describe(S->ElementProp, *Child, Error, Depth + 1, Nodes);
    }
    if (auto* M = CastField<FMapProperty>(P))
    {
        auto Key = MakeShared<FDasDeclaredType>(), Value = MakeShared<FDasDeclaredType>();
        Out.Key = Key;
        Out.Value = Value;
        return Describe(M->KeyProp, *Key, Error, Depth + 1, Nodes) &&
               Describe(M->ValueProp, *Value, Error, Depth + 1, Nodes);
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
    switch (T.GetKind())
    {
    case EDasTypeKind::Struct:
    {
        auto* P = new FStructProperty(Owner, Name);
        P->Struct = FindObject<UScriptStruct>(nullptr, *T.TypePath);
        return P;
    }
    case EDasTypeKind::Array:
    {
        auto* P = new FArrayProperty(Owner, Name);
        P->Inner =
            Build(P, Usage == EDasPropertyUsage::DelegateParameter ? TEXT("Inner"): TEXT("Values"), *T.Element, Usage);
        return P;
    }
    case EDasTypeKind::Set:
    {
        auto* P = new FSetProperty(Owner, Name);
        P->ElementProp = Build(P, TEXT("Element"), *T.Element, Usage);
        P->ElementProp->SetPropertyFlags(CPF_HasGetValueTypeHash);
        return P;
    }
    case EDasTypeKind::Map:
    {
        auto* P = new FMapProperty(Owner, Name);
        P->KeyProp = Build(P, TEXT("Key"), *T.Key, Usage);
        P->KeyProp->SetPropertyFlags(CPF_HasGetValueTypeHash);
        P->ValueProp = Build(P, TEXT("Value"), *T.Value, Usage);
        return P;
    }
    default:
        return nullptr;
    }
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
    if (CastField<FArrayProperty>(P))
        return EDasTypeKind::Array;
    if (CastField<FSetProperty>(P))
        return EDasTypeKind::Set;
    if (CastField<FMapProperty>(P))
        return EDasTypeKind::Map;
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
