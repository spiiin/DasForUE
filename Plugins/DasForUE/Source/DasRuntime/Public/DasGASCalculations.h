#pragma once
#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "GameplayEffectExecutionCalculation.h"
#include "DasGASAdvancedTypes.h"
#include "DasGASCalculations.generated.h"
USTRUCT()
struct DASRUNTIME_API FDasGASCapture
{
    GENERATED_BODY()
    UPROPERTY()
    FName Name;
    UPROPERTY()
    FGameplayEffectAttributeCaptureDefinition Definition;
};
USTRUCT()
struct DASRUNTIME_API FDasGASCalculationDefinition
{
    GENERATED_BODY()
    UPROPERTY()
    FString Id;
    UPROPERTY()
    FString Hash;
    UPROPERTY()
    FString Script;
    UPROPERTY()
    FString Contract;
    UPROPERTY()
    FString Entry;
    UPROPERTY()
    TArray<FDasGASCapture> Captures;
    UPROPERTY()
    FGameplayAttribute Output;
    UPROPERTY()
    TEnumAsByte<EGameplayModOp::Type> Operation = EGameplayModOp::Additive;
};
UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasGASMagnitudeCalculation : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FDasGASCalculationDefinition Definition;
    void ConfigureCaptures();
    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
    virtual void BeginDestroy() override;
    // Makes the detached calculation context discoverable by the native generator.
    UFUNCTION(BlueprintPure, Category = "GAS")
    FDasGASCalculationContext EmptyCalculationContext() const { return {}; }
};
UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasGASExecutionCalculation : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FDasGASCalculationDefinition Definition;
    void ConfigureCaptures();
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& Params,
                                        FGameplayEffectCustomExecutionOutput& Out) const override;
    virtual void BeginDestroy() override;
};
