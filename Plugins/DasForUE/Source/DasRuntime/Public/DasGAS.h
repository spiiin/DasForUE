#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
#include "GameplayTagContainer.h"
#include "DasGAS.generated.h"

class UDasHealthAbilitySystemComponent;

UENUM(BlueprintType)
enum class EDasHealthAttribute : uint8
{
    Health,
    MaxHealth
};
UENUM(BlueprintType)
enum class EDasHealthEffect : uint8
{
    Heal,
    Damage
};

// Detached payload; never stores FOnAttributeChangeData::GEModData.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGameplayAttributeChange
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    EDasHealthAttribute Attribute = EDasHealthAttribute::Health;
    UPROPERTY(BlueprintReadWrite)
    float OldValue = 0;
    UPROPERTY(BlueprintReadWrite)
    float NewValue = 0;
};

// Owned recipe for the two starter instant effects. Materialized as a real GAS
// spec on application; no source/target attribute captures are supported yet.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasHealthEffectSpec
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasHealthAbilitySystemComponent> Source = nullptr;
    UPROPERTY(BlueprintReadWrite)
    EDasHealthEffect Effect = EDasHealthEffect::Heal;
    UPROPERTY(BlueprintReadWrite)
    float Level = 1;
    UPROPERTY(BlueprintReadWrite)
    FName MagnitudeTag;
    UPROPERTY(BlueprintReadWrite)
    float Magnitude = 0;
    UPROPERTY(BlueprintReadWrite)
    bool HasMagnitude = false;
};

USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasHealthEffectResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool Valid = false;
    UPROPERTY(BlueprintReadWrite)
    bool Applied = false;
    UPROPERTY(BlueprintReadWrite)
    bool HasActiveHandle = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasAttributeChanged, FDasGameplayAttributeChange, Change);

UCLASS()
class DASRUNTIME_API UDasHealthAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category = "GAS")
    FGameplayAttributeData Health;
    UPROPERTY(BlueprintReadOnly, Category = "GAS")
    FGameplayAttributeData MaxHealth;
    static FGameplayAttribute HealthAttribute();
    static FGameplayAttribute MaxHealthAttribute();
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
    void ClampValue(const FGameplayAttribute& Attribute, float& Value) const;
};

UCLASS()
class DASRUNTIME_API UDasHealGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UDasHealGameplayEffect();
};

UCLASS()
class DASRUNTIME_API UDasDamageGameplayEffect : public UGameplayEffect
{
    GENERATED_BODY()
public:
    UDasDamageGameplayEffect();
};

// Native adapter exposes the non-reflected GAS delegate to the existing queued
// ue_event transport. Gameplay state remains in the real ASC / AttributeSet.
UCLASS(ClassGroup = (Das), meta = (BlueprintSpawnableComponent))
class DASRUNTIME_API UDasHealthAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()
public:
    UDasHealthAbilitySystemComponent();
    UPROPERTY(BlueprintAssignable, Category = "GAS")
    FDasAttributeChanged AttributeChanged;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    bool InitializeHealth(float Maximum);
    UFUNCTION(BlueprintPure, Category = "GAS")
    bool HealthReady() const;
    UFUNCTION(BlueprintPure, Category = "GAS")
    float ReadHealthAttribute(EDasHealthAttribute Attribute) const;
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasGameplayAttributeChange HealthSnapshot(EDasHealthAttribute Attribute) const;
    UFUNCTION(BlueprintCallable, Category = "GAS")
    FDasHealthEffectSpec MakeHealthSpec(EDasHealthEffect Effect, float Level);
    UFUNCTION(BlueprintCallable, Category = "GAS")
    FDasHealthEffectResult ApplyHealthSpec(const FDasHealthEffectSpec& Spec);
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void BeginDestroy() override;

private:
    UPROPERTY(Transient)
    TObjectPtr<UDasHealthAttributeSet> HealthSet;
    FDelegateHandle HealthChangedHandle, MaximumChangedHandle;
    bool HealthActive = false;
    void DetachHealthDelegates();
    void ForwardChange(const FOnAttributeChangeData& Data, EDasHealthAttribute Attribute);
    UFUNCTION()
    void OnOwnerDamage(AActor* Damaged, float Amount, const UDamageType* Type, AController* Instigator, AActor* Causer);
};
