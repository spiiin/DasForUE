#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "DasGASAbility.h"
#include "DasGASCues.h"
#include "DasGASAuthoring.generated.h"

USTRUCT()
struct DASRUNTIME_API FDasGASAttributeRule
{
    GENERATED_BODY()
    UPROPERTY()
    FName Name;
    UPROPERTY()
    float Initial = 0;
    UPROPERTY()
    bool HasMin = false;
    UPROPERTY()
    float Minimum = 0;
    UPROPERTY()
    bool HasMax = false;
    UPROPERTY()
    float Maximum = 0;
    UPROPERTY()
    FName MaximumAttribute;
};
struct FDasGASSetDeclaration
{
    FString Name, Id;
    TArray<FDasGASAttributeRule> Attributes;
};
struct FDasGASModifierDeclaration
{
    FName Attribute;
    FString Operation = TEXT("Add");
    float Magnitude = 0;
    FName SetByCaller;
    FString Calculation;
};
struct FDasGASCueDeclaration
{
    FString Name, Id;
    FName Tag;
    float MinLevel = 0, MaxLevel = 1;
};
struct FDasGASEffectDeclaration
{
    FString Name, Id, Attributes;
    TArray<FDasGASModifierDeclaration> Modifiers;
    TArray<FString> Cues;
    FString Execution;
    FString Duration = TEXT("Instant");
    float Seconds = 0;
    float Period = 0;
    bool ExecuteOnApplication = true;
    FString Stacking = TEXT("None");
    int32 StackLimit = 0;
    FString StackDuration = TEXT("RefreshOnSuccessfulApplication");
    FString StackPeriod = TEXT("ResetOnSuccessfulApplication");
    FString StackExpiration = TEXT("ClearEntireStack");
    bool DenyOverflow = true;
};
struct FDasGASAbilityDeclaration
{
    FString Name, Id, Effect, Cost;
    FString ScriptFile, ScriptContract, ActivateEntry, EndEntry, CancelEntry, TaskEntry;
    float Cooldown = 0;
    FName CooldownTag;
    bool AutoEnd = true;
};
struct FDasGASOutputDeclaration
{
    FName Name, Attribute;
    FString Attributes, Operation = TEXT("Add");
};
struct FDasGASCallerDeclaration
{
    FName Name, Tag;
};
struct FDasGASCaptureDeclaration
{
    FName Name, Attribute;
    FString Attributes, Source = TEXT("Source");
    bool Snapshot = true;
    FString ScopedOperation;
    float ScopedMagnitude = 0;
};
struct FDasGASCalculationDeclaration
{
    FString Name, Id, Kind = TEXT("Magnitude"), Attributes, Output, Operation = TEXT("Add");
    FString ScriptFile, ScriptContract, Entry;
    TArray<FDasGASCaptureDeclaration> Captures;
    TArray<FDasGASOutputDeclaration> Outputs;
    TArray<FDasGASCallerDeclaration> Callers;
    bool StructuredOutput = false;
};
struct FDasGASDeclarations
{
    TArray<FDasGASCalculationDeclaration> Calculations;
    TArray<FDasGASSetDeclaration> Sets;
    TArray<FDasGASCueDeclaration> Cues;
    TArray<FDasGASEffectDeclaration> Effects;
    TArray<FDasGASAbilityDeclaration> Abilities;
};
DASRUNTIME_API bool ParseDasGASDeclarations(const FString& Json, FDasGASDeclarations& Out, FString& Error);

UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasAuthoredAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString DefinitionId;
    UPROPERTY()
    FName DefinitionName;
    UPROPERTY()
    FString DefinitionHash;
    UPROPERTY()
    TArray<FDasGASAttributeRule> Rules;
    FGameplayAttribute Attribute(FName Name) const;
    void InitializeDeclaredValues();
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& Value) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& Value) const override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
    void Clamp(const FGameplayAttribute& Attribute, float& Value) const;
};
UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasAuthoredGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString DefinitionId;
    UPROPERTY()
    FString DefinitionHash;
    UPROPERTY()
    TArray<FName> RequiredMagnitudes;
    UPROPERTY()
    TArray<TSubclassOf<UDasGameplayCueNotify>> CueAssets;
    bool PrepareCues() const;
};
class UDasAuthoredAbilitySystemComponent;
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredAttributeChange
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    FName Set;
    UPROPERTY(BlueprintReadWrite)
    FName Attribute;
    UPROPERTY(BlueprintReadWrite)
    float OldValue = 0;
    UPROPERTY(BlueprintReadWrite)
    float NewValue = 0;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredEffectSpec
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredAbilitySystemComponent> Source = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredGameplayEffect> Definition = nullptr;
    UPROPERTY(BlueprintReadWrite)
    float Level = 1;
    UPROPERTY(BlueprintReadWrite)
    TMap<FName, float> Magnitudes;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredEffectResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool Valid = false;
    UPROPERTY(BlueprintReadWrite)
    bool Applied = false;
    UPROPERTY(BlueprintReadWrite)
    bool HasActiveHandle = false;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredAbilitySystemComponent> Target = nullptr;
    UPROPERTY(BlueprintReadWrite)
    int32 ActiveId = 0;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasAuthoredEffectState
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool Active = false;
    UPROPERTY(BlueprintReadWrite)
    bool Infinite = false;
    UPROPERTY(BlueprintReadWrite)
    int32 Stacks = 0;
    UPROPERTY(BlueprintReadWrite)
    float Duration = 0;
    UPROPERTY(BlueprintReadWrite)
    float Remaining = 0;
    UPROPERTY(BlueprintReadWrite)
    float Period = 0;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasAuthoredCueReceived, FDasGASCueEvent, Cue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasAuthoredAttributeChanged, FDasAuthoredAttributeChange, Change);
