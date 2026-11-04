#pragma once
#include "CoreMinimal.h"
#include "DasCollisionTypes.h"
#include "Engine/EngineTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasCollisionLibrary.generated.h"

class AController;
class URadialForceComponent;

// Reconstruct the engine hit value at the boundary; gameplay stays in daScript.
UCLASS()
class DASRUNTIME_API UDasCollisionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    // Native RadialForce only exposes removal by EObjectTypeQuery to Blueprint.
    UFUNCTION(BlueprintCallable, Category="daScript|Collision")
    static bool RemoveRadialForceCollisionChannel(URadialForceComponent* Force,
                                                   TEnumAsByte<ECollisionChannel> Channel);

    UFUNCTION(BlueprintCallable, Category = "daScript|Collision")
    static float ApplyPointDamage(AActor* Target, float Damage, FVector Direction, const FDasCollisionHit& Hit,
                                  AController* Instigator, AActor* Source);
};
