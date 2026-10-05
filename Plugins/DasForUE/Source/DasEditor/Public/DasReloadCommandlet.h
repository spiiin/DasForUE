#pragma once
#include "Commandlets/Commandlet.h"
#include "DasReloadCommandlet.generated.h"

UCLASS()
class UDasReloadCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasReloadCommandlet();
    virtual int32 Main(const FString& Params) override;
};
