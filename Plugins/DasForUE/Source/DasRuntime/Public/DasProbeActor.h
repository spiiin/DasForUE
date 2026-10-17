#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DasCallFrame.h"
#include "DasProbeActor.generated.h"

class UDasTypeRegistry;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasIntSignal, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasFloatSignal,float,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasBoolSignal,bool,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasStringSignal,const FString&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDasVoidSignal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVector2DSignal,FVector2D,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVector2DRefSignal,const FVector2D&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasNameSignal,FName,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayIntSignal,const TArray<int32>&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayDoubleSignal,const TArray<double>&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayStringSignal,const TArray<FString>&,Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasDoubleSignal,double,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasObjectSignal,UObject*,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVectorSignal,FVector,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVectorRefSignal,const FVector&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasRotatorSignal,FRotator,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasRotatorRefSignal,const FRotator&,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasTransformSignal,FTransform,Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasTransformRefSignal,const FTransform&,Value);


// Fixed native adapter for the editor feasibility experiment, not the public DSL API.
UCLASS(Blueprintable)
class DASRUNTIME_API ADasProbeActor : public AActor
{
    GENERATED_BODY()
public:
    // Cooked identity for checked authored references; not exposed as a script field.
    UPROPERTY() FString AuthoredTypeId;
    // Approved source default fingerprints; actual CDO values may be editor overrides.
    UPROPERTY() TMap<FString,FString> DasDefaultSourceContracts;
    // Native storage used only by the scoped array regression fixture.
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<ESpawnActorCollisionHandlingMethod> DasNativeArrayEnums;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<TEnumAsByte<EComponentMobility::Type>> DasNativeArrayBytes;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<FVector> DasNativeArrayVectors;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<FRotator> DasNativeArrayRotators;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<FTransform> DasNativeArrayTransforms;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<FIntPoint> DasNativeArrayPoints;
    UPROPERTY(BlueprintReadOnly,Category="DasForUE|Tests") TArray<FVector> DasNativeArrayReadonly;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<TObjectPtr<AActor>> DasNativeArrayObjects;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TArray<TObjectPtr<APawn>> DasNativeArrayPawns;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<int32,int32> DasNativeArrayIntMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FString,double> DasNativeArrayDoubleMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<int32,FName> DasNativeArrayNameMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FString,FVector> DasNativeArrayVectorMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<int32,FIntPoint> DasNativeArrayPointMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<int32,ESpawnActorCollisionHandlingMethod> DasNativeArrayEnumMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FString,TEnumAsByte<EComponentMobility::Type>> DasNativeArrayByteMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<int32,TObjectPtr<APawn>> DasNativeArrayPawnMap;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TSet<int32> DasNativeArrayIntSet;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TSet<FString> DasNativeArrayStringSet;
    UPROPERTY(BlueprintReadOnly,Category="DasForUE|Tests") TMap<int32,int32> DasNativeArrayReadonlyMap;
    UPROPERTY(BlueprintReadOnly,Category="DasForUE|Tests") TSet<FString> DasNativeArrayReadonlySet;
    UPROPERTY(BlueprintReadOnly,Category="DasForUE|Tests") TArray<TObjectPtr<APawn>> DasNativeArrayReadonlyObjects;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TSet<FName> DasNativeArrayNameSet;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,int32> DasNativeArrayNameKeys;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,FName> DasNativeArrayNameNames;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,FVector> DasNativeArrayNameVectors;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,FIntPoint> DasNativeArrayNamePoints;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,ESpawnActorCollisionHandlingMethod> DasNativeArrayNameEnums;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,TEnumAsByte<EComponentMobility::Type>> DasNativeArrayNameBytes;
    UPROPERTY(BlueprintReadWrite,Category="DasForUE|Tests") TMap<FName,TObjectPtr<APawn>> DasNativeArrayNameObjects;
    UPROPERTY(BlueprintReadOnly,Category="DasForUE|Tests") TSet<FName> DasNativeArrayReadonlyNameSet;
    UPROPERTY(BlueprintReadOnly,Category="DasForUE|Tests") TMap<FName,int32> DasNativeArrayReadonlyNameMap;
    ADasProbeActor();
    // Keep the selected identity catalog and its reflected types as cook/GC dependencies.
    UPROPERTY(EditAnywhere, Category="daScript") TObjectPtr<UDasTypeRegistry> TypeRegistry;
    // Internal persistent retirement storage. Hidden from Details and script API.
    UPROPERTY(SaveGame) TMap<FString,FDasScalarValue> RetiredDasValues;

