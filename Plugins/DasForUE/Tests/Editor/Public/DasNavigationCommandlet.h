#pragma once
#include "Commandlets/Commandlet.h"
#include "DasNavigationCommandlet.generated.h"
UCLASS()
class UDasNavigationCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasNavigationCommandlet();
    virtual int32 Main(const FString& Params) override;
};
