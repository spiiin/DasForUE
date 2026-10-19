#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasLatentProxy.generated.h"
UCLASS(Transient)
class UDasLatentProxy : public UObject
{
    GENERATED_BODY()
public:
    uint64 SubscriptionId = 0;
    UFUNCTION()
    void Complete(int32 Linkage);
};
