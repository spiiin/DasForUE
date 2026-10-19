#pragma once
#include "CoreMinimal.h"
#include "DasPropertyAccess.h"
#if DAS_PROJECT_NATIVE_GAMEPLAY
#include "DasProjectNativeGameplay.generated.h"
#else
#include "Generated/DasNativeGameplay.generated.h"
#endif
inline FDasPropertyAccess DasNativeAccessContract(const FDasNativeGameplayField& Row)
{
    FDasPropertyAccess A;
    A.NativeGetter = Row.NativeGetter;
    A.NativeSetter = Row.NativeSetter;
    A.GetterUnsafe = Row.GetterUnsafe;
    A.SetterUnsafe = Row.SetterUnsafe;
    A.Getter = FName(Row.Getter);
    A.Setter = FName(Row.Setter);
    return A;
}
// Registry lookups run on every native field access, so rows are indexed once.
// The map is keyed case-insensitively; candidates are then matched case-sensitively.
template <class Row, SIZE_T N>
const Row* DasFindNativeGameplayRow(const Row (&Rows)[N], const FString& Class, const FString& Name)
{
    static const TMap<FString, TArray<const Row*>> Index = [&Rows]()
    {
        TMap<FString, TArray<const Row*>> Result;
        for (const Row& Entry : Rows)
            Result.FindOrAdd(FString(Entry.Class) + TEXT("|") + Entry.Name).Add(&Entry);
        return Result;
    }();
    if (const auto* Candidates = Index.Find(Class + TEXT("|") + Name))
        for (const Row* Entry : *Candidates)
            if (Class.Equals(Entry->Class, ESearchCase::CaseSensitive) &&
                Name.Equals(Entry->Name, ESearchCase::CaseSensitive))
                return Entry;
    return nullptr;
}
inline const FDasNativeGameplayField* DasNativeGameplayField(const FString& Class, const FString& Name)
{
    return DasFindNativeGameplayRow(DasNativeGameplayFields, Class, Name);
}
inline const FDasNativeGameplayMethod* DasNativeGameplayMethod(const FString& Class, const FString& Name)
{
    return DasFindNativeGameplayRow(DasNativeGameplayMethods, Class, Name);
}
// Native rows by property name, for access-contract lookup from an FProperty.
inline const TArray<const FDasNativeGameplayField*>* DasNativeGameplayFieldsNamed(FName Name)
{
    static const TMap<FName, TArray<const FDasNativeGameplayField*>> Index = []()
    {
        TMap<FName, TArray<const FDasNativeGameplayField*>> Result;
        for (const auto& Entry : DasNativeGameplayFields)
            Result.FindOrAdd(FName(Entry.Name)).Add(&Entry);
        return Result;
    }();
    return Index.Find(Name);
}
