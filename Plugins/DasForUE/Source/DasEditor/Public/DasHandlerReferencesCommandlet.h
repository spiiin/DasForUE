#pragma once
#include "Commandlets/Commandlet.h"
#include "DasHandlerReferencesCommandlet.generated.h"
UCLASS()
class UDasHandlerReferencesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasHandlerReferencesCommandlet();
    virtual int32 Main(const FString& Params) override;
};
