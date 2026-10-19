#pragma once
#include "Commandlets/Commandlet.h"
#include "DasCurveTablesCommandlet.generated.h"
UCLASS()
class UDasCurveTablesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasCurveTablesCommandlet();
    virtual int32 Main(const FString& Params) override;
};
