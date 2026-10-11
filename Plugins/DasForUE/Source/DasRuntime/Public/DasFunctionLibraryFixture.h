#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasFunctionLibraryFixture.generated.h"
UCLASS()
class DASRUNTIME_API UDasFunctionLibraryFixture:public UBlueprintFunctionLibrary {
 GENERATED_BODY()
public:
 UFUNCTION(BlueprintPure,Category="daScript Test") static int32 Add(int32 A,int32 B);
 UFUNCTION(BlueprintPure,Category="daScript Test",meta=(WorldContext="Context")) static bool IsContextActor(const UObject* Context);
 UFUNCTION(BlueprintPure,Category="daScript Test",meta=(WorldContext="Context")) static int32 ContextWorldId(const UObject* Context);
};
