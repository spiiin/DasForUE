#pragma once
#include "Commandlets/Commandlet.h"
#include "DasSmokeCommandlet.generated.h"

UCLASS()
class UDasSmokeCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasSmokeCommandlet();
    virtual int32 Main(const FString& Params) override;
};
