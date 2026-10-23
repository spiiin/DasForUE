#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DasCallFrame.h"
#include "DasSubsystems.generated.h"
struct FDasExecutionResult;
DASRUNTIME_API void LoadDasSubsystemClasses();
DASRUNTIME_API void ResetDasSubsystemClasses();

UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasWorldSubsystem : public UWorldSubsystem
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
    UPROPERTY(EditDefaultsOnly, Category = "daScript")
    FString LifecycleScript;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void BeginDestroy() override;
    bool IsDasActive() const { return bDasActive; }

private:
    bool bDasActive = false;

public:
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
class DASRUNTIME_API UDasGameInstanceSubsystem : public UGameInstanceSubsystem
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
    UPROPERTY(EditDefaultsOnly, Category = "daScript")
    FString LifecycleScript;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void BeginDestroy() override;
    bool IsDasActive() const { return bDasActive; }

private:
    bool bDasActive = false;

public:
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
