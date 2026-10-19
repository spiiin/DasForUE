#pragma once
#include "Commandlets/Commandlet.h"
#include "DasImageSoftBrushCommandlet.generated.h"
UCLASS()
class UDasImageSoftBrushCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasImageSoftBrushCommandlet();virtual int32 Main(const FString& Params)override;
};
