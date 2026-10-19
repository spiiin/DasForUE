#include "DasDelegateLibrary.h"
#include "DasDelegateSignatures.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
FDasScalarValue UDasDelegateLibrary::PackValue(const int32&, const FString&)
{
    checkNoEntry();
    return {};
}
void UDasDelegateLibrary::UnpackValue(const FDasScalarValue&, int32&)
{
    checkNoEntry();
}
DEFINE_FUNCTION(UDasDelegateLibrary::execPackValue)
{
    Stack.MostRecentProperty = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    auto* Property = CastField<FDelegateProperty>(Stack.MostRecentProperty);
    void* Memory = Stack.MostRecentPropertyAddress;
    P_GET_PROPERTY(FStrProperty, SignatureWire);
    P_FINISH;
    P_NATIVE_BEGIN;
    auto& Result = *static_cast<FDasScalarValue*>(RESULT_PARAM);
    Result.Kind = -1;
    FString Error;
    TStrongObjectPtr<UFunction> Signature(MakeDasSingleDelegateSignature(SignatureWire, Error));
    if (Property && Memory && Signature && Property->SignatureFunction &&
        Signature->IsSignatureCompatibleWith(Property->SignatureFunction))
    {
        Result.Kind = 16;
        Result.ObjectClass = SignatureWire;
        Result.DelegateValue = Property->GetPropertyValue(Memory);
    }
    else
    {
        FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,
                                     FText::FromString(TEXT("Delegate input signature mismatch")));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, Info);
    }
    P_NATIVE_END;
}
DEFINE_FUNCTION(UDasDelegateLibrary::execUnpackValue)
{
    P_GET_STRUCT(FDasScalarValue, Data);
    Stack.MostRecentProperty = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    auto* Property = CastField<FDelegateProperty>(Stack.MostRecentProperty);
    void* Memory = Stack.MostRecentPropertyAddress;
    P_FINISH;
    P_NATIVE_BEGIN;
    FString Error;
    TStrongObjectPtr<UFunction> Signature(MakeDasSingleDelegateSignature(Data.ObjectClass, Error));
    if (Data.Kind == 16 && Property && Memory && Signature && Property->SignatureFunction &&
        Signature->IsSignatureCompatibleWith(Property->SignatureFunction))
        Property->SetPropertyValue(Memory, Data.DelegateValue);
    else
    {
        FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,
                                     FText::FromString(TEXT("Delegate result signature mismatch")));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, Info);
    }
    P_NATIVE_END;
}
