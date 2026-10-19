#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasCallFrame.h"
#include "DasDelegateLibrary.generated.h"
UCLASS()
class DASRUNTIME_API UDasDelegateLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
    static FDasScalarValue PackValue(const int32& Value, const FString& SignatureWire);
    UFUNCTION(BlueprintPure, CustomThunk, meta = (CustomStructureParam = "Value", BlueprintInternalUseOnly = "true"))
    static void UnpackValue(const FDasScalarValue& Data, int32& Value);
    DECLARE_FUNCTION(execPackValue);
    DECLARE_FUNCTION(execUnpackValue);
};
