#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DasCallFrame.h"
#include "DasSaveGame.generated.h"
struct FDasExecutionResult;

UCLASS(BlueprintType,Blueprintable)
class DASRUNTIME_API UDasSaveGame : public USaveGame
{
 GENERATED_BODY()
public:
 UPROPERTY() FString AuthoredTypeId;
 UPROPERTY() TMap<FString,FString> DasDefaultSourceContracts;
 UPROPERTY(Transient,VisibleInstanceOnly,BlueprintReadOnly,Category="daScript") FString LastDasError;
 void RecordLifecycleResult(const FDasExecutionResult& Result);
 virtual void BeginDestroy() override;
 UFUNCTION(BlueprintCallable,Category="daScript Internal",meta=(BlueprintInternalUseOnly="true"))
 FDasScalarValue ExecuteDasFrame(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass);
 UFUNCTION(BlueprintCallable,Category="daScript Internal",meta=(BlueprintInternalUseOnly="true"))
 FDasScalarValue ExecuteDasFrameOutputs(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs);
 UFUNCTION(BlueprintCallable,BlueprintPure=false,Category="daScript Internal",meta=(BlueprintInternalUseOnly="true"))
 FDasScalarValue ExecuteDasConstFrame(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass) const;
 UFUNCTION(BlueprintCallable,BlueprintPure=false,Category="daScript Internal",meta=(BlueprintInternalUseOnly="true"))
 FDasScalarValue ExecuteDasConstFrameOutputs(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs) const;
};
