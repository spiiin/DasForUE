#include "DasEnumLibrary.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
FDasScalarValue UDasEnumLibrary::PackValue(const int32&){checkNoEntry();return {};}
void UDasEnumLibrary::UnpackValue(const FDasScalarValue&,int32&){checkNoEntry();}
DEFINE_FUNCTION(UDasEnumLibrary::execPackValue){Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);auto* P=CastField<FByteProperty>(Stack.MostRecentProperty);void* M=Stack.MostRecentPropertyAddress;P_FINISH;P_NATIVE_BEGIN;auto& Result=*static_cast<FDasScalarValue*>(RESULT_PARAM);Result.Kind=-1;FString Error;if(P && P->Enum && M && ValidateDasEnumNumber(P->Enum->GetPathName(),P->GetPropertyValue(M),Error)){Result.Kind=13;Result.IntValue=P->GetPropertyValue(M);Result.ObjectClass=P->Enum->GetPathName();}P_NATIVE_END;}
DEFINE_FUNCTION(UDasEnumLibrary::execUnpackValue){P_GET_STRUCT(FDasScalarValue,Data);Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);auto* P=CastField<FByteProperty>(Stack.MostRecentProperty);void* M=Stack.MostRecentPropertyAddress;P_FINISH;P_NATIVE_BEGIN;FString Error;if(P && P->Enum && M && Data.Kind==13 && P->Enum->GetPathName()==Data.ObjectClass && ValidateDasEnumNumber(Data.ObjectClass,Data.IntValue,Error))P->SetPropertyValue(M,uint8(Data.IntValue));else {FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,FText::FromString(TEXT("Enum frame type/value mismatch")));FBlueprintCoreDelegates::ThrowScriptException(P_THIS,Stack,Info);}P_NATIVE_END;}
