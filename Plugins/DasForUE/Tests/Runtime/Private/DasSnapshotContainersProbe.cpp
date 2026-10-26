#include "DasSnapshotContainers.h"
#include "DasPropertyValues.h"

namespace DasSnapshotContainers
{
namespace
{
struct FNativeValue
{
    FProperty* Property;
    void* Memory;
    explicit FNativeValue(FProperty* P) : Property(P), Memory(FMemory::Malloc(P->GetSize(), P->GetMinAlignment()))
    {
        P->InitializeValue(Memory);
    }
    ~FNativeValue()
    {
        Property->DestroyValue(Memory);
        FMemory::Free(Memory);
    }
};
template <class View> bool CheckOrder(const View& Values, FString& Error)
{
    FKeyOrderCache Cache;
    const auto& First = Cache.Get(1, TEXT("Values"), Values);
    if (First.Num() != Values.Num())
    {
        Error = TEXT("Key order count mismatch");
        return false;
    }
    const int32* Data = First.GetData();
    TSet<int32> Seen;
    DasSnapshotFields::TField<const uint8> Previous{}, Current{};
    for (int32 Slot : First)
    {
        if (Seen.Contains(Slot) || !Values.KeyAtSlot(Slot, Current, Error))
            return false;
        Seen.Add(Slot);
        const auto Kind = DasPropertyKind(Current.Property);
        if (Previous.Property && ((Kind == EKind::Int && *reinterpret_cast<const int32*>(Previous.Memory) >
                                                             *reinterpret_cast<const int32*>(Current.Memory)) ||
                                  (Kind == EKind::String && *reinterpret_cast<const FString*>(Current.Memory) <
                                                                *reinterpret_cast<const FString*>(Previous.Memory))))
        {
            Error = TEXT("Key order changed sorted contract");
            return false;
        }
        Previous = Current;
    }
    for (int32 I = 0; I < 20; ++I)
        if (Cache.Get(1, TEXT("Values"), Values).GetData() != Data || Cache.BuildCount() != 1)
        {
            Error = TEXT("Repeated traversal rebuilt key order");
            return false;
        }
    Cache.Get(2, TEXT("Values"), Values);
    Cache.Prune([](uint64 Token) { return Token == 2; });
    if (Cache.Num() != 1 || Cache.SlotCount() != Values.Num())
    {
        Error = TEXT("Collected snapshot retained traversal cache");
        return false;
    }
    Cache.Get(2, TEXT("OtherField"), Values);
    if (Cache.Num() != 2 || Cache.BuildCount() != 3)
    {
        Error = TEXT("Key order cache aliased snapshot or field identity");
        return false;
    }
    for (uint64 Token = 3; Token < 140; ++Token)
        Cache.Get(Token, TEXT("Values"), Values);
    if (Cache.Num() > FKeyOrderCache::MaxOrders || Cache.SlotCount() > FKeyOrderCache::MaxSlots)
    {
        Error = TEXT("Key order cache exceeded budget");
        return false;
    }
    FKeyOrderCache Uncached;
    const FString LongPath = FString::ChrN(FKeyOrderCache::MaxPathLength + 1, TEXT('x'));
    Uncached.Get(1, LongPath, Values);
    Uncached.Get(1, LongPath, Values);
    if (Uncached.Num() != 0 || Uncached.BuildCount() != 2)
    {
        Error = TEXT("Unbounded path retained by key order cache");
        return false;
    }
    return true;
}
}
bool CheckProperty(FProperty* Property, void* Memory, FString& Error)
{
    auto Fail = [&](const TCHAR* Why)
    {
        Error = FString(TEXT("Snapshot container view: ")) + Why;
        return false;
    };
    DasSnapshotFields::TField<uint8> Field{Property, static_cast<uint8*>(Memory)};
    DasSnapshotFields::TField<const uint8> ConstField{Property, static_cast<const uint8*>(Memory)}, Read{};
    FString Rejected;
    if (auto* P = CastField<FMapProperty>(Property))
    {
        TMapView<uint8> Mutable;
        TMapView<const uint8> Borrowed;
        const auto KeyKind = DasPropertyKind(P->KeyProp), ValueKind = DasPropertyKind(P->ValueProp);
        if (!Map(Field, KeyKind, ValueKind, Mutable, Error) || !Map(ConstField, KeyKind, ValueKind, Borrowed, Error))
            return false;
        if (Map(ConstField, KeyKind == EKind::Int ? EKind::Byte : EKind::Int, ValueKind, Borrowed, Rejected) ||
            Rejected.IsEmpty())
            return Fail(TEXT("wrong map key accepted"));
        if (Map(ConstField, KeyKind, ValueKind == EKind::Int ? EKind::Byte : EKind::Int, Borrowed, Rejected))
            return Fail(TEXT("wrong map value accepted"));
        FNativeValue Key(P->KeyProp), Value(P->ValueProp);
        FDasScalarValue Encoded;
        if (!ReadDasPropertyValue(P->ValueProp, Value.Memory, Encoded, Error))
            return false;
        auto Write = [&](FProperty* Leaf, void* Target, FString& Issue)
        {
            return WriteDasPropertyValue(Leaf, Target, Encoded, Issue);
        };
        if (!Mutable.Upsert(Key.Memory, Write, Error) || !Mutable.Upsert(Key.Memory, Write, Error) ||
            Mutable.Num() != 1 || Borrowed.Num() != 1)
            return Fail(TEXT("map insert/update"));
        if (!Borrowed.Find(Key.Memory, Read, Error) || Read.Memory != FScriptMapHelper(P, Memory).GetValuePtr(0) ||
            !P->ValueProp->Identical(Read.Memory, Value.Memory))
            return Fail(TEXT("map read must alias stored native value"));
        if (!Borrowed.KeyAt(0, Read, Error) || Borrowed.KeyAt(1, Read, Rejected) || Borrowed.KeyAt(-1, Read, Rejected))
            return Fail(TEXT("map key bounds"));
        if (KeyKind == EKind::String)
        {
            FString Z = TEXT("z"), A = TEXT("a");
            if (!Mutable.Upsert(&Z, Write, Error) || !Mutable.Upsert(&A, Write, Error))
                return false;
        }
        if (!CheckOrder(Borrowed, Error))
            return false;
        if (KeyKind == EKind::Int && ValueKind == EKind::Int)
        {
            int32 One = 1, Two = 2;
            Mutable.Upsert(&One, Write, Error);
            Mutable.Upsert(&Two, Write, Error);
            FScriptMapHelper Setup(P, Memory);
            Setup.RemoveAt(Setup.FindMapIndexWithKey(&One));
            if (!Borrowed.KeyAt(1, Read, Error) || *reinterpret_cast<const int32*>(Read.Memory) != 2 ||
                Borrowed.Find(&One, Read, Rejected))
                return Fail(TEXT("sparse map hole or absent key"));
            int32 Negative = -3;
            if (!Mutable.Upsert(&Negative, Write, Error) || !CheckOrder(Borrowed, Error))
                return false;
            {
                FScriptMapHelper SetupNegative(P, Memory);
                SetupNegative.RemoveAt(SetupNegative.FindMapIndexWithKey(&Negative));
            }
            for (int32 I = 1; I < 4096; ++I)
                if (!Mutable.Upsert(&I, Write, Error))
                    return false;
            int32 Extra = 4096;
            bool Called = false;
            if (Mutable.Upsert(
                    &Extra,
                    [&](FProperty*, void*, FString&)
                    {
                        Called = true;
                        return true;
                    },
                    Rejected) ||
                Called || Mutable.Num() != 4096 || !Mutable.Upsert(&One, Write, Error))
                return Fail(TEXT("map capacity/update semantics"));
            if (!CheckOrder(Borrowed, Error))
                return false;
        }
        return true;
    }
    if (auto* P = CastField<FSetProperty>(Property))
    {
        TSetView<uint8> Mutable;
        TSetView<const uint8> Borrowed;
        const auto Kind = DasPropertyKind(P->ElementProp);
        if (!Set(Field, Kind, Mutable, Error) || !Set(ConstField, Kind, Borrowed, Error))
            return false;
        if (Set(ConstField, Kind == EKind::Int ? EKind::Byte : EKind::Int, Borrowed, Rejected))
            return Fail(TEXT("wrong set element accepted"));
        FNativeValue Key(P->ElementProp);
        if (!Mutable.Set(Key.Memory, true, Error) || !Mutable.Set(Key.Memory, true, Error) ||
            !Borrowed.Contains(Key.Memory) || Borrowed.Num() != 1)
            return Fail(TEXT("set insert/contains"));
        if (!Borrowed.ElementAt(0, Read, Error) || Read.Memory != FScriptSetHelper(P, Memory).GetElementPtr(0) ||
            Borrowed.ElementAt(1, Read, Rejected) || Borrowed.ElementAt(-1, Read, Rejected))
            return Fail(TEXT("set element alias/bounds"));
        if (Kind == EKind::String)
        {
            FString Z = TEXT("z"), A = TEXT("a");
            if (!Mutable.Set(&Z, true, Error) || !Mutable.Set(&A, true, Error))
                return false;
        }
        if (!CheckOrder(Borrowed, Error))
            return false;
        if (Kind == EKind::Int)
        {
            int32 One = 1, Two = 2;
            Mutable.Set(&One, true, Error);
            Mutable.Set(&Two, true, Error);
            Mutable.Set(&One, false, Error);
            if (!Borrowed.ElementAt(1, Read, Error) || *reinterpret_cast<const int32*>(Read.Memory) != 2 ||
                Borrowed.Contains(&One))
                return Fail(TEXT("sparse set hole"));
            int32 Negative = -3;
            if (!Mutable.Set(&Negative, true, Error) || !CheckOrder(Borrowed, Error) ||
                !Mutable.Set(&Negative, false, Error))
                return false;
            for (int32 I = 1; I < 4096; ++I)
                if (!Mutable.Set(&I, true, Error))
                    return false;
            int32 Extra = 4096;
            if (Mutable.Set(&Extra, true, Rejected) || Mutable.Num() != 4096 || !Mutable.Set(&One, true, Error) ||
                !Mutable.Set(&One, false, Error))
                return Fail(TEXT("set capacity/update semantics"));
        }
        if (!Mutable.Set(Key.Memory, false, Error) || Borrowed.Contains(Key.Memory))
            return Fail(TEXT("set remove"));
        return true;
    }
    if (CastField<FArrayProperty>(Property))
    {
        TArrayView<uint8> Mutable;
        TArrayView<const uint8> Borrowed;
        if (!Array(Field, Mutable, Error) || !Array(ConstField, Borrowed, Error) || !Mutable.Resize(2, Error) ||
            Borrowed.Num() != 2 || !Borrowed.ElementAt(1, Read, Error))
            return Fail(TEXT("array resize/read"));
        if (Borrowed.ElementAt(-1, Read, Rejected) || Borrowed.ElementAt(2, Read, Rejected) ||
            Mutable.Resize(-1, Rejected) || Mutable.Resize(4097, Rejected) || Borrowed.Num() != 2)
            return Fail(TEXT("array bounds/failed resize"));
    }
    return true;
}
}
