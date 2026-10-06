#pragma once
#include "CoreMinimal.h"
#include "DasBlueprintService.h"
class UBlueprint;
// Explicit game-thread DSL import. Assets stay dirty until the user saves them.
class DASEDITOR_API FDasDSLService {
public:
    static bool ReimportWidenedNumericFields(UBlueprint* Blueprint,FString& Error,bool IncludeDependencies=false);
    static bool ReimportRenamedFields(UBlueprint* Blueprint,FString& Error,bool IncludeDependencies=false);
    static bool Apply(const FString& ScriptName,const FString& PackageName,const FString& ClassId,UBlueprint*& Out,FString& Error,EDasDefaultMigrationPolicy DefaultPolicy=EDasDefaultMigrationPolicy::Refuse);
    static bool Reimport(UBlueprint* Blueprint,FString& Error,EDasDefaultMigrationPolicy DefaultPolicy=EDasDefaultMigrationPolicy::Refuse);
};
