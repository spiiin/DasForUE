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

UCLASS()
class UDasGASHooksCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASHooksCommandlet();
    virtual int32 Main(const FString& Params) override;
};

UCLASS()
class ADasGASAdvancedProbeHost : public ADasGASAuthoredProbeHost
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> Other;
    UPROPERTY()
    FHitResult ProbeHit;
    UFUNCTION(BlueprintPure)
    FHitResult ReadProbeHit() const { return ProbeHit; }
};
UCLASS()
class UDasGASTargetingCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASTargetingCommandlet();
    virtual int32 Main(const FString& Params) override;
};
UCLASS()
class UDasGASAdvancedCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASAdvancedCommandlet();
    virtual int32 Main(const FString& Params) override;
};

UCLASS()
class UDasGASExecutionCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASExecutionCommandlet();
    virtual int32 Main(const FString& Params) override;
};

UCLASS()
class UDasGASCuesCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASCuesCommandlet();
    virtual int32 Main(const FString& Params) override;
};

UCLASS()
class UDasGASCueAssetsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASCueAssetsCommandlet();
    virtual int32 Main(const FString& Params) override;
};

UCLASS()
class UDasGASCueHooksCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASCueHooksCommandlet();
    virtual int32 Main(const FString& Params) override;
};

UCLASS()
class UDasGASCuePlaygroundCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasGASCuePlaygroundCommandlet();
    virtual int32 Main(const FString& Params) override;
};
