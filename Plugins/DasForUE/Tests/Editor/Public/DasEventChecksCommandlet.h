#pragma once
#include "Commandlets/Commandlet.h"
#include "DasEventChecksCommandlet.generated.h"
UCLASS()
class UDasEventChecksCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasEventChecksCommandlet();
    virtual int32 Main(const FString& Params) override;
};
