#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasActorComponentCommandlet.generated.h"
UCLASS()
class UDasActorComponentCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasActorComponentCommandlet();
    virtual int32 Main(const FString& Params) override;
};
