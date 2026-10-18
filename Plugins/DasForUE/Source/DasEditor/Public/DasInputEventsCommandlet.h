#pragma once
#include "Commandlets/Commandlet.h"
#include "DasInputEventsCommandlet.generated.h"
UCLASS()
class UDasInputEventsCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasInputEventsCommandlet();virtual int32 Main(const FString& Params)override;
};
