#pragma once
#include "Commandlets/Commandlet.h"
#include "DasSaveGameCompatibilityCommandlet.generated.h"
UCLASS()
class UDasSaveGameCompatibilityCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasSaveGameCompatibilityCommandlet();
    virtual int32 Main(const FString& Params) override;
};
