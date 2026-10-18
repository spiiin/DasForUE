#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/HitResult.h"
#include "DasHitTransportLibrary.generated.h"

USTRUCT(BlueprintType)
struct FDasHitTransportEnvelope {
 GENERATED_BODY()
 UPROPERTY() FHitResult Hit;
 UPROPERTY() TArray<FHitResult> Hits;
};

USTRUCT(BlueprintType)
struct FDasReferenceArrayLeaf {
 GENERATED_BODY()
 UPROPERTY() TArray<AActor*> Actors;
 UPROPERTY() TArray<TSubclassOf<AActor>> Classes;
};
USTRUCT(BlueprintType)
struct FDasReferenceArrayEnvelope {
 GENERATED_BODY()
 UPROPERTY() FDasReferenceArrayLeaf Inner;
 UPROPERTY() TArray<FDasReferenceArrayLeaf> Entries;
};

// Deliberate fixture: exercises the same reflected backend as real Engine libraries.
UCLASS()
class DASRUNTIME_API UDasHitTransportLibrary:public UBlueprintFunctionLibrary {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable) static FHitResult Echo(const FHitResult& Hit){return Hit;}
 UFUNCTION(BlueprintCallable) static TArray<FHitResult> EchoMany(const TArray<FHitResult>& Hits){return Hits;}
 UFUNCTION(BlueprintCallable) static FDasHitTransportEnvelope EchoEnvelope(const FDasHitTransportEnvelope& Value){return Value;}
 UFUNCTION(BlueprintCallable) static void ChangeItem(UPARAM(ref) FHitResult& Hit){Hit.Item+=7;}
 UFUNCTION(BlueprintCallable) static AActor* CachedActor(const FHitResult& Hit){return Hit.HitObjectHandle.GetCachedActor();}
 UFUNCTION(BlueprintCallable) static FHitResult MakeWeakHit();
 UFUNCTION(BlueprintCallable) static void CollectWeak();
 UFUNCTION(BlueprintCallable) static FVector EchoVector(FVector Value){return Value;}
 UFUNCTION(BlueprintCallable) static FDasReferenceArrayEnvelope EchoReferences(const FDasReferenceArrayEnvelope& Value){return Value;}
};
