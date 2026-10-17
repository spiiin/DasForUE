#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DasNativeHashProbeComponent.generated.h"

// Reflection fixtures for the generated native ABI, not an authored gameplay host.
UENUM(BlueprintType)
enum class EDasNativeHashKey:uint8 { First, Second };
USTRUCT(BlueprintType)
struct FDasNativeHashValue {
 GENERATED_BODY()
 UPROPERTY() TMap<int32,FString> Labels;
 UPROPERTY() TSet<FString> Names;
};
UCLASS()
class DASRUNTIME_API UDasNativeHashProbeComponent:public UActorComponent {
 GENERATED_BODY()
public:
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TMap<FString,int32> Counts;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TMap<FName,FVector> Positions;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TMap<EDasNativeHashKey,double> Weights;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TMap<AActor*,FString> ActorLabels;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TSet<int32> Numbers;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TSet<FName> Tags;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TSet<EDasNativeHashKey> Modes;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") TSet<AActor*> Actors;
 UPROPERTY(BlueprintReadOnly,Category="Das|NativeHashProbe") TSet<FString> ReadOnlyNames;
 UPROPERTY(BlueprintReadWrite,Category="Das|NativeHashProbe") FDasNativeHashValue Nested;
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") TMap<FString,int32> EchoCounts(const TMap<FString,int32>& Value)const{return Value;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") void UpdateCounts(UPARAM(ref) TMap<FString,int32>& Value)const{Value.FindOrAdd(TEXT("native"))+=7;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") void ReadCounts(TMap<FString,int32>& Value)const{Value=Counts;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") TSet<AActor*> EchoActors(const TSet<AActor*>& Value)const{return Value;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") TMap<FName,FVector> EchoPositions(const TMap<FName,FVector>& Value)const{return Value;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") TMap<EDasNativeHashKey,double> EchoWeights(const TMap<EDasNativeHashKey,double>& Value)const{return Value;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") TMap<AActor*,FString> EchoActorLabels(const TMap<AActor*,FString>& Value)const{return Value;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") FDasNativeHashValue EchoNested(const FDasNativeHashValue& Value)const{return Value;}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") void UpdateNumbers(UPARAM(ref) TSet<int32>& Value)const{Value.Add(42);Value.Remove(-1);}
 UFUNCTION(BlueprintCallable,Category="Das|NativeHashProbe") void ReadModes(TSet<EDasNativeHashKey>& Value)const{Value=Modes;}
};
