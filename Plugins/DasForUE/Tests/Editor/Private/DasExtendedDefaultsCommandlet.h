#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasExtendedDefaultsCommandlet.generated.h"
UCLASS()
class UDasExtendedDefaultsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasExtendedDefaultsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
