#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DasCallFrame.h"
#include "DasDataAssets.generated.h"
struct FDasExecutionResult;
DASRUNTIME_API bool ParseDasAssetBundleNames(const FString& Text, TArray<FName>& Out, FString& Error);

UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasDataAsset : public UDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString AuthoredTypeId;
    UPROPERTY()
    TMap<FString, FString> DasDefaultSourceContracts;
    // Values of removed fields, kept for a restore (see the archive field migration).
    UPROPERTY(SaveGame)
    TMap<FString, FDasScalarValue> RetiredDasValues;
    UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "daScript")
    FString LastDasError;
    void RecordLifecycleResult(const FDasExecutionResult& Result);
    virtual void BeginDestroy() override;
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasFrame(const FString& ScriptFile, const FString& EntryPoint,
                                    const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                    const FString& ResultClass);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasFrameOutputs(const FString& ScriptFile, const FString& EntryPoint,
                                           const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                           const FString& ResultClass, TArray<FDasScalarValue>& Outputs);
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "daScript Internal",
              meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasConstFrame(const FString& ScriptFile, const FString& EntryPoint,
                                         const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                         const FString& ResultClass) const;
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "daScript Internal",
              meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasConstFrameOutputs(const FString& ScriptFile, const FString& EntryPoint,
                                                const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                                const FString& ResultClass, TArray<FDasScalarValue>& Outputs) const;
};

UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasPrimaryDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FName DasPrimaryAssetType;
    virtual FPrimaryAssetId GetPrimaryAssetId() const override;
    UPROPERTY()
    FString AuthoredTypeId;
    UPROPERTY()
    TMap<FString, FString> DasDefaultSourceContracts;
    // Values of removed fields, kept for a restore (see the archive field migration).
    UPROPERTY(SaveGame)
    TMap<FString, FDasScalarValue> RetiredDasValues;
    UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "daScript")
    FString LastDasError;
    void RecordLifecycleResult(const FDasExecutionResult& Result);
    virtual void BeginDestroy() override;
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasFrame(const FString& ScriptFile, const FString& EntryPoint,
                                    const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                    const FString& ResultClass);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasFrameOutputs(const FString& ScriptFile, const FString& EntryPoint,
                                           const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                           const FString& ResultClass, TArray<FDasScalarValue>& Outputs);
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "daScript Internal",
              meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasConstFrame(const FString& ScriptFile, const FString& EntryPoint,
                                         const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                         const FString& ResultClass) const;
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "daScript Internal",
              meta = (BlueprintInternalUseOnly = "true"))
    FDasScalarValue ExecuteDasConstFrameOutputs(const FString& ScriptFile, const FString& EntryPoint,
                                                const TArray<FDasScalarValue>& Arguments, int32 ResultKind,
                                                const FString& ResultClass, TArray<FDasScalarValue>& Outputs) const;
};
