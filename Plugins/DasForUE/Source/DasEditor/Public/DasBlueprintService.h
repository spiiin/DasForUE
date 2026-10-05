#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"

class UBlueprint;

// Game-thread editor backend. Preflight failures do not mutate the Blueprint.
// Full rollback after a Blueprint compiler failure is not implemented yet.
class DASEDITOR_API FDasBlueprintService
{
public:
    static bool ReadSchema(const FString& ScriptFile, FDasClassSchema& Out, FString& Error);
    static bool Apply(UBlueprint* Blueprint, const FDasClassSchema& Schema, FString& Error);
};
