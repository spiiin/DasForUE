#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasNativeDefaultsCommandlet.generated.h"
UCLASS()
class UDasNativeDefaultsCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasNativeDefaultsCommandlet();
 virtual int32 Main(const FString& Params)override;
};
