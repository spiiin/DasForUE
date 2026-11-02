#pragma once
#include "CoreMinimal.h"
#include "DasCollisionTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasCollisionLibrary.generated.h"

class AController;

// Reconstruct the engine hit value at the boundary; gameplay stays in daScript.
UCLASS()
class DASRUNTIME_API UDasCollisionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "daScript|Collision")
    static float ApplyPointDamage(AActor* Target, float Damage, FVector Direction, const FDasCollisionHit& Hit,
                                  AController* Instigator, AActor* Source);
};
