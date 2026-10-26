#pragma once
#include "Commandlets/Commandlet.h"
#include "DasAsyncPhysicsVMCommandlet.generated.h"
UCLASS()
class UDasAsyncPhysicsVMCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasAsyncPhysicsVMCommandlet();
    virtual int32 Main(const FString& Params) override;
};
