#pragma once
#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "DasStaticLibraryCommandlet.generated.h"
UCLASS()
class UDasStaticLibraryCommandlet : public UCommandlet {
 GENERATED_BODY()
public:
 UDasStaticLibraryCommandlet();virtual int32 Main(const FString& Params)override;
};
