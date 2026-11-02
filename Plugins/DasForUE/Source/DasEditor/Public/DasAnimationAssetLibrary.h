#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasAnimationAssetLibrary.generated.h"
class UAnimBlueprint;
class USkeleton;
class UBlendSpace;

UCLASS()
class DASEDITOR_API UDasAnimationAssetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    // Builds a presentation-only AnimGraph: locomotion blendspace -> montage slot -> pose.
    // All gameplay stays with the caller; no course Blueprint or native parent is copied.
    UFUNCTION(BlueprintCallable, Category = "daScript|Animation")
    static UAnimBlueprint* CreateLocomotionBlueprint(const FString& PackagePath, USkeleton* Skeleton,
                                                     UBlendSpace* BlendSpace, int32 SpeedAxis, int32 DirectionAxis,
                                                     FName MontageSlot, FString& Error);
};
