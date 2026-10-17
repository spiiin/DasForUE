#include "UObject/TextProperty.h"
#include "DasStaticLibraryProbe.h"
#include "DasRuntime.h"
#include "DasSchema.h"
#include "Engine/World.h"
#include "Modules/ModuleManager.h"
#include "UObject/StructOnScope.h"
#include "UObject/StrongObjectPtr.h"

bool RunDasTextByteProbe(ADasStaticLibraryProbe* Carrier,FString& Report){
 bool Good=true;auto Check=[&](bool Value,const TCHAR* Name){Good &= Value;Report+=FString(Value?TEXT("PASS "):TEXT("FAIL "))+Name+TEXT("\n");};
 UClass* Class=Carrier?Carrier->TextByteClass.Get():nullptr;if(!Class){Check(false,TEXT("authored_text_byte_cooked_class"));return false;}
 auto* Byte=FindFProperty<FByteProperty>(Class,TEXT("Priority"));auto* Text=FindFProperty<FTextProperty>(Class,TEXT("Title"));auto* Config=FindFProperty<FStructProperty>(Class,TEXT("Config"));
 Check(Byte && !Byte->Enum && Text && Config,TEXT("authored_byte_text_native_property_types"));if(!Byte || !Text || !Config)return false;
 auto* StructByte=CastField<FByteProperty>(FindDasStructField(Config->Struct,TEXT("Priority")));auto* StructText=CastField<FTextProperty>(FindDasStructField(Config->Struct,TEXT("Title")));
 Check(StructByte && !StructByte->Enum && StructText,TEXT("authored_byte_text_struct_leaf_types"));
 if(StructText){FInstancedStruct First,Same,Different;First.InitializeAs(Config->Struct);Same.InitializeAs(Config->Struct);Different.InitializeAs(Config->Struct);
 const FText One=FText::AsLocalizable_Advanced_LocText(TEXT("IdentityTest"),TEXT("First"),TEXT("Same display"));const FText Other=FText::AsLocalizable_Advanced_LocText(TEXT("IdentityTest"),TEXT("Other"),TEXT("Same display"));
 StructText->SetPropertyValue_InContainer(First.GetMutableMemory(),One);StructText->SetPropertyValue_InContainer(Same.GetMutableMemory(),One);StructText->SetPropertyValue_InContainer(Different.GetMutableMemory(),Other);
 Check(MatchesDasStructDefault(Config->Struct,First.GetMemory(),Same.GetMemory()) && !MatchesDasStructDefault(Config->Struct,First.GetMemory(),Different.GetMemory()),TEXT("authored_struct_text_default_identity_not_display"));}
 const FText Expected=FText::AsLocalizable_Advanced_LocText(TEXT("DasTextByte"),TEXT("Title"),TEXT("Example title"));
 Check(Byte->GetPropertyValue_InContainer(Class->GetDefaultObject())==255 && DasTextDefaultsEqual(Text->GetPropertyValue_InContainer(Class->GetDefaultObject()),Expected),TEXT("authored_byte_text_persisted_defaults"));
 FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* A=Carrier->GetWorld()->SpawnActor<AActor>(Class,FTransform::Identity,Spawn);if(!A){Check(false,TEXT("authored_byte_text_spawn"));return false;}TStrongObjectPtr<AActor> Keep(A);
 auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 for(int32 I=0;I<2;++I){const auto Result=R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_text_byte_calls.das")),TEXT("main"),{},EDasMethodSignature::Bool,A);Check(Result.bSucceeded && Result.BoolValue,TEXT("authored_byte_text_typed_accessors_struct_ref_out"));if(!Result.bSucceeded)Report+=Result.Error+TEXT("\n");R.ReleaseObject(A);}
 if(auto* F=A->FindFunction(TEXT("EchoText"))){FStructOnScope Args(F);auto* In=FindFProperty<FTextProperty>(F,TEXT("value"));auto* Out=CastField<FTextProperty>(F->GetReturnProperty());if(In && Out){In->SetPropertyValue_InContainer(Args.GetStructMemory(),Expected);A->ProcessEvent(F,Args.GetStructMemory());Check(DasTextDefaultsEqual(Out->GetPropertyValue_InContainer(Args.GetStructMemory()),Expected),TEXT("authored_text_Blueprint_ProcessEvent_identity"));}else Check(false,TEXT("authored_text_UFunction_pin_types"));}else Check(false,TEXT("authored_text_UFunction"));
 for(uint8 Value:{uint8(0),uint8(255)}){auto* F=A->FindFunction(TEXT("EchoByte"));FStructOnScope Args(F);auto* In=F?FindFProperty<FByteProperty>(F,TEXT("value")):nullptr;auto* Out=F?CastField<FByteProperty>(F->GetReturnProperty()):nullptr;if(In && Out && !In->Enum && !Out->Enum){In->SetPropertyValue_InContainer(Args.GetStructMemory(),Value);A->ProcessEvent(F,Args.GetStructMemory());Check(Out->GetPropertyValue_InContainer(Args.GetStructMemory())==Value,TEXT("authored_byte_Blueprint_boundary_roundtrip"));}else Check(false,TEXT("authored_byte_UFunction_pin_types"));}
 A->Destroy();R.ReleaseObject(A);return RunDasTextByteContainerProbe(Carrier,Report) && Good;
}
