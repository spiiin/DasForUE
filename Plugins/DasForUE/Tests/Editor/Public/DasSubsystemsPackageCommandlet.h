#pragma once
#include "Commandlets/Commandlet.h"
#include "DasSubsystemsPackageCommandlet.generated.h"
UCLASS()
class UDasSubsystemsPackageCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasSubsystemsPackageCommandlet();
    virtual int32 Main(const FString& Params) override;
};
