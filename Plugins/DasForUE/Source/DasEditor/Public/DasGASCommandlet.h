#pragma once
#include "Commandlets/Commandlet.h"
#include "GameFramework/Actor.h"
#include "DasGAS.h"
#include "DasGASCommandlet.generated.h"

UCLASS()
class ADasGASProbeHost : public AActor
{
    GENERATED_BODY()
public:
    ADasGASProbeHost();
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UDasHealthAbilitySystemComponent> Abilities;
};
UCLASS()
class UDasGASCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASCommandlet();
    virtual int32 Main(const FString& Params) override;
};
