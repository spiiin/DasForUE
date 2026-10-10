#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasIntDelegateProxy.generated.h"

UCLASS(Transient)
class UDasIntDelegateProxy : public UObject
{
    GENERATED_BODY()
public:
    uint64 SubscriptionId = 0;
    UFUNCTION()
    void Receive(int32 Value);
    UFUNCTION() void ReceiveFloat(float Value);
    UFUNCTION() void ReceiveBool(bool Value);
    UFUNCTION() void ReceiveString(const FString& Value);
    UFUNCTION() void ReceiveVoid();
    UFUNCTION() void ReceiveName(FName Value);
    UFUNCTION() void ReceiveArrayInt(const TArray<int32>& Value);
    UFUNCTION() void ReceiveArrayDouble(const TArray<double>& Value);
    UFUNCTION() void ReceiveArrayString(const TArray<FString>& Value);
    UFUNCTION() void ReceiveDouble(double Value);
    UFUNCTION() void ReceiveObject(UObject* Value);
    UFUNCTION() void ReceiveVector(FVector Value);
    UFUNCTION() void ReceiveVector2D(FVector2D Value);
    UFUNCTION() void ReceiveVector2DRef(const FVector2D& Value);
    UFUNCTION() void ReceiveVectorRef(const FVector& Value);
    UFUNCTION() void ReceiveRotator(FRotator Value);
    UFUNCTION() void ReceiveRotatorRef(const FRotator& Value);
    UFUNCTION() void ReceiveTransform(FTransform Value);
    UFUNCTION() void ReceiveTransformRef(const FTransform& Value);
};
