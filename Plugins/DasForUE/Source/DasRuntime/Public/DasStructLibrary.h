#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "DasStructLibrary.generated.h"
UCLASS()
class DASRUNTIME_API UDasStructLibrary : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure,CustomThunk,Category="daScript",meta=(BlueprintInternalUseOnly="true",CustomStructureParam="Value"))
    static FInstancedStruct PackValue(const int32& Value);
    DECLARE_FUNCTION(execPackValue);
    UFUNCTION(BlueprintPure,CustomThunk,Category="daScript",meta=(BlueprintInternalUseOnly="true",CustomStructureParam="Value"))
    static void UnpackValue(const FInstancedStruct& Data,int32& Value);
    DECLARE_FUNCTION(execUnpackValue);
};
