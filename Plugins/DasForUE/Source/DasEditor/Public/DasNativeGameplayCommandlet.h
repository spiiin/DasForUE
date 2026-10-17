#pragma once
#include "Commandlets/Commandlet.h"
#include "DasNativeGameplayCommandlet.generated.h"
UCLASS()
class UDasNativeGameplayCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasNativeGameplayCommandlet();
 virtual int32 Main(const FString& Params) override;
};
