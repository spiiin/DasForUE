#pragma once
#include "Commandlets/Commandlet.h"
#include "DasExportCommandlet.generated.h"
UCLASS()
class UDasExportCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasExportCommandlet();
    virtual int32 Main(const FString& Params) override;
};
