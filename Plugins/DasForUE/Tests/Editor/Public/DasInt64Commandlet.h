#pragma once
#include "Commandlets/Commandlet.h"
#include "DasInt64Commandlet.generated.h"
UCLASS()
class UDasInt64Commandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasInt64Commandlet();
    virtual int32 Main(const FString& Params) override;
};
