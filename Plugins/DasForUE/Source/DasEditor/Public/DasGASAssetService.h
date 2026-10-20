#pragma once
#include "CoreMinimal.h"
class UBlueprint;
class DASEDITOR_API FDasGASAssetService
{
public:
    // New classes or identical reimport only. Structural changes require a new folder.
    static bool Apply(const FString& Script, const FString& Folder, TMap<FString, UBlueprint*>& Out, FString& Error,
                      bool SaveAssets = true);
};
