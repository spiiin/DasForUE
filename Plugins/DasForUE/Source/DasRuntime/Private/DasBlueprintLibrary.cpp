#include "DasBlueprintLibrary.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "UObject/Script.h"
#include "Blueprint/BlueprintExceptionInfo.h"
namespace
{
FDasScalarValue ExecuteLibrary(const FString& Contract, const FString& File, const FString& Entry,
                               const TArray<FDasScalarValue>& Args, int32 Kind, const FString& ResultClass,
                               TArray<FDasScalarValue>* Outputs)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const auto V =
        Runtime.RunLibraryFrame(Contract, File, Entry, Args, static_cast<EDasMethodSignature>(Kind), ResultClass);
    if (Outputs)
        *Outputs = V.bSucceeded ? V.OutputValues : Args;
    if (!V.bSucceeded)
    {
        if (auto* Stack = FFrame::GetThreadLocalTopStackFrame())
        {
            FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution, FText::FromString(V.Error));
            FBlueprintCoreDelegates::ThrowScriptException(Stack->Object, *Stack, Info);
        }
        else
            UE_LOG(LogTemp, Warning, TEXT("Static library call rejected: %s"), *V.Error);
    }
    FDasScalarValue R;
    R.Kind = V.bSucceeded ? Kind : -1;
    R.IntValue = V.Value;
    R.FloatValue = V.FloatValue;
    R.BoolValue = V.BoolValue;
    R.StringValue = V.StringValue;
    R.DoubleValue = V.DoubleValue;
    R.NameValue = V.NameValue;
    R.ByteValue = V.ByteValue;
    R.TextValue = V.TextValue;
    R.ObjectClass = ResultClass;
    R.ObjectValue = V.ObjectValue.Get();
    R.ClassValue = Cast<UClass>(V.ObjectValue.Get());
    R.StructValue = V.StructValue;
    R.VectorValue = V.VectorValue;
    R.RotatorValue = V.RotatorValue;
    R.TransformValue = V.TransformValue;
    R.DelegateValue = V.DelegateValue;
    return R;
}
}
FDasScalarValue UDasBlueprintLibrary::ExecuteLibraryFrame(const FString& C, const FString& F, const FString& E,
                                                          const TArray<FDasScalarValue>& A, int32 K, const FString& R)
{
    return ExecuteLibrary(C, F, E, A, K, R, nullptr);
}
FDasScalarValue UDasBlueprintLibrary::ExecuteLibraryFrameOutputs(const FString& C, const FString& F, const FString& E,
                                                                 const TArray<FDasScalarValue>& A, int32 K,
                                                                 const FString& R, TArray<FDasScalarValue>& O)
{
    return ExecuteLibrary(C, F, E, A, K, R, &O);
}
