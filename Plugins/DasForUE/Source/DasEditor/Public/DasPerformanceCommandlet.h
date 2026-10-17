#pragma once
#include "Commandlets/Commandlet.h"
#include "DasPerformanceCommandlet.generated.h"

UCLASS()
class UDasPerformanceCommandlet : public UCommandlet {
    GENERATED_BODY()
public:
    UDasPerformanceCommandlet();
    virtual int32 Main(const FString& Params) override;
};
