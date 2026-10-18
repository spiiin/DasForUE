#pragma once
#include "Commandlets/Commandlet.h"
#include "DasAuthoredAssetsCommandlet.generated.h"
UCLASS()
class UDasAuthoredAssetsCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasAuthoredAssetsCommandlet();
 virtual int32 Main(const FString& Params) override;
};
