#pragma once
#include "Commandlets/Commandlet.h"
#include "DasCollisionFoundationCommandlet.generated.h"
UCLASS()
class UDasCollisionFoundationCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasCollisionFoundationCommandlet();
 virtual int32 Main(const FString& Params)override;
};
