#pragma once
#include "Commandlets/Commandlet.h"
#include "DasEnumNamesCommandlet.generated.h"
UCLASS()
class UDasEnumNamesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasEnumNamesCommandlet();
    virtual int32 Main(const FString& Params) override;
};
