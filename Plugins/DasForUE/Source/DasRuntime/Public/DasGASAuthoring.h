#pragma once
#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GameplayEffect.h"
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
};
struct FDasGASEffectDeclaration
{
    FString Name, Id, Attributes;
    TArray<FDasGASModifierDeclaration> Modifiers;
};
struct FDasGASDeclarations
{
    TArray<FDasGASSetDeclaration> Sets;
    TArray<FDasGASEffectDeclaration> Effects;
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
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasAuthoredAttributeChanged, FDasAuthoredAttributeChange, Change);
UCLASS(ClassGroup = (Das), meta = (BlueprintSpawnableComponent))
class DASRUNTIME_API UDasAuthoredAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()
public:
    UDasAuthoredAbilitySystemComponent();
    UPROPERTY(BlueprintAssignable, Category = "GAS")
    FDasAuthoredAttributeChanged AttributeChanged;
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
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void BeginDestroy() override;

private:
    UPROPERTY(Transient)
    TMap<FName, TObjectPtr<UDasAuthoredAttributeSet>> Sets;
    UPROPERTY(Transient)
    TArray<TObjectPtr<UDasAuthoredGameplayEffect>> Definitions;
    TMap<FGameplayAttribute, FDelegateHandle> ChangeHandles;
    bool Ended = false;
    bool Live() const;
    FGameplayAttribute FindAttribute(FName Set, FName Name) const;
    void Changed(const FOnAttributeChangeData& Data, FName Set);
    void Detach();
};
