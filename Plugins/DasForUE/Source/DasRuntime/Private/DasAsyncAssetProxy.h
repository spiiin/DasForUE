#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/SoftObjectPath.h"
#include "DasAsyncAssetProxy.generated.h"
UCLASS(Transient)
class UDasAsyncAssetProxy:public UObject {
    GENERATED_BODY()
public:
    uint64 SubscriptionId=0;
    FSoftObjectPath Path;
    FString ExpectedClass;
    bool ClassReference=false;
    void Complete();
};
