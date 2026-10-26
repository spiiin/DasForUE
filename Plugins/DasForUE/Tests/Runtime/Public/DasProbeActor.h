#pragma once
#include "DasActor.h"
#include "DasCallbackValueTypes.h"
#include "DasProbeActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasByteSignal, uint8, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasTextSignal, FText, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasTextRefSignal, const FText&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayFloatSignal, const TArray<float>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayBoolSignal, const TArray<bool>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayNameSignal, const TArray<FName>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayByteSignal, const TArray<uint8>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayTextSignal, const TArray<FText>&, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasIntSignal, int32, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasFloatSignal, float, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasBoolSignal, bool, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasStringSignal, const FString&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDasVoidSignal);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVector2DSignal, FVector2D, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVector2DRefSignal, const FVector2D&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasNameSignal, FName, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayIntSignal, const TArray<int32>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayDoubleSignal, const TArray<double>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasArrayStringSignal, const TArray<FString>&, Value);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasDoubleSignal, double, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasObjectSignal, UObject*, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasCallbackActorSignal, AActor*, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasCallbackActorsSignal, const TArray<AActor*>&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVectorSignal, FVector, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasVectorRefSignal, const FVector&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasRotatorSignal, FRotator, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasRotatorRefSignal, const FRotator&, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasTransformSignal, FTransform, Value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDasTransformRefSignal, const FTransform&, Value);


