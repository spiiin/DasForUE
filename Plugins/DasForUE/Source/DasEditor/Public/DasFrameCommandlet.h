#pragma once
#include "Commandlets/Commandlet.h"
#include "DasFrameCommandlet.generated.h"
UCLASS()
class UDasFrameCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasFrameCommandlet();
    virtual int32 Main(const FString& Params) override;
};
