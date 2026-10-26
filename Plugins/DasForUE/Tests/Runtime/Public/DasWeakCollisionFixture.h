#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DasWeakCollisionFixture.generated.h"
USTRUCT(BlueprintType)
struct FDasWeakCollisionSnapshot
{
    GENERATED_BODY()
    UPROPERTY()
    TWeakObjectPtr<UObject> Target;
    UPROPERTY()
    int32 Marker = 17;
};
UCLASS()
class DASRUNTIME_API UDasWeakCollisionFixture : public UActorComponent
{
    GENERATED_BODY()
public:
    UPROPERTY()
    TWeakObjectPtr<UObject> WeakTarget;
    UPROPERTY()
    TArray<TWeakObjectPtr<UObject>> WeakArray;
    UFUNCTION(BlueprintCallable)
    FDasWeakCollisionSnapshot MakeWeakSnapshot();
    UFUNCTION(BlueprintCallable)
    bool CollectWeakSnapshot(const FDasWeakCollisionSnapshot& Value);
};
