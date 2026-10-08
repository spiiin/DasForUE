#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DasProbeInterface.generated.h"
UINTERFACE(BlueprintType,Blueprintable)
class DASRUNTIME_API UDasProbeInterface : public UInterface {GENERATED_BODY()};
class DASRUNTIME_API IDasProbeInterface {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") int32 Query(int32 Input);
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") FString Label();
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") TArray<double> Weights(const TArray<double>& Values);
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") void Stats(UPARAM(ref) int32& Counter,int32& Total);
};

UINTERFACE(BlueprintType,Blueprintable)
class DASRUNTIME_API UDasCollectionInterface : public UInterface {GENERATED_BODY()};
class DASRUNTIME_API IDasCollectionInterface {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") TMap<int32,FString> EchoMap(const TMap<int32,FString>& Values);
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") TSet<int32> EchoSet(const TSet<int32>& Values);
};

UINTERFACE(BlueprintType,Blueprintable)
class DASRUNTIME_API UDasEventInterface : public UInterface {GENERATED_BODY()};
class DASRUNTIME_API IDasEventInterface {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") void Notify(int32 Amount);
 UFUNCTION(BlueprintCallable,BlueprintNativeEvent,Category="daScript Probe") void ResetCounter();
};
