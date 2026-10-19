#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasComputedDefaultsCommandlet.generated.h"
UCLASS()
class UDasComputedDefaultsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasComputedDefaultsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
