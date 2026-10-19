#pragma once
#include "Commandlets/Commandlet.h"
#include "DasGeometryCommandlet.generated.h"
UCLASS()
class UDasGeometryCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGeometryCommandlet();
    virtual int32 Main(const FString& Params) override;
};
