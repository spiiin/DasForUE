#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasRandomLibrary.generated.h"

UCLASS()
class DASRUNTIME_API UDasRandomLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    // Keep FRandomStream's engine algorithm while exposing its mutable seed by ref.
    UFUNCTION(BlueprintCallable, Category="daScript|Math")
    static int32 RandomIntegerInRange(UPARAM(ref) int32& Seed, int32 Minimum, int32 Maximum);

    UFUNCTION(BlueprintCallable, Category="daScript|Math")
    static float RandomFloatInRange(UPARAM(ref) int32& Seed, float Minimum, float Maximum);
};
