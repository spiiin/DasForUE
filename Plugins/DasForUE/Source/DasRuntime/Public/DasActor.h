#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DasCallFrame.h"
#include "DasActor.generated.h"

class UDasTypeRegistry;

// Native runtime host for authored daScript actors.
UCLASS(Blueprintable)
class DASRUNTIME_API ADasActor : public AActor
{
    GENERATED_BODY()
public:
    ADasActor();
    // Cooked identity for checked authored references; not exposed as a script field.
    UPROPERTY()
    FString AuthoredTypeId;
    // Approved source default fingerprints; actual CDO values may be editor overrides.
    UPROPERTY()
    TMap<FString, FString> DasDefaultSourceContracts;
    // Keep the selected identity catalog and its reflected types as cook/GC dependencies.
    UPROPERTY(EditAnywhere, Category = "daScript")
    TObjectPtr<UDasTypeRegistry> TypeRegistry;
    // Internal persistent retirement storage. Hidden from Details and script API.
    UPROPERTY(SaveGame)
    TMap<FString, FDasScalarValue> RetiredDasValues;

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    // Script entry points are captured when the actor begins play.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "daScript")
    FString LifecycleScript;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "daScript")
    FString LastDasError;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void Destroyed() override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostInitializeComponents() override;
    UPROPERTY()
    bool bDasConstructionScript = false;
    UPROPERTY()
    FName DasSceneRootProperty;

    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    int32 ExecuteDas(const FString& ScriptFile, int32 Input);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    int32 ExecuteDasNamed(const FString& ScriptFile, const FString& EntryPoint, int32 Input);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    float ExecuteDasFloat(const FString& ScriptFile, const FString& EntryPoint, float Input);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    bool ExecuteDasBool(const FString& ScriptFile, const FString& EntryPoint, bool Input);
    UFUNCTION(BlueprintCallable, Category = "daScript Internal", meta = (BlueprintInternalUseOnly = "true"))
    void ExecuteDasVoid(const FString& ScriptFile, const FString& EntryPoint);
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
    friend class FDasRuntimeModule;
    friend struct FDasActorHostRuntime;
    FString ActiveLifecyclePath;
    bool bLifecycleActive = false;
    void ApplyDasSceneRoot();
    void RecordLifecycleResult(const struct FDasExecutionResult& Result);
};
