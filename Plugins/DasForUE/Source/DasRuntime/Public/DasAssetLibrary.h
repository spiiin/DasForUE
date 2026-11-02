#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasAssetLibrary.generated.h"

// Optional asset dependencies must not make a source-only sample fail to start.
UCLASS()
class DASRUNTIME_API UDasAssetLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "daScript|Assets")
    static UObject* LoadOptionalAsset(const FString& AssetPath);
};
