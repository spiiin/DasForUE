#pragma once
#include "Commandlets/Commandlet.h"
#include "DasPhysicsSimulationCommandlet.generated.h"
UCLASS()
class UDasPhysicsSimulationCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasPhysicsSimulationCommandlet();
 virtual int32 Main(const FString& Params)override;
};
