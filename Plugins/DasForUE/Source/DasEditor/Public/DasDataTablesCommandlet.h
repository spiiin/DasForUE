#pragma once
#include "Commandlets/Commandlet.h"
#include "DasDataTablesCommandlet.generated.h"
UCLASS()
class UDasDataTablesCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasDataTablesCommandlet();
 virtual int32 Main(const FString& Params) override;
};
