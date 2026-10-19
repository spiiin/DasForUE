#pragma once
#include "Commandlets/Commandlet.h"
#include "DasPawnAICommandlet.generated.h"
UCLASS()
class UDasPawnAICommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasPawnAICommandlet();
    virtual int32 Main(const FString& Params) override;
};
