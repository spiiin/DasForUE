#pragma once
#include "Commandlets/Commandlet.h"
#include "DasCollisionProfileCommandlet.generated.h"
UCLASS()
class UDasCollisionProfileCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasCollisionProfileCommandlet();
    virtual int32 Main(const FString& Params) override;
};
