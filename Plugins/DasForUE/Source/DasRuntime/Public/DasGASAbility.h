#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "DasGASAdvancedTypes.h"
#include "DasGASAbility.generated.h"
class UDasAuthoredAbilitySystemComponent;
class UDasAuthoredGameplayEffect;
class UDasAuthoredGameplayAbility;
class UDasGASAbilityTaskRelay;
class UAnimMontage;

USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredAbilityContext
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredGameplayAbility> Ability = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Owner = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Avatar = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredAbilitySystemComponent> System = nullptr;
    UPROPERTY(BlueprintReadWrite)
    int32 GrantId = 0;
    UPROPERTY(BlueprintReadWrite)
    int32 ActivationId = 0;
    UPROPERTY(BlueprintReadWrite)
    int32 Level = 0;
    UPROPERTY(BlueprintReadWrite)
    FName Phase;
    UPROPERTY(BlueprintReadWrite)
    FDasGASAbilityTaskResult Task;
    UPROPERTY(BlueprintReadWrite)
    bool Cancelled = false;
};
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
    UPROPERTY(BlueprintReadWrite)
    bool Committed = false;
    UPROPERTY(BlueprintReadWrite)
    FString Error;
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
    UPROPERTY()
    FString ScriptFile;
    UPROPERTY()
    FString ScriptContract;
    UPROPERTY()
    FString ActivateEntry;
    UPROPERTY()
    FString EndEntry;
    UPROPERTY()
    FString CancelEntry;
    UPROPERTY()
    FString TaskEntry;
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasAuthoredAbilityContext ScriptAbilityContext() const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool CommitScriptAbility(int32 ActivationId);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool EndScriptAbility(int32 ActivationId);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool CancelScriptAbility(int32 ActivationId);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool ApplyScriptAbilityEffect(int32 ActivationId);
    UFUNCTION(BlueprintPure, Category = "GAS")
    bool IsScriptActivationCurrent(int32 Id) const { return ValidScriptActivation(Id); }
    UFUNCTION(BlueprintCallable, Category = "GAS")
    int32 WaitScriptDelay(int32 ActivationId, float Seconds);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    int32 WaitScriptEvent(int32 ActivationId, FName Tag, bool Once = true, bool Exact = true);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    int32 PlayScriptMontage(int32 ActivationId, UAnimMontage* Montage, float Rate = 1.f, FName Section = NAME_None);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool CancelScriptTask(int32 ActivationId, int32 TaskId);
    UFUNCTION(BlueprintPure, Category = "GAS")
    int32 ScriptTaskCount() const { return ScriptTasks.Num(); }
    void DeliverScriptTask(UDasGASAbilityTaskRelay* Relay, const FDasGASAbilityTaskResult& Result, bool Terminal);
    UFUNCTION(BlueprintPure, Category = "GAS")
    bool IsAuthoredCommitted() const { return CommittedForActivation; }
    const FString& GetScriptError() const { return ScriptError; }
    virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* Info, const FGameplayAbilitySpec& Spec) override;
    virtual void BeginDestroy() override;
    virtual UGameplayEffect* GetCostGameplayEffect() const override;
    virtual UGameplayEffect* GetCooldownGameplayEffect() const override;
    virtual const FGameplayTagContainer* GetCooldownTags() const override;
    virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                                    const FGameplayTagContainer* SourceTags = nullptr,
                                    const FGameplayTagContainer* TargetTags = nullptr,
                                    FGameplayTagContainer* RelevantTags = nullptr) const override;
    virtual bool CheckCost(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                           FGameplayTagContainer* RelevantTags = nullptr) const override;
    virtual void ApplyCooldown(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                               FGameplayAbilityActivationInfo Activation) const override;
    virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                                 FGameplayAbilityActivationInfo Activation, const FGameplayEventData* Event) override;
    virtual void EndAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* Info,
                            FGameplayAbilityActivationInfo Activation, bool Replicate, bool Cancelled) override;

private:
    UPROPERTY(Transient)
    TMap<int32, TObjectPtr<UDasGASAbilityTaskRelay>> ScriptTasks;
    int32 NextScriptTask = 1;
    FDasGASAbilityTaskResult CurrentTask;
    UDasGASAbilityTaskRelay* NewScriptTask(int32 Serial, FName Kind);
    void ClearScriptTasks();
    int32 ActivationSerial = 0;
    int32 ScriptGrantId = 0;
    int32 ScriptLevel = 0;
    bool CommittedForActivation = false;
    bool ScriptEnding = false;
    bool ScriptActivating = false;
    bool ScriptCancelled = false;
    FName ScriptPhase;
    FString ScriptError;
    TWeakObjectPtr<AActor> ScriptOwner;
    TWeakObjectPtr<AActor> ScriptAvatar;
    TWeakObjectPtr<UDasAuthoredAbilitySystemComponent> ScriptSystem;
    bool ValidScriptActivation(int32 Id) const;
    bool RunScriptHook(const FString& Entry, FName Phase, bool Cancelled = false);
    void ReleaseScript();
};
