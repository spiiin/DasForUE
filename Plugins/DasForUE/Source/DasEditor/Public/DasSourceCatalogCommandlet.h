#pragma once
#include "Commandlets/Commandlet.h"
#include "DasSourceCatalogCommandlet.generated.h"
UCLASS()
class UDasSourceCatalogCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasSourceCatalogCommandlet();
    virtual int32 Main(const FString& Params) override;
};
