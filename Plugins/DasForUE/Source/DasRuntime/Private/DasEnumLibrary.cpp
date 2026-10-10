#include "DasEnumLibrary.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "UObject/Script.h"
FDasScalarValue UDasEnumLibrary::PackValue(const int32&,int32){checkNoEntry();return {};}
void UDasEnumLibrary::UnpackValue(const FDasScalarValue&,int32&){checkNoEntry();}
DEFINE_FUNCTION(UDasEnumLibrary::execPackValue){Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);auto* Property=Stack.MostRecentProperty;void* Memory=Stack.MostRecentPropertyAddress;P_GET_PROPERTY(FIntProperty,Direction);P_FINISH;P_NATIVE_BEGIN;auto& Result=*static_cast<FDasScalarValue*>(RESULT_PARAM);Result.Kind=-1;Result.Direction=Direction;auto* Enum=DasEnumPropertyType(Property);FString Error;if(Enum && Memory && ValidateDasEnumNumber(Enum->GetPathName(),DasEnumPropertyNumber(Property,Memory),Error)){Result.Kind=13;Result.IntValue=int32(DasEnumPropertyNumber(Property,Memory));Result.ObjectClass=Enum->GetPathName();}P_NATIVE_END;}
DEFINE_FUNCTION(UDasEnumLibrary::execUnpackValue){P_GET_STRUCT(FDasScalarValue,Data);Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);auto* Property=Stack.MostRecentProperty;void* Memory=Stack.MostRecentPropertyAddress;P_FINISH;P_NATIVE_BEGIN;auto* Enum=DasEnumPropertyType(Property);if(!Enum || Data.Kind!=13 || Enum->GetPathName()!=Data.ObjectClass || !SetDasEnumPropertyNumber(Property,Memory,Data.IntValue)){FBlueprintExceptionInfo Info(EBlueprintExceptionType::AbortExecution,FText::FromString(TEXT("Enum frame type/value mismatch")));FBlueprintCoreDelegates::ThrowScriptException(P_THIS,Stack,Info);}P_NATIVE_END;}
