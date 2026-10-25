#pragma once
#include "Commandlets/Commandlet.h"
#include "DasAccessContextCommandlet.generated.h"
UCLASS()
class UDasAccessContextCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasAccessContextCommandlet();
    virtual int32 Main(const FString& Params) override;
};
