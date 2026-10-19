#pragma once
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "DasCallFrame.generated.h"

// Internal Blueprint transport; scalar values never contain native pointers.
USTRUCT(BlueprintType)
struct DASRUNTIME_API FDasScalarValue
{
    GENERATED_BODY()
    // Native, transient transport: copy binding values, never context-owned VM tokens.
    FScriptDelegate DelegateValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    uint8 ByteValue = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FText TextValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    int32 Direction = 0; // in=0,out=1,inout=2
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    int32 Kind = 0; // int=0,float=1,bool=2,void=3,string=5,double=6,name=7
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    int32 IntValue = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    float FloatValue = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    bool BoolValue = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FString StringValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    double DoubleValue = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FName NameValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    TObjectPtr<UObject> ObjectValue = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    TObjectPtr<UClass> ClassValue = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FString ObjectClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FInstancedStruct StructValue;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FVector VectorValue = FVector::ZeroVector;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FRotator RotatorValue = FRotator::ZeroRotator;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "daScript")
    FTransform TransformValue = FTransform::Identity;
};
template <> struct TStructOpsTypeTraits<FDasScalarValue> : TStructOpsTypeTraitsBase2<FDasScalarValue>
{
    enum
    {
        WithCopy = true
    };
};