UCLASS(ClassGroup = (Das), meta = (BlueprintSpawnableComponent))
class DASRUNTIME_API UDasAuthoredAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()
public:
    UDasAuthoredAbilitySystemComponent();
    virtual FGameplayEffectContextHandle MakeEffectContext() const override;
    UPROPERTY(BlueprintAssignable, Category = "GAS")
    FDasAuthoredAttributeChanged AttributeChanged;
    UPROPERTY(BlueprintAssignable, Category = "GAS")
    FDasAuthoredAbilityChanged AbilityChanged;
    UPROPERTY(BlueprintAssignable, Category = "GAS|Cues")
    FDasAuthoredCueReceived GameplayCueReceived;
    UFUNCTION(BlueprintCallable, Category = "GAS|Cues")
    bool WatchAuthoredCue(FName Tag);
    UFUNCTION(BlueprintCallable, Category = "GAS|Cues")
    bool UnwatchAuthoredCue(FName Tag);
    UFUNCTION(BlueprintCallable, Category = "GAS|Cues")
    bool ExecuteAuthoredCue(FName Tag, const FDasGASCueParameters& Parameters);
    UFUNCTION(BlueprintCallable, Category = "GAS|Cues")
    bool AddAuthoredCue(FName Tag, const FDasGASCueParameters& Parameters);
    UFUNCTION(BlueprintCallable, Category = "GAS|Cues")
    bool RemoveAuthoredCue(FName Tag);
    UFUNCTION(BlueprintPure, Category = "GAS|Cues")
    bool AuthoredCueActive(FName Tag) const;
    UFUNCTION(BlueprintPure, Category = "GAS|Cues")
    FDasGASCueEvent EmptyAuthoredCueEvent() const { return {}; }
    void ReceiveAuthoredCue(FName Tag, FName MatchedTag, FName Event, const FGameplayCueParameters& Parameters,
                            bool AuthoredAsset = false);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    FDasAuthoredAbilityGrant GiveAuthoredAbility(TSubclassOf<UDasAuthoredGameplayAbility> Class, int32 Level = 1);
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasAuthoredAbilityState AuthoredAbilityState(int32 Id) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool ActivateAuthoredAbility(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool CancelAuthoredAbility(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool FinishAuthoredAbility(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool RemoveAuthoredAbility(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    int32 BeginAuthoredTargeting(const FDasGASTargetingQuery& Query);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool UpdateAuthoredTargeting(int32 Id, const FDasGASTargetingQuery& Query);
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasGASTargetingPreview PreviewAuthoredTargeting(int32 Id) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    FDasGASTargetingResult ConfirmAuthoredTargeting(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool CancelAuthoredTargeting(int32 Id);
    int32 RegisterAuthoredTargetHit(const FHitResult& Hit);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool ReleaseAuthoredTargetHit(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool SendAuthoredGameplayEvent(FName Tag, float Magnitude, AActor* Instigator, AActor* Target);
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasGASTargetData MakeAuthoredTargets(const TArray<AActor*>& Actors, const TArray<int32>& Hits) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    TArray<FDasAuthoredEffectResult> ApplyAuthoredTargets(const FDasAuthoredEffectSpec& Spec,
                                                          const FDasGASTargetData& Targets);
    void NotifyAuthoredAbility(FGameplayAbilitySpecHandle Handle, FName Phase, bool Succeeded = false);
    int32 AuthoredAbilityId(FGameplayAbilitySpecHandle Handle) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool AddAuthoredAttributes(TSubclassOf<UDasAuthoredAttributeSet> Class);
    UFUNCTION(BlueprintPure, Category = "GAS")
    bool HasAuthoredAttribute(FName Set, FName Name) const;
    UFUNCTION(BlueprintPure, Category = "GAS")
    float ReadAuthoredAttribute(FName Set, FName Name) const;
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasAuthoredAttributeChange AuthoredAttributeSnapshot(FName Set, FName Name) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    FDasAuthoredEffectSpec MakeAuthoredSpec(TSubclassOf<UDasAuthoredGameplayEffect> Class, float Level);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    FDasAuthoredEffectResult ApplyAuthoredSpec(const FDasAuthoredEffectSpec& Spec);
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasAuthoredEffectState AuthoredEffectState(int32 ActiveId) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool RemoveAuthoredEffect(int32 ActiveId, int32 Stacks = -1);
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void BeginDestroy() override;

private:
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDasAuthoredAttributeSet>> Sets;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDasAuthoredGameplayEffect>> Definitions;
    TMap<FGameplayAttribute, FDelegateHandle> ChangeHandles;
    // Only native GAS handles are retained here; IDs are never reused on this ASC.
    TMap<int32, FActiveGameplayEffectHandle> ActiveEffects;
    int32 NextActiveId = 1;
    struct FGrantedAbility
    {
        FGameplayAbilitySpecHandle Handle;
        FName Name;
        bool ActivationSucceeded = false;
    };
    TMap<int32, FGrantedAbility> Abilities;
    int32 NextAbilityId = 1;
    bool Ended = false;
    void PruneActiveEffects();
    bool Live() const;
    bool InitializeAuthoredActorInfo();
    TMap<int32, FHitResult> TargetHits;
    int32 NextTargetHit = 1;
    struct FTargetSelection
    {
        TArray<TWeakObjectPtr<AActor>> Actors;
        TArray<FHitResult> Hits;
    };
    // Queries are GC-visible; selected actors/hit components remain weak until confirmation.
    UPROPERTY(Transient)
    TMap<int32, FDasGASTargetingQuery> TargetQueries;
    TMap<int32, FTargetSelection> TargetSelections;
    int32 NextTargetSelection = 1;
    bool SelectTargets(const FDasGASTargetingQuery& Query, FTargetSelection& Out) const;
    bool AcceptTarget(const FDasGASTargetingQuery& Query, AActor* Actor) const;
    FDasAuthoredEffectResult ApplyAuthoredSpecInternal(const FDasAuthoredEffectSpec& Spec, const FHitResult* Hit);
    FGameplayAttribute FindAttribute(FName Set, FName Name) const;
    void Changed(const FOnAttributeChangeData& Data, FName Set);
    void Detach();
    TSet<FName> WatchedCues;
    TSet<FName> AddedCues;
    void ClearAuthoredCues();
    bool MakeAuthoredCueParameters(const FDasGASCueParameters& In, FGameplayCueParameters& Out);
};
