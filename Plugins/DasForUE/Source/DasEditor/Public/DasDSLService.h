#pragma once
#include "CoreMinimal.h"
#include "DasBlueprintService.h"
#include "DasNumericLibrary.h"
struct FDasNumericMigrationOptions {bool IncludeDependencies=false;bool AdaptGraphs=true;bool WidenOnly=false;EDasIntegerRule Integers=EDasIntegerRule::Exact;EDasFloatRule Floats=EDasFloatRule::Exact;};
class UBlueprint;
// Explicit game-thread DSL import. Assets stay dirty until the user saves them.
class DASEDITOR_API FDasDSLService {
public:
    static bool ReimportArchivedFields(UBlueprint* Blueprint,bool Restore,bool IncludeDependencies,FString& Error);
    static bool ReimportConvertedNumericFields(UBlueprint* Blueprint,const FDasNumericMigrationOptions& Options,FString& Error);
    static bool ReimportWidenedNumericFields(UBlueprint* Blueprint,FString& Error,bool IncludeDependencies=false);
    static bool ReimportRenamedFields(UBlueprint* Blueprint,FString& Error,bool IncludeDependencies=false);
    static bool Apply(const FString& ScriptName,const FString& PackageName,const FString& ClassId,UBlueprint*& Out,FString& Error,EDasDefaultMigrationPolicy DefaultPolicy=EDasDefaultMigrationPolicy::Refuse);
    static bool Reimport(UBlueprint* Blueprint,FString& Error,EDasDefaultMigrationPolicy DefaultPolicy=EDasDefaultMigrationPolicy::Automatic);
};
