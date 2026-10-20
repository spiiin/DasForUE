#pragma once
#include "Commandlets/Commandlet.h"
#include "DasHiddenCallbacksCommandlet.generated.h"
UCLASS()
class UDasHiddenCallbacksCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasHiddenCallbacksCommandlet();
    virtual int32 Main(const FString& Params) override;
};
