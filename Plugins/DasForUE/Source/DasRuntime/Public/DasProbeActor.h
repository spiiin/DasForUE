#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DasProbeActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasIntSignal, int32, Value);

// Fixed native adapter for the editor feasibility experiment, not the public DSL API.
UCLASS(Blueprintable)
class DASRUNTIME_API ADasProbeActor : public AActor
{
    GENERATED_BODY()
public:
    ADasProbeActor();
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
private:
    FString ActiveLifecyclePath;
    bool bLifecycleActive = false;
    void RecordLifecycleResult(const struct FDasExecutionResult& Result);

};
