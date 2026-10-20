#pragma once
#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "DasGASAdvancedTypes.generated.h"
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASAbilityTaskResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    int32 Id = 0;
    UPROPERTY(BlueprintReadWrite)
    FName Kind;
    UPROPERTY(BlueprintReadWrite)
    FName Phase;
    UPROPERTY(BlueprintReadWrite)
    FName EventTag;
    UPROPERTY(BlueprintReadWrite)
    float Magnitude = 0;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Instigator = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Target = nullptr;
};
class UDasAuthoredAbilitySystemComponent;
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASTargetData
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    TArray<TObjectPtr<AActor>> Actors;
    UPROPERTY(BlueprintReadWrite)
    TArray<int32> Hits;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UDasAuthoredAbilitySystemComponent> Source = nullptr;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASCalculationContext
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    float Level = 1;
    UPROPERTY(BlueprintReadWrite)
    TMap<FName, float> Values;
    UPROPERTY(BlueprintReadWrite)
    TMap<FName, float> SetByCaller;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Source = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Target = nullptr;
};
