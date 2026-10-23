#pragma once
#include "Commandlets/Commandlet.h"
#include "DasSubsystemsCommandlet.generated.h"
UCLASS()
class UDasSubsystemsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasSubsystemsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
