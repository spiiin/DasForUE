#pragma once
#include "Commandlets/Commandlet.h"
#include "GameFramework/Actor.h"
#include "DasGASAuthoring.h"
#include "DasGASAuthoredCommandlet.generated.h"
UCLASS()
class ADasGASAuthoredProbeHost : public AActor
{
    GENERATED_BODY()
public:
    ADasGASAuthoredProbeHost();
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UDasAuthoredAbilitySystemComponent> Abilities;
};
UCLASS()
class UDasGASAuthoredCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASAuthoredCommandlet();
    virtual int32 Main(const FString& Params) override;
};
