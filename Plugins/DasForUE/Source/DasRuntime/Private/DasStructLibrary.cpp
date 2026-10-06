#include "DasStructLibrary.h"
#include "DasSchema.h"
#include "UObject/UnrealType.h"
FInstancedStruct UDasStructLibrary::PackValue(const int32&){checkNoEntry();return {};}
void UDasStructLibrary::UnpackValue(const FInstancedStruct&,int32&){checkNoEntry();}
DEFINE_FUNCTION(UDasStructLibrary::execPackValue){
    Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FStructProperty>(nullptr);auto* P=CastField<FStructProperty>(Stack.MostRecentProperty);void* Memory=Stack.MostRecentPropertyAddress;P_FINISH;
    P_NATIVE_BEGIN;
    auto& Result=*static_cast<FInstancedStruct*>(RESULT_PARAM);FString Error;
    if(P && Memory && ValidateDasValueStruct(P->Struct->GetPathName(),Error))Result.InitializeAs(P->Struct,static_cast<const uint8*>(Memory));else Result.Reset();
    P_NATIVE_END;
}
DEFINE_FUNCTION(UDasStructLibrary::execUnpackValue){
    P_GET_STRUCT(FInstancedStruct,Data);Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FStructProperty>(nullptr);auto* P=CastField<FStructProperty>(Stack.MostRecentProperty);void* Memory=Stack.MostRecentPropertyAddress;P_FINISH;
    P_NATIVE_BEGIN;
    if(P && Memory){if(Data.IsValid() && Data.GetScriptStruct()==P->Struct)P->Struct->CopyScriptStruct(Memory,Data.GetMemory());else P->Struct->ClearScriptStruct(Memory);}
    P_NATIVE_END;
}
