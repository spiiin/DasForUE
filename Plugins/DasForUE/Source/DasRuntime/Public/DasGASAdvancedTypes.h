#pragma once
#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "GameplayTagContainer.h"
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
// Local, explicit targeting. Radius=0 is a line trace; positive radius is a sphere sweep/overlap.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASTargetingQuery
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    FName Mode = TEXT("Trace");
    UPROPERTY(BlueprintReadWrite)
    FVector Start = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector End = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    float Radius = 0;
    UPROPERTY(BlueprintReadWrite)
    int32 Channel = 3; // ECC_Visibility; override for the project's query channel.
    UPROPERTY(BlueprintReadWrite)
    bool TraceComplex = false;
    UPROPERTY(BlueprintReadWrite)
    bool IgnoreSelf = true;
    UPROPERTY(BlueprintReadWrite)
    bool RequireAbilitySystem = true;
    UPROPERTY(BlueprintReadWrite)
    int32 MaxTargets = 16;
    UPROPERTY(BlueprintReadWrite)
    FString ActorClassPath; // Filled from a loaded typed class by ue_gas_target_class.
    UPROPERTY(BlueprintReadWrite)
    TArray<TObjectPtr<AActor>> IgnoredActors;
    // Hierarchical ASC-owned GameplayTags: all required, none blocked.
    UPROPERTY(BlueprintReadWrite)
    TArray<FName> RequiredTags;
    UPROPERTY(BlueprintReadWrite)
    TArray<FName> BlockedTags;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASTargetingPreview
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool Valid = false;
    UPROPERTY(BlueprintReadWrite)
    TArray<TObjectPtr<AActor>> Actors;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASTargetingResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool Confirmed = false;
    UPROPERTY(BlueprintReadWrite)
    FDasGASTargetData Targets;
};
class UPrimitiveComponent;
class UPhysicalMaterial;
// Reflected, owned view of HitResult; native instance/physics internals never enter DAS.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASCueHit
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool BlockingHit = false;
    UPROPERTY(BlueprintReadWrite)
    bool StartPenetrating = false;
    UPROPERTY(BlueprintReadWrite)
    float Time = 0;
    UPROPERTY(BlueprintReadWrite)
    float Distance = 0;
    UPROPERTY(BlueprintReadWrite)
    float PenetrationDepth = 0;
    UPROPERTY(BlueprintReadWrite)
    int32 FaceIndex = -1;
    UPROPERTY(BlueprintReadWrite)
    int32 Item = -1;
    UPROPERTY(BlueprintReadWrite)
    int32 MyItem = -1;
    UPROPERTY(BlueprintReadWrite)
    int32 ElementIndex = 0;
    UPROPERTY(BlueprintReadWrite)
    FName BoneName;
    UPROPERTY(BlueprintReadWrite)
    FName MyBoneName;
    UPROPERTY(BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector ImpactPoint = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector Normal = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector ImpactNormal = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector TraceStart = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector TraceEnd = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    TWeakObjectPtr<AActor> Actor;
    UPROPERTY(BlueprintReadWrite)
    TWeakObjectPtr<UPrimitiveComponent> Component;
    UPROPERTY(BlueprintReadWrite)
    TWeakObjectPtr<UPhysicalMaterial> PhysMaterial;
    FDasGASCueHit() = default;
    FDasGASCueHit(const FHitResult& Hit);
    FHitResult ToNative() const;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASCueEffectContext
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    bool Valid = false;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Instigator = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> OriginalInstigator = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> EffectCauser = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UObject> SourceObject = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TArray<TObjectPtr<AActor>> Actors;
    UPROPERTY(BlueprintReadWrite)
    bool HasOrigin = false;
    UPROPERTY(BlueprintReadWrite)
    FVector Origin = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    bool HasHit = false;
    UPROPERTY(BlueprintReadWrite)
    FDasGASCueHit Hit;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASCueParameters
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    float RawMagnitude = 0;
    UPROPERTY(BlueprintReadWrite)
    float NormalizedMagnitude = 0;
    UPROPERTY(BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    FVector Normal = FVector::ZeroVector;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Instigator = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> EffectCauser = nullptr;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UObject> SourceObject = nullptr;
    UPROPERTY(BlueprintReadWrite)
    int32 AbilityLevel = 1;
    UPROPERTY(BlueprintReadWrite)
    int32 EffectLevel = 1;
    UPROPERTY(BlueprintReadWrite)
    FGameplayTagContainer SourceTags;
    UPROPERTY(BlueprintReadWrite)
    FGameplayTagContainer TargetTags;
    UPROPERTY(BlueprintReadWrite)
    FDasGASCueEffectContext Context;
};
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASCueEvent
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    FName Tag;
    UPROPERTY(BlueprintReadWrite)
    FName MatchedTag;
    UPROPERTY(BlueprintReadWrite)
    FName Event;
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<AActor> Target = nullptr;
    UPROPERTY(BlueprintReadWrite)
    FDasGASCueParameters Parameters;
};
// Keys are declared execution output aliases; attributes and operations remain native schema.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasGASExecutionResult
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadWrite)
    TMap<FName, float> Outputs;
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
