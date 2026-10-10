#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DasCallFrame.h"
#include "DasProbeActor.generated.h"

class UDasTypeRegistry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasIntSignal, int32, Value);

// Fixed native adapter for the editor feasibility experiment, not the public DSL API.
UCLASS(Blueprintable)
class DASRUNTIME_API ADasProbeActor : public AActor
{
    GENERATED_BODY()
public:
    // Cooked identity for checked authored references; not exposed as a script field.
    UPROPERTY() FString AuthoredTypeId;
    ADasProbeActor();
    // Keep the selected identity catalog and its reflected types as cook/GC dependencies.
    UPROPERTY(EditAnywhere, Category="daScript") TObjectPtr<UDasTypeRegistry> TypeRegistry;
    // Internal persistent retirement storage. Hidden from Details and script API.
    UPROPERTY(SaveGame) TMap<FString,FDasScalarValue> RetiredDasValues;

    UPROPERTY(BlueprintAssignable, Category="daScript")
    FDasIntSignal OnDasSignal;
    UFUNCTION(BlueprintCallable, Category="daScript")
    void EmitDasSignal(int32 Value) { OnDasSignal.Broadcast(Value); }
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    // Fixed callback contract until the user-facing DSL is selected. Captured at BeginPlay.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="daScript")
    FString LifecycleScript;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category="daScript")
    FString LastDasError;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void Destroyed() override;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="daScript Probe")
    TObjectPtr<AActor> Peer;

    UFUNCTION(BlueprintCallable, Category="daScript Probe", meta=(BlueprintInternalUseOnly="true"))
    int32 ExecuteDas(const FString& ScriptFile, int32 Input);
    UFUNCTION(BlueprintCallable, Category="daScript Probe", meta=(BlueprintInternalUseOnly="true"))
    int32 ExecuteDasNamed(const FString& ScriptFile, const FString& EntryPoint, int32 Input);
    UFUNCTION(BlueprintCallable,Category="daScript Probe",meta=(BlueprintInternalUseOnly="true"))
    float ExecuteDasFloat(const FString& ScriptFile,const FString& EntryPoint,float Input);
    UFUNCTION(BlueprintCallable,Category="daScript Probe",meta=(BlueprintInternalUseOnly="true"))
    bool ExecuteDasBool(const FString& ScriptFile,const FString& EntryPoint,bool Input);
    UFUNCTION(BlueprintCallable,Category="daScript Probe",meta=(BlueprintInternalUseOnly="true"))
    void ExecuteDasVoid(const FString& ScriptFile,const FString& EntryPoint);
    UFUNCTION(BlueprintCallable,Category="daScript Probe",meta=(BlueprintInternalUseOnly="true"))
    FDasScalarValue ExecuteDasFrame(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass);
    UFUNCTION(BlueprintCallable,Category="daScript Probe",meta=(BlueprintInternalUseOnly="true"))
    FDasScalarValue ExecuteDasFrameOutputs(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs);
private:
    friend class FDasRuntimeModule;
    FString ActiveLifecyclePath;
    bool bLifecycleActive = false;
    void RecordLifecycleResult(const struct FDasExecutionResult& Result);

};
