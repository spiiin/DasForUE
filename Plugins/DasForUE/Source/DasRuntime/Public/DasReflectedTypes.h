#pragma once
#include "DasAuthoring.h"
DASRUNTIME_API bool IsDasMathStructPath(const FString& Path);
struct FDasEnumEntry
{
    FString Name;
    int64 Value = 0;
    FString Id;
};
struct FDasEnumDeclaration
{
    FString Id, Name;
    TArray<FDasEnumEntry> Entries;
    TMap<FString, FString> Metadata;
};
struct DASRUNTIME_API FDasReflectedTypes
{
    FString Id;
    TArray<FDasEnumDeclaration> Enums;
    TArray<FDasDeclaredClass> Structs;
    bool Validate(FString& Error, bool AllowUnresolved = false) const;
    bool ToJson(FString& Out, FString& Error, bool AllowUnresolved = false) const;
    static bool FromJson(const FString& Json, FDasReflectedTypes& Out, FString& Error, bool AllowUnresolved = false);
};
