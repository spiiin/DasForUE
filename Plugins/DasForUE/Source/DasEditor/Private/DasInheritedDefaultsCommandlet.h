#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasInheritedDefaultsCommandlet.generated.h"
UCLASS()
class UDasInheritedDefaultsCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasInheritedDefaultsCommandlet();
 virtual int32 Main(const FString& Params)override;
};
