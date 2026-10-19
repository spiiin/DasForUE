#pragma once
#include "CoreMinimal.h"

// Source locations only. Reflected assets remain resolved through existing registries.
struct DASRUNTIME_API FDasSourceCatalogEntry
{
    FString Id, Name, Script, Reference;
};
struct DASRUNTIME_API FDasCatalogValueType
{
    FString Kind, Id, Name;
};
struct DASRUNTIME_API FDasCatalogValueModule
{
    FString Module, Script;
    TArray<FDasCatalogValueType> Types;
};
struct DASRUNTIME_API FDasSourceCatalog
{
    TArray<FDasSourceCatalogEntry> Entries;
    TArray<FDasCatalogValueModule> ValueModules;
    bool Validate(FString& Error) const;
    bool Resolve(const FString& Identity, FDasSourceCatalogEntry& Out, FString& Error) const;
    bool ToJson(FString& Out, FString& Error) const;
    static bool FromJson(const FString& Wire, FDasSourceCatalog& Out, FString& Error);
    bool ReferenceModule(const FString& Module, const FString& Catalog, FString& Out, FString& Error) const;
    static bool LocalFile(const FString& File, const FString& Extension);
};
