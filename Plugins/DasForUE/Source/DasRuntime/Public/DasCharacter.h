#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DasCallFrame.h"
#include "DasCharacter.generated.h"
class UDasTypeRegistry;
UCLASS(Blueprintable)
class DASRUNTIME_API ADasCharacter : public ACharacter
{
    GENERATED_BODY()
public:
    ADasCharacter();
    UPROPERTY()
    FString AuthoredTypeId;
    UPROPERTY()
    TMap<FString, FString> DasDefaultSourceContracts;
    UPROPERTY(EditAnywhere, Category = "daScript")
    TObjectPtr<UDasTypeRegistry> TypeRegistry;
    UPROPERTY(SaveGame)
    TMap<FString, FDasScalarValue> RetiredDasValues;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "daScript")
    FString LifecycleScript;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "daScript")
    FString LastDasError;
    UPROPERTY()
    bool bDasConstructionScript = false;
    UPROPERTY()
    FName DasSceneRootProperty;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void Destroyed() override;
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

private:
    friend struct FDasActorHostRuntime;
    friend class FDasRuntimeModule;
    FString ActiveLifecyclePath;
    bool bLifecycleActive = false;
    void ApplyDasSceneRoot();
    void RecordLifecycleResult(const struct FDasExecutionResult& Result);
};
