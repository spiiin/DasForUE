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
    // Header inventory only; the compiler remains authoritative for declarations, bodies and identities.
    static bool DeclarationInventory(const TMap<FString, FString>& Sources, FDasSourceCatalog& Out, FString& Error);
    static bool UpdateDeclarationInventory(const FString& ScriptsFolder, FString& Error);
    bool Validate(FString& Error) const;
    bool Resolve(const FString& Identity, FDasSourceCatalogEntry& Out, FString& Error) const;
    bool ResolveType(const FString& Kind, const FString& Identity, FDasCatalogValueModule& Module,
                     FDasCatalogValueType& Type, FString& Error) const;
    bool ToJson(FString& Out, FString& Error) const;
    static bool FromJson(const FString& Wire, FDasSourceCatalog& Out, FString& Error);
    bool ReferenceModule(const FString& Module, const FString& Catalog, FString& Out, FString& Error) const;
    static bool LocalFile(const FString& File, const FString& Extension);
};
