#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasCookedGameplayCommandlet.generated.h"
UCLASS()
class UDasCookedGameplayCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasCookedGameplayCommandlet();
    virtual int32 Main(const FString& Params) override;
};
