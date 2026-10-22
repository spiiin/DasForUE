#pragma once
#include "CoreMinimal.h"
#include "DasBlueprintService.h"
#include "DasNumericLibrary.h"
struct FDasNumericMigrationOptions
{
    bool IncludeDependencies = false;
    bool AdaptGraphs = true;
    bool WidenOnly = false;
    EDasIntegerRule Integers = EDasIntegerRule::Exact;
    EDasFloatRule Floats = EDasFloatRule::Exact;
};
class UBlueprint;
// What a reimport would change, computed without changing anything. Auto-reimport asks before applying a field
// migration that also modifies saved maps or other Blueprints.
struct DASEDITOR_API FDasReimportPlan
{
    // One entry per field: "Name", "Old -> New", "Name: int -> float", "Name: 10 -> 20".
    TArray<FString> Added, Removed, Renamed, Retyped, Defaults;
    bool OtherDefaults = false;
    // Saved packages outside the class (and its own DataAsset instances) that the change modifies.
    TArray<FString> Maps, Blueprints;
    bool NeedsConfirmation = false;
    // "Removed 12, renamed 3, retyped 1, new defaults 40".
    FString Summary() const;
    // The summary, at most MaxFields fields (most significant first) and the affected assets (names up to
    // MaxAssets, counts beyond).
    FString Short(int32 MaxFields = 5, int32 MaxAssets = 3) const;
    // Every group with its count; at most MaxPerGroup entries per group.
    FString Details(int32 MaxPerGroup = 200) const;
};
// Explicit game-thread DSL import. Assets stay dirty until the user saves them.
class DASEDITOR_API FDasDSLService
{
public:
    static bool ReimportArchivedFields(UBlueprint* Blueprint, bool Restore, bool IncludeDependencies, FString& Error);
    static bool ReimportConvertedNumericFields(UBlueprint* Blueprint, const FDasNumericMigrationOptions& Options,
                                               FString& Error);
    static bool ReimportWidenedNumericFields(UBlueprint* Blueprint, FString& Error, bool IncludeDependencies = false);
    static bool ReimportRenamedFields(UBlueprint* Blueprint, FString& Error, bool IncludeDependencies = false);
    static bool Apply(const FString& ScriptName, const FString& PackageName, const FString& ClassId, UBlueprint*& Out,
                      FString& Error, EDasDefaultMigrationPolicy DefaultPolicy = EDasDefaultMigrationPolicy::Refuse);
    // IncludeDependencies lets the automatic field migrations (rename, archive, numeric) also modify saved maps
    // and other Blueprints that use the class (see PlanReimport).
    static bool Reimport(UBlueprint* Blueprint, FString& Error,
                         EDasDefaultMigrationPolicy DefaultPolicy = EDasDefaultMigrationPolicy::Automatic,
                         bool IncludeDependencies = false);
    static bool PlanReimport(UBlueprint* Blueprint, FDasReimportPlan& Out, FString& Error);
};
