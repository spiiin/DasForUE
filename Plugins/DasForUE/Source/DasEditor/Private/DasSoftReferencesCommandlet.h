#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasSoftReferencesCommandlet.generated.h"
UCLASS()
class UDasSoftReferencesCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasSoftReferencesCommandlet();
 virtual int32 Main(const FString& Params)override;
};
