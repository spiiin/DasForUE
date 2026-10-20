#pragma once
#include "Commandlets/Commandlet.h"
#include "DasInlineCallbacksCommandlet.generated.h"
UCLASS()
class UDasInlineCallbacksCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasInlineCallbacksCommandlet();
    virtual int32 Main(const FString& Params) override;
};
