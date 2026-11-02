#pragma once
#include "CoreMinimal.h"
#include "DasCollisionTypes.generated.h"
class FProperty;
class AActor;
struct FDasScalarValue;
// Owned projection for queued collision events; never exposes ActorInstanceHandle internals.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasCollisionHit
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly)
    bool BlockingHit = false;
    UPROPERTY(BlueprintReadOnly)
    bool StartPenetrating = false;
    UPROPERTY(BlueprintReadOnly)
    float Time = 0;
    UPROPERTY(BlueprintReadOnly)
    float Distance = 0;
    UPROPERTY(BlueprintReadOnly)
    float PenetrationDepth = 0;
    UPROPERTY(BlueprintReadOnly)
    FVector Location = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FVector ImpactPoint = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FVector Normal = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FVector ImpactNormal = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FVector TraceStart = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    FVector TraceEnd = FVector::ZeroVector;
    UPROPERTY(BlueprintReadOnly)
    int32 FaceIndex = -1;
    UPROPERTY(BlueprintReadOnly)
    int32 Item = -1;
    UPROPERTY(BlueprintReadOnly)
    int32 MyItem = -1;
    UPROPERTY(BlueprintReadOnly)
    uint8 ElementIndex = 0;
    UPROPERTY(BlueprintReadOnly)
    FName BoneName = NAME_None;
    UPROPERTY(BlueprintReadOnly)
    FName MyBoneName = NAME_None;
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> Actor = nullptr;
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UObject> Component = nullptr;
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UObject> PhysMaterial = nullptr;
};
DASRUNTIME_API bool CanProjectDasCollisionHit(const FProperty* Source, const UScriptStruct* Expected);
DASRUNTIME_API bool ReadDasCollisionHitCallback(const FProperty* Source, const void* Memory,
                                                const UScriptStruct* Expected, FDasScalarValue& Out, FString& Error);
