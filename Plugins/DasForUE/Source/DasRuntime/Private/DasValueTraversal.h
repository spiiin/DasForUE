#pragma once

#include "UObject/UnrealType.h"

// Topology only: policies decide which values are valid and what to do with
// leaves. Memory retains its constness, including through container helpers.
// GC intentionally uses Unreal's collector instead (native struct ARO and
// incremental GC cannot be reproduced by enumerating reflected object leaves).
namespace DasValueTraversal
{
enum class ERole
{
    Field,
    ArrayElement,
    SetElement,
    MapKey,
    MapValue
};

template <class Memory, class Visitor> bool Property(FProperty* P, Memory Data, ERole Role, Visitor& V);

template <class Memory, class Visitor> bool Struct(const UScriptStruct* Type, Memory Data, Visitor& V)
{
    if (!V.Struct(Type, Data))
        return false;
    for (TFieldIterator<FProperty> It(Type); It; ++It)
    {
        for (int32 I = 0; I < It->ArrayDim; ++I)
        {
            Memory Field = It->template ContainerPtrToValuePtr<uint8>(Data, I);
            if (!Property(*It, Field, ERole::Field, V))
                return false;
        }
    }
    return true;
}

template <class Memory, class Visitor> bool Property(FProperty* P, Memory Data, ERole Role, Visitor& V)
{
    if (!V.Leaf(P, Data, Role))
        return false;
    if (auto* S = CastField<FStructProperty>(P))
        return Struct(S->Struct, Data, V);
    if (auto* A = CastField<FArrayProperty>(P))
    {
        FScriptArrayHelper Items(A, Data);
        if (!V.Begin(P, Data, Items.Num(), int64(Items.Num()) * A->Inner->GetElementSize()))
            return false;
        for (int32 I = 0; I < Items.Num(); ++I)
        {
            Memory Item = Items.GetRawPtr(I);
            if (!Property(A->Inner, Item, ERole::ArrayElement, V))
                return false;
        }
    }
    else if (auto* S = CastField<FSetProperty>(P))
    {
        FScriptSetHelper Items(S, Data);
        if (!V.Begin(P, Data, Items.Num(), int64(Items.Num()) * S->ElementProp->GetElementSize()))
            return false;
        for (int32 I = 0; I < Items.GetMaxIndex(); ++I)
            if (Items.IsValidIndex(I))
            {
                Memory Item = Items.GetElementPtr(I);
                if (!Property(S->ElementProp, Item, ERole::SetElement, V))
                    return false;
            }
    }
    else if (auto* M = CastField<FMapProperty>(P))
    {
        FScriptMapHelper Items(M, Data);
        if (!V.Begin(P, Data, Items.Num(),
                     int64(Items.Num()) * (M->KeyProp->GetElementSize() + M->ValueProp->GetElementSize())))
            return false;
        for (int32 I = 0; I < Items.GetMaxIndex(); ++I)
            if (Items.IsValidIndex(I))
            {
                Memory Key = Items.GetKeyPtr(I), Value = Items.GetValuePtr(I);
                if (!Property(M->KeyProp, Key, ERole::MapKey, V) || !Property(M->ValueProp, Value, ERole::MapValue, V))
                    return false;
            }
    }
    else
        return true;
    V.End(P, Data);
    return true;
}
}
