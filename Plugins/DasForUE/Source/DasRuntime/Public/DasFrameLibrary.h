#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasCallFrame.h"
#include "DasFrameLibrary.generated.h"
UCLASS()
class DASRUNTIME_API UDasFrameLibrary : public UBlueprintFunctionLibrary {
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure,Category="daScript",meta=(BlueprintInternalUseOnly="true"))
    static FDasScalarValue DefaultValue(int32 Kind,const FString& ClassPath,int32 Direction);
    UFUNCTION(BlueprintPure,Category="daScript",meta=(BlueprintInternalUseOnly="true"))
    static FDasScalarValue OutputAt(const TArray<FDasScalarValue>& Values,int32 Index);
};
