#include "DasStructLibrary.h"
#include "DasSchema.h"
#include "DasContainerTypes.h"
#include "UObject/UnrealType.h"
FInstancedStruct UDasStructLibrary::PackValue(const int32&){checkNoEntry();return {};}
void UDasStructLibrary::UnpackValue(const FInstancedStruct&,int32&){checkNoEntry();}
DEFINE_FUNCTION(UDasStructLibrary::execPackValue){
    Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);auto* P=Stack.MostRecentProperty;void* Memory=Stack.MostRecentPropertyAddress;P_FINISH;
    P_NATIVE_BEGIN;
    auto& Result=*static_cast<FInstancedStruct*>(RESULT_PARAM);FString Error;
    if(auto* Carrier=DasContainerCarrierForProperty(P);Carrier && Memory){Result.InitializeAs(Carrier);auto* Values=Carrier->FindPropertyByName(TEXT("Values"));Values->CopyCompleteValue(Values->ContainerPtrToValuePtr<void>(Result.GetMutableMemory()),Memory);if(!ValidateDasStructSnapshot(Result,Error))Result.Reset();}else if(auto* S=CastField<FStructProperty>(P);S && Memory && ValidateDasValueStruct(S->Struct->GetPathName(),Error))Result.InitializeAs(S->Struct,static_cast<const uint8*>(Memory));else Result.Reset();
    P_NATIVE_END;
}
DEFINE_FUNCTION(UDasStructLibrary::execUnpackValue){
    P_GET_STRUCT(FInstancedStruct,Data);Stack.MostRecentProperty=nullptr;Stack.StepCompiledIn<FProperty>(nullptr);auto* P=Stack.MostRecentProperty;void* Memory=Stack.MostRecentPropertyAddress;P_FINISH;
    P_NATIVE_BEGIN;
    if(P && Memory){if(auto* Carrier=DasContainerCarrierForProperty(P)){auto* Values=Carrier->FindPropertyByName(TEXT("Values"));FString Error;if(Data.IsValid() && Data.GetScriptStruct()==Carrier && ValidateDasStructSnapshot(Data,Error))P->CopyCompleteValue(Memory,Values->ContainerPtrToValuePtr<void>(Data.GetMemory()));else P->ClearValue(Memory);}else if(auto* S=CastField<FStructProperty>(P)){if(Data.IsValid() && Data.GetScriptStruct()==S->Struct)S->Struct->CopyScriptStruct(Memory,Data.GetMemory());else S->Struct->ClearScriptStruct(Memory);}}
    P_NATIVE_END;
}
