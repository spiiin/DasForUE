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
};
