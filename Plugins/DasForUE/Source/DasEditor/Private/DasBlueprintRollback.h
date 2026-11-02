#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"
#include "UObject/StrongObjectPtr.h"
class UBlueprint;
struct FAssetData;
class FDasBlueprintRollback
{
    struct FImpl;
    TUniquePtr<FImpl> Impl;

public:
    FDasBlueprintRollback();
    ~FDasBlueprintRollback();
    static bool GatherScope(UBlueprint* Root, TArray<UBlueprint*>& Blueprints,
                            TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error, bool DefaultsOnly = false,
                            bool AllowIndependentSources = false);
    // A saved DataAsset of Root's class family: an instance migrated, and marked for saving, with the class.
    static bool InstanceAsset(const FAssetData& Data, const UClass* Root);
    bool Prepare(UBlueprint* Root, const FDasClassSchema& Fields, const TArray<UBlueprint*>& Blueprints,
                 const TArray<TStrongObjectPtr<UObject>>& Assets, FString& Error);
    bool Finish(FString& Error);
    bool Restore(FString& Error);
};

// A preflighted interface transaction may reimport several independent DSL roots.
// Nested class migrations can touch only Blueprints already captured by that transaction.
class FDasBlueprintMigrationBatch
{
    TSet<const UBlueprint*> Allowed;
    const TSet<const UBlueprint*>* Previous = nullptr;

public:
    explicit FDasBlueprintMigrationBatch(const TArray<UBlueprint*>& Scope);
    ~FDasBlueprintMigrationBatch();
    static bool Contains(const UBlueprint* Blueprint);
    FDasBlueprintMigrationBatch(const FDasBlueprintMigrationBatch&) = delete;
    FDasBlueprintMigrationBatch& operator=(const FDasBlueprintMigrationBatch&) = delete;
};
