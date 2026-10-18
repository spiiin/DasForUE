#pragma once
#include "CoreMinimal.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "DasConstraintTransportRecord.generated.h"

struct FProxyTimestampBase;
// Private capability state. No native accessor, index or release callback is persisted.
USTRUCT()
struct FDasConstraintTransportRecord {
 GENERATED_BODY()
 UPROPERTY() TWeakObjectPtr<UPhysicsConstraintComponent> Owner;
 UPROPERTY() TWeakObjectPtr<UPrimitiveComponent> Body1;
 UPROPERTY() TWeakObjectPtr<UPrimitiveComponent> Body2;
 UPROPERTY() FName Bone1;
 UPROPERTY() FName Bone2;
 bool HadBody1=false,HadBody2=false,Broken=false;
 TSharedPtr<FProxyTimestampBase,ESPMode::ThreadSafe> Generation;
};
template<>struct TStructOpsTypeTraits<FDasConstraintTransportRecord>:TStructOpsTypeTraitsBase2<FDasConstraintTransportRecord>{enum{WithCopy=true};};
