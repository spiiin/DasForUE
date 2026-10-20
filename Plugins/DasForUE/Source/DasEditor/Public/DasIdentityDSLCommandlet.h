#pragma once
#include "Commandlets/Commandlet.h"
#include "DasIdentityDSLCommandlet.generated.h"
UCLASS()
class UDasIdentityDSLCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasIdentityDSLCommandlet();
    virtual int32 Main(const FString& Params) override;
};
