#pragma once
#include "Commandlets/Commandlet.h"
#include "DasWidgetEventsCommandlet.generated.h"

UCLASS()
class UDasWidgetEventsCommandlet : public UCommandlet
{
 GENERATED_BODY()
public:
 UDasWidgetEventsCommandlet();
 virtual int32 Main(const FString& Params) override;
};
