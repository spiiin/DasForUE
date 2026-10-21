#pragma once
#include "Commandlets/Commandlet.h"
#include "DasUEAotCommandlet.generated.h"
UCLASS()
class UDasUEAotCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasUEAotCommandlet();
    virtual int32 Main(const FString& Params) override;
};
