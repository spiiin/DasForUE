#pragma once
#include "Commandlets/Commandlet.h"
#include "DasSaveGameCommandlet.generated.h"
UCLASS()
class UDasSaveGameCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasSaveGameCommandlet();
 virtual int32 Main(const FString& Params) override;
};
