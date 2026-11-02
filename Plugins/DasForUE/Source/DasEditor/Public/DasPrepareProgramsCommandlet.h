#pragma once
#include "Commandlets/Commandlet.h"
#include "DasPrepareProgramsCommandlet.generated.h"
UCLASS()
class UDasPrepareProgramsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasPrepareProgramsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
