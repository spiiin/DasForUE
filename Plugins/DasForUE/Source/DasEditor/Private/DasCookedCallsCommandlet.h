#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasCookedCallsCommandlet.generated.h"
UCLASS()
class UDasCookedCallsCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasCookedCallsCommandlet();
 virtual int32 Main(const FString& Params)override;
};
