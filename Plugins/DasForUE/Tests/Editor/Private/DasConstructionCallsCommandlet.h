#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasConstructionCallsCommandlet.generated.h"

UCLASS()
class UDasConstructionCallsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasConstructionCallsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
