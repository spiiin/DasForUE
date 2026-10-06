#pragma once
#include "CoreMinimal.h"
class UBlueprint;
// Explicit game-thread DSL import. Assets stay dirty until the user saves them.
class DASEDITOR_API FDasDSLService {
public:
    static bool Apply(const FString& ScriptName,const FString& PackageName,const FString& ClassId,UBlueprint*& Out,FString& Error);
    static bool Reimport(UBlueprint* Blueprint,FString& Error);
};
