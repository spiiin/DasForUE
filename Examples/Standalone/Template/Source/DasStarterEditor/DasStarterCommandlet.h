#pragma once
#include "Commandlets/Commandlet.h"
#include "DasStarterCommandlet.generated.h"
UCLASS()
class UDasStarterCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasStarterCommandlet();
 virtual int32 Main(const FString& Params)override;
};
