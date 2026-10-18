#pragma once
#include "Commandlets/Commandlet.h"
#include "Components/BoxComponent.h"
#include "DasAsyncPhysicsAuditCommandlet.generated.h"
UCLASS()
class UDasAsyncPhysicsAuditBody:public UBoxComponent {
 GENERATED_BODY()
public:
 void EnableAuditTick(bool Enabled){SetAsyncPhysicsTickEnabled(Enabled);}
 int32 CallbackCount=0,ValidCount=0;bool AllGameThread=true,AllPhysicsContext=true,AllFrozenContext=true;
 int32 ForceMode=0;double ForcedTime=0;
 virtual void AsyncPhysicsTickComponent(float Dt,float SimTime)override;
};
UCLASS()
class UDasAsyncPhysicsAuditCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasAsyncPhysicsAuditCommandlet();virtual int32 Main(const FString& Params)override;
};
