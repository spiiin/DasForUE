#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "DasGASAbility.generated.h"
class UDasAuthoredAbilitySystemComponent;
class UDasAuthoredGameplayEffect;

USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredAbilityGrant
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredAbilitySystemComponent> Target = nullptr;
    UPROPERTY(BlueprintReadWrite)
    int32 Id = 0;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredAbilityState
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    FName Phase;
    UPROPERTY(BlueprintReadWrite)
    int32 GrantId = 0;
    UPROPERTY(BlueprintReadWrite)
    bool Cancelled = false;
    UPROPERTY(BlueprintReadWrite)
    bool Granted = false;
    UPROPERTY(BlueprintReadWrite)
    bool Active = false;
    UPROPERTY(BlueprintReadWrite)
    bool CanActivate = false;
    UPROPERTY(BlueprintReadWrite)
    FName Ability;
    UPROPERTY(BlueprintReadWrite)
    int32 Level = 0;
    UPROPERTY(BlueprintReadWrite)
    float CooldownRemaining = 0;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasAuthoredAbilityChanged, FDasAuthoredAbilityState, Change);

// One native definition; each ability spec adds its own duration and granted tag.
UCLASS()
class DASRUNTIME_API UDasAuthoredAbilityCooldown : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UDasAuthoredAbilityCooldown();
};
UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasAuthoredGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UDasAuthoredGameplayAbility();
    void Finish()
    {
        if (IsActive())
            K2_EndAbility();
    }
    UPROPERTY()
    FString DefinitionId;
    UPROPERTY()
    FString DefinitionHash;
    UPROPERTY()
    FName DefinitionName;
    UPROPERTY()
    TSubclassOf<UDasAuthoredGameplayEffect> Effect;
    UPROPERTY()
    TSubclassOf<UDasAuthoredGameplayEffect> Cost;
    UPROPERTY()
    float CooldownSeconds = 0;
    UPROPERTY()
    FGameplayTagContainer CooldownTags;
    UPROPERTY()
    bool AutoEnd = true;
    virtual UGameplayEffect* GetCostGameplayEffect() const override;
    virtual UGameplayEffect* GetCooldownGameplayEffect() const override;
    virtual const FGameplayTagContainer* GetCooldownTags() const override;
    virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                           FGameplayTagContainer* RelevantTags = nullptr) const override;
    virtual void ApplyCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                               FGameplayAbilityActivationInfo Activation) const override;
    virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                                 FGameplayAbilityActivationInfo Activation, const FGameplayEventData* Event) override;
    virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                            FGameplayAbilityActivationInfo Activation, bool Replicate, bool Cancelled) override;
};
