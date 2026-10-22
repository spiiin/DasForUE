#pragma once
#include "CoreMinimal.h"
class UBlueprint;
class DASEDITOR_API FDasGASAssetService
{
public:
    // Creates the GAS assets of Script in Folder, or updates the existing ones in place: changed effects,
    // abilities, calculations and cues are rewritten, new attributes are added to their AttributeSet. Removed
    // or renamed attributes, changed cue tags and identities are refused (import into a new folder); a failure
    // restores every updated asset. Updated (optional) receives the number of assets changed in place.
    static bool Apply(const FString& Script, const FString& Folder, TMap<FString, UBlueprint*>& Out, FString& Error,
                      bool SaveAssets = true, int32* Updated = nullptr);
};
