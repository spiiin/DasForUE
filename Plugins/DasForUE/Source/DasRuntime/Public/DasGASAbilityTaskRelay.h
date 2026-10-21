#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "DasGASAdvancedTypes.h"
#include "DasGASAbilityTaskRelay.generated.h"
class UAbilityTask;
class UDasAuthoredGameplayAbility;
UCLASS()
class DASRUNTIME_API UDasGASAbilityTaskRelay : public UObject
{
    GENERATED_BODY()
public:
    UPROPERTY()
    TObjectPtr<UAbilityTask> Native;
    TWeakObjectPtr<UDasAuthoredGameplayAbility> Ability;
    int32 Id = 0, Serial = 0;
    FName Kind;
    bool Once = true;
    void Stop();
    void Emit(FName Phase, bool Terminal, const FGameplayEventData* Payload = nullptr);
    UFUNCTION()
    void Finished();
    UFUNCTION()
    void Event(FGameplayEventData Payload);
    UFUNCTION()
    void Completed();
    UFUNCTION()
    void BlendedIn();
    UFUNCTION()
    void BlendOut();
    UFUNCTION()
    void Interrupted();
    UFUNCTION()
    void Cancelled();
};
