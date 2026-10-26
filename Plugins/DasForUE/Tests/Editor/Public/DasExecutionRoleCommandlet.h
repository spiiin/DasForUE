#pragma once
#include "Commandlets/Commandlet.h"
#include "Components/ActorComponent.h"
#include "DasExecutionRoleCommandlet.generated.h"
UCLASS()
class UDasUnscopedPhysicsRoleProbe : public UActorComponent
{
    GENERATED_BODY()
public:
    TFunction<void()> Callback;
    void Enable(bool Enabled) { SetAsyncPhysicsTickEnabled(Enabled); }
    virtual void AsyncPhysicsTickComponent(float, float) override
    {
        if (Callback)
            Callback();
    }
};
UCLASS()
class UDasExecutionRoleCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasExecutionRoleCommandlet();
    virtual int32 Main(const FString& Params) override;
};
