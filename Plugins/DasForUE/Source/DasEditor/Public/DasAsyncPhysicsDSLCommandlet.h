#pragma once
#include "Commandlets/Commandlet.h"
#include "DasAsyncPhysicsDSLCommandlet.generated.h"
UCLASS()
class UDasAsyncPhysicsDSLCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasAsyncPhysicsDSLCommandlet();virtual int32 Main(const FString& Params)override;
};
