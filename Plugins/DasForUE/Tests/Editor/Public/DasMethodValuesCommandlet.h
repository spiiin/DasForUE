#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasMethodValuesCommandlet.generated.h"
UCLASS()
class UDasMethodValuesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasMethodValuesCommandlet();
    virtual int32 Main(const FString& Params) override;
};
