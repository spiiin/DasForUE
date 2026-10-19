#include "DasNumericLibrary.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include <cmath>
#include <cfloat>
bool ConvertDasNumber(const FDasScalarValue& V, int32 Target, EDasIntegerRule I, EDasFloatRule F, FDasScalarValue& Out,
                      FString& Error)
{
    Error.Reset();
    Out = FDasScalarValue();
    if ((V.Kind != 0 && V.Kind != 1 && V.Kind != 6) || (Target != 0 && Target != 1 && Target != 6) || uint8(I) > 4 ||
        uint8(F) > 1)
    {
        Error = TEXT("Invalid numeric conversion policy/type");
        return false;
    }
    double N = V.Kind == 0 ? double(V.IntValue) : V.Kind == 1 ? double(V.FloatValue) : V.DoubleValue;
    if (!FMath::IsFinite(N))
    {
        Error = TEXT("Non-finite numeric value");
        return false;
    }
    Out.Kind = Target;
    if (Target == 6)
    {
        Out.DoubleValue = N;
        return true;
    }
    if (Target == 1)
    {
        if (std::abs(N) > double(FLT_MAX))
        {
            Error = TEXT("Numeric value exceeds float32 range");
            return false;
        }
        Out.FloatValue = float(N);
        if (F == EDasFloatRule::Exact && double(Out.FloatValue) != N)
        {
            Error = TEXT("Numeric conversion loses float32 precision");
            return false;
        }
        return true;
    }
    double R = I == EDasIntegerRule::Floor         ? std::floor(N)
               : I == EDasIntegerRule::Ceil        ? std::ceil(N)
               : I == EDasIntegerRule::NearestAway ? std::round(N)
                                                   : std::trunc(N);
    if (I == EDasIntegerRule::Exact && R != N)
    {
        Error = TEXT("Non-integral value requires explicit integer rounding");
        return false;
    }
    if (R < double(MIN_int32) || R > double(MAX_int32))
    {
        Error = TEXT("Numeric value exceeds int32 range");
        return false;
    }
    Out.IntValue = int32(R);
    return true;
}
namespace
{
void NumericFailure(UObject* Context, FFrame& Stack, const FString& Error)
{
    FBlueprintCoreDelegates::ThrowScriptException(
        Context, Stack, FBlueprintExceptionInfo(EBlueprintExceptionType::AbortExecution, FText::FromString(Error)));
}
}
double UDasNumericLibrary::IntToDouble(int32 Value)
{
    checkNoEntry();
    return 0;
}
DEFINE_FUNCTION(UDasNumericLibrary::execIntToDouble)
{
    P_GET_PROPERTY(FIntProperty, Value);
    P_FINISH;
    P_NATIVE_BEGIN;
    FDasScalarValue V, Out;
    FString Error;
    V.Kind = 0;
    V.IntValue = Value;
    if (ConvertDasNumber(V, 6, EDasIntegerRule::Exact, EDasFloatRule::Exact, Out, Error))
        *static_cast<double*>(RESULT_PARAM) = Out.DoubleValue;
    else
        NumericFailure(P_THIS, Stack, Error);
    P_NATIVE_END;
}
double UDasNumericLibrary::FloatToDouble(float Value)
{
    checkNoEntry();
    return 0;
}
DEFINE_FUNCTION(UDasNumericLibrary::execFloatToDouble)
{
    P_GET_PROPERTY(FFloatProperty, Value);
    P_FINISH;
    P_NATIVE_BEGIN;
    FDasScalarValue V, Out;
    FString Error;
    V.Kind = 1;
    V.FloatValue = Value;
    if (ConvertDasNumber(V, 6, EDasIntegerRule::Exact, EDasFloatRule::Exact, Out, Error))
        *static_cast<double*>(RESULT_PARAM) = Out.DoubleValue;
    else
        NumericFailure(P_THIS, Stack, Error);
    P_NATIVE_END;
}
float UDasNumericLibrary::IntToFloat(int32 Value, EDasFloatRule Rule)
{
    checkNoEntry();
    return 0;
}
DEFINE_FUNCTION(UDasNumericLibrary::execIntToFloat)
{
    P_GET_PROPERTY(FIntProperty, Value);
    P_GET_ENUM(EDasFloatRule, Rule);
    P_FINISH;
    P_NATIVE_BEGIN;
    FDasScalarValue V, Out;
    FString Error;
    V.Kind = 0;
    V.IntValue = Value;
    if (ConvertDasNumber(V, 1, EDasIntegerRule::Exact, static_cast<EDasFloatRule>(Rule), Out, Error))
        *static_cast<float*>(RESULT_PARAM) = Out.FloatValue;
    else
        NumericFailure(P_THIS, Stack, Error);
    P_NATIVE_END;
}
float UDasNumericLibrary::DoubleToFloat(double Value, EDasFloatRule Rule)
{
    checkNoEntry();
    return 0;
}
DEFINE_FUNCTION(UDasNumericLibrary::execDoubleToFloat)
{
    P_GET_PROPERTY(FDoubleProperty, Value);
    P_GET_ENUM(EDasFloatRule, Rule);
    P_FINISH;
    P_NATIVE_BEGIN;
    FDasScalarValue V, Out;
    FString Error;
    V.Kind = 6;
    V.DoubleValue = Value;
    if (ConvertDasNumber(V, 1, EDasIntegerRule::Exact, static_cast<EDasFloatRule>(Rule), Out, Error))
        *static_cast<float*>(RESULT_PARAM) = Out.FloatValue;
    else
        NumericFailure(P_THIS, Stack, Error);
    P_NATIVE_END;
}
int32 UDasNumericLibrary::FloatToInt(float Value, EDasIntegerRule Rule)
{
    checkNoEntry();
    return 0;
}
DEFINE_FUNCTION(UDasNumericLibrary::execFloatToInt)
{
    P_GET_PROPERTY(FFloatProperty, Value);
    P_GET_ENUM(EDasIntegerRule, Rule);
    P_FINISH;
    P_NATIVE_BEGIN;
    FDasScalarValue V, Out;
    FString Error;
    V.Kind = 1;
    V.FloatValue = Value;
    if (ConvertDasNumber(V, 0, static_cast<EDasIntegerRule>(Rule), EDasFloatRule::Exact, Out, Error))
        *static_cast<int32*>(RESULT_PARAM) = Out.IntValue;
    else
        NumericFailure(P_THIS, Stack, Error);
    P_NATIVE_END;
}
int32 UDasNumericLibrary::DoubleToInt(double Value, EDasIntegerRule Rule)
{
    checkNoEntry();
    return 0;
}
DEFINE_FUNCTION(UDasNumericLibrary::execDoubleToInt)
{
    P_GET_PROPERTY(FDoubleProperty, Value);
    P_GET_ENUM(EDasIntegerRule, Rule);
    P_FINISH;
    P_NATIVE_BEGIN;
    FDasScalarValue V, Out;
    FString Error;
    V.Kind = 6;
    V.DoubleValue = Value;
    if (ConvertDasNumber(V, 0, static_cast<EDasIntegerRule>(Rule), EDasFloatRule::Exact, Out, Error))
        *static_cast<int32*>(RESULT_PARAM) = Out.IntValue;
    else
        NumericFailure(P_THIS, Stack, Error);
    P_NATIVE_END;
}
