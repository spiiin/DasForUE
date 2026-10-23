#pragma once
#include "Commandlets/Commandlet.h"
#include "DasInteropPerformanceCommandlet.generated.h"

// Call-cost benchmark (Docs/Dev/Architecture/Performance.md): the runtime cases of RunDasInteropProbe plus the
// cases that need an imported DSL class. -PerfSamples=N, -PerfOutput=report.json.
UCLASS()
class UDasInteropPerformanceCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasInteropPerformanceCommandlet();
    virtual int32 Main(const FString& Params) override;
};
