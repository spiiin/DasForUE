#pragma once
#include "Commandlets/Commandlet.h"
#include "DasInheritedHandlersCommandlet.generated.h"
UCLASS()
class UDasInheritedHandlersCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasInheritedHandlersCommandlet();
    virtual int32 Main(const FString& Params) override;
};