// Explicit regression host; absent from ordinary game targets.
UCLASS(Blueprintable)
class DASRUNTIME_API ADasProbeActor : public ADasActor
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasByteSignal OnDasByteSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasTextSignal OnDasTextSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasTextRefSignal OnDasTextRefSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasArrayFloatSignal OnDasArrayFloatSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasArrayBoolSignal OnDasArrayBoolSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasArrayNameSignal OnDasArrayNameSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasArrayByteSignal OnDasArrayByteSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasArrayTextSignal OnDasArrayTextSignal;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasCallbackEnumRefSignal OnDasCallbackEnumRef;
    UPROPERTY(BlueprintAssignable, Category = "daScript Probe")
    FDasCallbackActorSignal OnDasCallbackActor;
    UPROPERTY(BlueprintAssignable, Category = "daScript Probe")
    FDasCallbackActorsSignal OnDasCallbackActors;
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Tests")
    FDasCallbackRecordRefSignal OnDasCallbackRecordRef;
    // Native storage used only by the scoped array regression fixture.
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<ESpawnActorCollisionHandlingMethod> DasNativeArrayEnums;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<TEnumAsByte<EComponentMobility::Type>> DasNativeArrayBytes;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<FVector> DasNativeArrayVectors;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<FRotator> DasNativeArrayRotators;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<FTransform> DasNativeArrayTransforms;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<FIntPoint> DasNativeArrayPoints;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TArray<FVector> DasNativeArrayReadonly;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<TObjectPtr<AActor>> DasNativeArrayObjects;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TArray<TObjectPtr<APawn>> DasNativeArrayPawns;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<int32, int32> DasNativeArrayIntMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FString, double> DasNativeArrayDoubleMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<int32, FName> DasNativeArrayNameMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FString, FVector> DasNativeArrayVectorMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<int32, FIntPoint> DasNativeArrayPointMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<int32, ESpawnActorCollisionHandlingMethod> DasNativeArrayEnumMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FString, TEnumAsByte<EComponentMobility::Type>> DasNativeArrayByteMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<int32, TObjectPtr<APawn>> DasNativeArrayPawnMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<int32> DasNativeArrayIntSet;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<FString> DasNativeArrayStringSet;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TMap<int32, int32> DasNativeArrayReadonlyMap;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TSet<FString> DasNativeArrayReadonlySet;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TArray<TObjectPtr<APawn>> DasNativeArrayReadonlyObjects;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<FName> DasNativeArrayNameSet;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, int32> DasNativeArrayNameKeys;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, FName> DasNativeArrayNameNames;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, FVector> DasNativeArrayNameVectors;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, FIntPoint> DasNativeArrayNamePoints;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, ESpawnActorCollisionHandlingMethod> DasNativeArrayNameEnums;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, TEnumAsByte<EComponentMobility::Type>> DasNativeArrayNameBytes;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<FName, TObjectPtr<APawn>> DasNativeArrayNameObjects;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TSet<FName> DasNativeArrayReadonlyNameSet;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TMap<FName, int32> DasNativeArrayReadonlyNameMap;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<ESpawnActorCollisionHandlingMethod> DasNativeArrayEnumSet;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<TEnumAsByte<EComponentMobility::Type>> DasNativeArrayByteSet;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TSet<ESpawnActorCollisionHandlingMethod> DasNativeArrayReadonlyEnumSet;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<ESpawnActorCollisionHandlingMethod, int32> DasNativeArrayEnumKeys;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<TEnumAsByte<EComponentMobility::Type>, FVector> DasNativeArrayByteKeys;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<ESpawnActorCollisionHandlingMethod, ESpawnActorCollisionHandlingMethod> DasNativeArrayEnumKeyEnums;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<ESpawnActorCollisionHandlingMethod, TObjectPtr<AActor>> DasNativeArrayEnumKeyObjects;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TMap<ESpawnActorCollisionHandlingMethod, int32> DasNativeArrayLockedEnumKeys;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<TObjectPtr<AActor>> DasNativeArrayActorSet;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TSet<TObjectPtr<APawn>> DasNativeArrayPawnSet;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<TObjectPtr<AActor>, int32> DasNativeArrayObjectKeys;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<TObjectPtr<AActor>, FVector> DasNativeArrayObjectVectors;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<TObjectPtr<AActor>, ESpawnActorCollisionHandlingMethod> DasNativeArrayObjectEnums;
    UPROPERTY(BlueprintReadWrite, Category = "DasForUE|Tests")
    TMap<TObjectPtr<APawn>, TObjectPtr<APawn>> DasNativeArrayPawnKeys;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TSet<TObjectPtr<AActor>> DasNativeArrayReadonlyActorSet;
    UPROPERTY(BlueprintReadOnly, Category = "DasForUE|Tests")
    TMap<TObjectPtr<AActor>, int32> DasNativeArrayReadonlyObjectKeys;
    UPROPERTY(BlueprintAssignable, Category = "daScript")
    FDasIntSignal OnDasSignal;
    UFUNCTION(BlueprintCallable, Category = "daScript")
    void EmitDasSignal(int32 Value) { OnDasSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasFloatSignal OnDasFloatSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasFloatSignal(float Value) { OnDasFloatSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasBoolSignal OnDasBoolSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasBoolSignal(bool Value) { OnDasBoolSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasStringSignal OnDasStringSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasStringSignal(const FString& Value) { OnDasStringSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasDoubleSignal OnDasDoubleSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasDoubleSignal(double Value) { OnDasDoubleSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasObjectSignal OnDasObjectSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasObjectSignal(UObject* Value) { OnDasObjectSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasVectorRefSignal OnDasVectorRefSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasVectorRefSignal(const FVector& Value) { OnDasVectorRefSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasRotatorSignal OnDasRotatorSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasRotatorSignal(FRotator Value) { OnDasRotatorSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasRotatorRefSignal OnDasRotatorRefSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasRotatorRefSignal(const FRotator& Value) { OnDasRotatorRefSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasTransformSignal OnDasTransformSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasTransformSignal(FTransform Value) { OnDasTransformSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasTransformRefSignal OnDasTransformRefSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasTransformRefSignal(const FTransform& Value) { OnDasTransformRefSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasVectorSignal OnDasVectorSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasVectorSignal(FVector Value) { OnDasVectorSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasNameSignal OnDasNameSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasNameSignal(FName Value) { OnDasNameSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasArrayIntSignal OnDasArrayIntSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasArrayIntSignal(const TArray<int32>& Value) { OnDasArrayIntSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasArrayDoubleSignal OnDasArrayDoubleSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasArrayDoubleSignal(const TArray<double>& Value) { OnDasArrayDoubleSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasArrayStringSignal OnDasArrayStringSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasArrayStringSignal(const TArray<FString>& Value) { OnDasArrayStringSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasVector2DSignal OnDasVector2DSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasVector2DSignal(FVector2D Value) { OnDasVector2DSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasVector2DRefSignal OnDasVector2DRefSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasVector2DRefSignal(const FVector2D& Value) { OnDasVector2DRefSignal.Broadcast(Value); }
    UPROPERTY(BlueprintAssignable, Category = "DasForUE|Signals")
    FDasVoidSignal OnDasVoidSignal;
    UFUNCTION(BlueprintCallable, Category = "DasForUE|Signals")
    void EmitDasVoidSignal() { OnDasVoidSignal.Broadcast(); }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript Probe")
    TObjectPtr<AActor> Peer;
};
