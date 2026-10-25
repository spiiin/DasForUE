#pragma once
#include "DasSourceCatalogCommandlet.h"
#include "DasGameplaySourceTestCommandlet.generated.h"

// Compiler checks with in-memory reflected fixtures; no packages are saved.
UCLASS()
class UDasGameplaySourceTestCommandlet : public UDasSourceCatalogCommandlet
{
    GENERATED_BODY()
public:
    virtual int32 Main(const FString& Params) override;
};
