#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasCallFrame.h"
#include "DasInterfaceLibrary.generated.h"
UCLASS()
class DASRUNTIME_API UDasInterfaceLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
    static FDasScalarValue PackValue(const int32& Value, int32 Direction = 0);
    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
    static void UnpackValue(const FDasScalarValue& Data, int32& Value);
    DECLARE_FUNCTION(execPackValue);
    DECLARE_FUNCTION(execUnpackValue);
};
