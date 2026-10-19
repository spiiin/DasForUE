#pragma once
#include "Commandlets/Commandlet.h"
#include "DasGameplayTagsCommandlet.generated.h"
UCLASS()
class UDasGameplayTagsCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasGameplayTagsCommandlet();
 virtual int32 Main(const FString& Params) override;
};
