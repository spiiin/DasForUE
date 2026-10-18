#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "DasConstraintTransportFixture.generated.h"
UCLASS()
class DASRUNTIME_API UDasConstraintTransportFixture:public UBlueprintFunctionLibrary {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,meta=(WorldContext="WorldContextObject")) static UPhysicsConstraintComponent* CreateTestConstraint(const UObject* WorldContextObject);
 UFUNCTION(BlueprintCallable) static void RebuildConstraint(UPhysicsConstraintComponent* Component);
 UFUNCTION(BlueprintCallable) static void DestroyOwner(UPhysicsConstraintComponent* Component);
 UFUNCTION(BlueprintCallable) static void CollectWeak();
 UFUNCTION(BlueprintCallable) static bool LastOwnerCollected();
};