    UPROPERTY(BlueprintAssignable, Category="daScript")
    FDasIntSignal OnDasSignal;
    UFUNCTION(BlueprintCallable, Category="daScript")
    void EmitDasSignal(int32 Value) { OnDasSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasFloatSignal OnDasFloatSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasFloatSignal(float Value){OnDasFloatSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasBoolSignal OnDasBoolSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasBoolSignal(bool Value){OnDasBoolSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasStringSignal OnDasStringSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasStringSignal(const FString& Value){OnDasStringSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasDoubleSignal OnDasDoubleSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasDoubleSignal(double Value){OnDasDoubleSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasObjectSignal OnDasObjectSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasObjectSignal(UObject* Value){OnDasObjectSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasVectorRefSignal OnDasVectorRefSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasVectorRefSignal(const FVector& Value){OnDasVectorRefSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasRotatorSignal OnDasRotatorSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasRotatorSignal(FRotator Value){OnDasRotatorSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasRotatorRefSignal OnDasRotatorRefSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasRotatorRefSignal(const FRotator& Value){OnDasRotatorRefSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasTransformSignal OnDasTransformSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasTransformSignal(FTransform Value){OnDasTransformSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasTransformRefSignal OnDasTransformRefSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasTransformRefSignal(const FTransform& Value){OnDasTransformRefSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasVectorSignal OnDasVectorSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasVectorSignal(FVector Value){OnDasVectorSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasNameSignal OnDasNameSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasNameSignal(FName Value){OnDasNameSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasArrayIntSignal OnDasArrayIntSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasArrayIntSignal(const TArray<int32>& Value){OnDasArrayIntSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasArrayDoubleSignal OnDasArrayDoubleSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasArrayDoubleSignal(const TArray<double>& Value){OnDasArrayDoubleSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasArrayStringSignal OnDasArrayStringSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasArrayStringSignal(const TArray<FString>& Value){OnDasArrayStringSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasVector2DSignal OnDasVector2DSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasVector2DSignal(FVector2D Value){OnDasVector2DSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasVector2DRefSignal OnDasVector2DRefSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasVector2DRefSignal(const FVector2D& Value){OnDasVector2DRefSignal.Broadcast(Value);}
    UPROPERTY(BlueprintAssignable,Category="DasForUE|Signals") FDasVoidSignal OnDasVoidSignal;
    UFUNCTION(BlueprintCallable,Category="DasForUE|Signals") void EmitDasVoidSignal(){OnDasVoidSignal.Broadcast();}

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    // Fixed callback contract until the user-facing DSL is selected. Captured at BeginPlay.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="daScript")
    FString LifecycleScript;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category="daScript")
    FString LastDasError;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    virtual void Destroyed() override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void PostInitializeComponents() override;
    UPROPERTY() bool bDasConstructionScript = false;
    UPROPERTY() FName DasSceneRootProperty;
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
    UFUNCTION(BlueprintCallable,BlueprintPure=false,Category="daScript Internal",meta=(BlueprintInternalUseOnly="true"))
    FDasScalarValue ExecuteDasConstFrame(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass) const;
    UFUNCTION(BlueprintCallable,BlueprintPure=false,Category="daScript Internal",meta=(BlueprintInternalUseOnly="true"))
    FDasScalarValue ExecuteDasConstFrameOutputs(const FString& ScriptFile,const FString& EntryPoint,const TArray<FDasScalarValue>& Arguments,int32 ResultKind,const FString& ResultClass,TArray<FDasScalarValue>& Outputs) const;

private:
    friend class FDasRuntimeModule;
    friend struct FDasActorHostRuntime;
    FString ActiveLifecyclePath;
    bool bLifecycleActive = false;
    void ApplyDasSceneRoot();
    void RecordLifecycleResult(const struct FDasExecutionResult& Result);

};
