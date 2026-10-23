#pragma once
#include "Commandlets/Commandlet.h"
#include "DasDSLDiagnosticsCommandlet.generated.h"
UCLASS()
class UDasDSLDiagnosticsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasDSLDiagnosticsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
