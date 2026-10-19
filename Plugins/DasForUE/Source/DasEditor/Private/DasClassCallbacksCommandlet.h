#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasClassCallbacksCommandlet.generated.h"
UCLASS()
class UDasClassCallbacksCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasClassCallbacksCommandlet();
    virtual int32 Main(const FString& Params) override;
};
