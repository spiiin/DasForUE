#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InputActionValue.h"
#include "DasEnhancedInputProxy.generated.h"
class UInputAction;
UCLASS(Transient)
class UDasEnhancedInputProxy:public UObject {
    GENERATED_BODY()
public:
    uint64 SubscriptionId=0;
    int32 PayloadKind=2;
    bool TypedValue=false;
    UPROPERTY() TObjectPtr<const UInputAction> Action;
    UFUNCTION() void Receive(FInputActionValue ActionValue,float ElapsedTime,float TriggeredTime,const UInputAction* SourceAction);
};
