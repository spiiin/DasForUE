#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DasCallFrame.h"
#include "DasScriptComponent.generated.h"
struct FDasExecutionResult;
class UDasScriptComponent;
DECLARE_MULTICAST_DELEGATE_TwoParams(FDasComponentScriptEnded,UDasScriptComponent*,EEndPlayReason::Type);
// Native lifecycle bridge: each component owns an independent interpreter session.
UCLASS(ClassGroup=(daScript),BlueprintType,Blueprintable,meta=(BlueprintSpawnableComponent))
class DASRUNTIME_API UDasScriptComponent:public UActorComponent {
 GENERATED_BODY()
public:
    // Cooked identity for checked authored references; not exposed as a script field.
    UPROPERTY() FString AuthoredTypeId;
 UDasScriptComponent();
 FDasComponentScriptEnded OnScriptEnded;
 UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="daScript") FString LifecycleScript;
 UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Transient,Category="daScript") FString LastDasError;
 // Fields used by the executable Actor + Component example.
 UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Example") int32 Counter=0;
 UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,Category="Example") int32 BeginCalls=0;
 UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,Category="Example") int32 TickCalls=0;
 UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,Category="Example") int32 EndCalls=0;
 UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,Category="Example") float Elapsed=0;
 virtual void BeginPlay() override;
 virtual void TickComponent(float DeltaTime,ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
 virtual void EndPlay(const EEndPlayReason::Type Reason) override;
 virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;
    UFUNCTION(BlueprintCallable,Category="daScript",meta=(BlueprintInternalUseOnly="true"))
    FDasScalarValue ExecuteDasFrame(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass);
    UFUNCTION(BlueprintCallable,Category="daScript",meta=(BlueprintInternalUseOnly="true"))
    FDasScalarValue ExecuteDasFrameOutputs(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs);
private:
 FString ActivePath;bool bLifecycleActive=false;
 void FinishScript(EEndPlayReason::Type Reason);
 void Record(const FDasExecutionResult& Result);
};
