#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DasCallFrame.h"
#include "DasNumericLibrary.generated.h"
UENUM(BlueprintType)
enum class EDasIntegerRule : uint8
{
    Exact,
    Truncate,
    Floor,
    Ceil,
    NearestAway
};
UENUM(BlueprintType)
enum class EDasFloatRule : uint8
{
    Exact,
    RoundNearest
};
DASRUNTIME_API bool ConvertDasNumber(const FDasScalarValue& Value, int32 Target, EDasIntegerRule IntegerRule,
                                     EDasFloatRule FloatRule, FDasScalarValue& Out, FString& Error);
UCLASS()
class DASRUNTIME_API UDasNumericLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintPure, CustomThunk, Category = "daScript", meta = (BlueprintInternalUseOnly = "true"))
    static double IntToDouble(int32 Value);
    DECLARE_FUNCTION(execIntToDouble);
    UFUNCTION(BlueprintPure, CustomThunk, Category = "daScript", meta = (BlueprintInternalUseOnly = "true"))
    static double FloatToDouble(float Value);
    DECLARE_FUNCTION(execFloatToDouble);
    UFUNCTION(BlueprintPure, CustomThunk, Category = "daScript", meta = (BlueprintInternalUseOnly = "true"))
    static float IntToFloat(int32 Value, EDasFloatRule Rule);
    DECLARE_FUNCTION(execIntToFloat);
    UFUNCTION(BlueprintPure, CustomThunk, Category = "daScript", meta = (BlueprintInternalUseOnly = "true"))
    static float DoubleToFloat(double Value, EDasFloatRule Rule);
    DECLARE_FUNCTION(execDoubleToFloat);
    UFUNCTION(BlueprintPure, CustomThunk, Category = "daScript", meta = (BlueprintInternalUseOnly = "true"))
    static int32 FloatToInt(float Value, EDasIntegerRule Rule);
    DECLARE_FUNCTION(execFloatToInt);
    UFUNCTION(BlueprintPure, CustomThunk, Category = "daScript", meta = (BlueprintInternalUseOnly = "true"))
    static int32 DoubleToInt(double Value, EDasIntegerRule Rule);
    DECLARE_FUNCTION(execDoubleToInt);
};
