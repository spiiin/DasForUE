#pragma once
#include "Commandlets/Commandlet.h"
#include "DasScopedPhysicsBodyCommandlet.generated.h"
UCLASS()
class UDasScopedPhysicsBodyCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasScopedPhysicsBodyCommandlet();virtual int32 Main(const FString& Params)override;
};
