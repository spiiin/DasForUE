#pragma once
#include "Commandlets/Commandlet.h"
#include "DasAssetCheckCommandlet.generated.h"
UCLASS()
class UDasAssetCheckCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasAssetCheckCommandlet();
    virtual int32 Main(const FString& Params) override;
};
