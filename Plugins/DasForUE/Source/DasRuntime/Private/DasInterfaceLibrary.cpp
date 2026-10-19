#include "DasInterfaceLibrary.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
FDasScalarValue UDasInterfaceLibrary::PackValue(const int32&, int32)
{
    checkNoEntry();
    return {};
}
void UDasInterfaceLibrary::UnpackValue(const FDasScalarValue&, int32&)
{
    checkNoEntry();
}
DEFINE_FUNCTION(UDasInterfaceLibrary::execPackValue)
{
    Stack.MostRecentProperty = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    auto* P = CastField<FInterfaceProperty>(Stack.MostRecentProperty);
    void* Memory = Stack.MostRecentPropertyAddress;
    P_GET_PROPERTY(FIntProperty, Direction);
    P_FINISH;
    P_NATIVE_BEGIN;
    auto& Result = *static_cast<FDasScalarValue*>(RESULT_PARAM);
    Result.Kind = -1;
    Result.Direction = Direction;
    if (P && Memory)
    {
        auto* O = DasReferenceGet(P, Memory);
        if (DasReferenceMatches(O, P->InterfaceClass))
        {
            Result.Kind = 8;
            Result.ObjectClass = P->InterfaceClass->GetPathName();
            Result.ObjectValue = O;
        }
    }
    P_NATIVE_END;
}
DEFINE_FUNCTION(UDasInterfaceLibrary::execUnpackValue)
{
    P_GET_STRUCT(FDasScalarValue, Data);
    Stack.MostRecentProperty = nullptr;
    Stack.StepCompiledIn<FProperty>(nullptr);
    auto* P = CastField<FInterfaceProperty>(Stack.MostRecentProperty);
    void* Memory = Stack.MostRecentPropertyAddress;
    P_FINISH;
    P_NATIVE_BEGIN;
    if (!P || !Memory || Data.Kind != 8 || Data.ObjectClass != P->InterfaceClass->GetPathName() ||
        !DasReferenceSet(P, Memory, Data.ObjectValue.Get()))
    {
        FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,
                                     FText::FromString(TEXT("Interface frame identity/value mismatch")));
        FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, Info);
    }
    P_NATIVE_END;
}
