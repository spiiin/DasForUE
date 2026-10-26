#pragma once
#include "Commandlets/Commandlet.h"
#include "DasBuiltinValuesCommandlet.generated.h"
UCLASS()
class UDasBuiltinValuesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasBuiltinValuesCommandlet();
    virtual int32 Main(const FString& Params) override;
};
