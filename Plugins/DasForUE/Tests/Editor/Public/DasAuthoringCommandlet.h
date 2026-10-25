#pragma once
#include "Commandlets/Commandlet.h"
#include "DasAuthoringCommandlet.generated.h"
UCLASS()
class UDasAuthoringCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasAuthoringCommandlet();
    virtual int32 Main(const FString& Params) override;
};
