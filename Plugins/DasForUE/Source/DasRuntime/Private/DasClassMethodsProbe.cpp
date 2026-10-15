#include "DasClassMethodsProbe.h"
#include "DasRuntime.h"
#include "Modules/ModuleManager.h"
#include "DasProbeActor.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "UObject/StructOnScope.h"
#include "UObject/UnrealType.h"
bool RunDasClassMethodsProbe(UObject* Target,FString& Report){
 bool Good=true;auto Check=[&](bool OK,const TCHAR* N){Good &= OK;Report+=FString::Printf(TEXT("DAS_CLASS_METHODS %s %s\n"),OK?TEXT("PASS"):TEXT("FAIL"),N);};
 if(!Target)return false;
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 for(const auto* Name:{TEXT("EchoClass"),TEXT("EchoAnyClass")}){
  auto* F=Target->FindFunction(FName(Name));auto* Input=F?FindFProperty<FClassProperty>(F,TEXT("value")):nullptr;auto* Return=F?CastField<FClassProperty>(F->GetReturnProperty()):nullptr;
  auto* Base=FString(Name)==TEXT("EchoClass")?AActor::StaticClass():UObject::StaticClass();Check(Input && Return && Input->MetaClass==Base && Return->MetaClass==Base,Name);if(!Input || !Return)continue;
  for(UClass* Value:TArray<UClass*>{AActor::StaticClass(),Base==AActor::StaticClass() && !Target->IsA<AActor>()?ADasProbeActor::StaticClass():Target->GetClass(),nullptr}){FStructOnScope Frame(F);Input->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(),Value);Target->ProcessEvent(F,Frame.GetStructMemory());Check(Return->GetObjectPropertyValue_InContainer(Frame.GetStructMemory())==Value,TEXT("ProcessEvent_class_roundtrip_null_BP_subclass"));}
 }
 auto* F=Target->FindFunction(TEXT("ClassOutputs"));auto* Input=F?FindFProperty<FClassProperty>(F,TEXT("value")):nullptr;auto* Output=F?FindFProperty<FClassProperty>(F,TEXT("output")):nullptr;auto* Changed=F?FindFProperty<FClassProperty>(F,TEXT("changed")):nullptr;auto* Return=F?CastField<FClassProperty>(F->GetReturnProperty()):nullptr;
 Check(Input && Output && Changed && Return && Output->MetaClass==AActor::StaticClass() && Changed->MetaClass==AActor::StaticClass() && Output->HasAnyPropertyFlags(CPF_OutParm) && !Output->HasAnyPropertyFlags(CPF_ReferenceParm) && Changed->HasAllPropertyFlags(CPF_OutParm|CPF_ReferenceParm),TEXT("class_ref_out_signature"));
 if(Input && Output && Changed && Return){FStructOnScope Frame(F);Input->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(),AActor::StaticClass());Changed->SetObjectPropertyValue_InContainer(Frame.GetStructMemory(),AActor::StaticClass());Target->ProcessEvent(F,Frame.GetStructMemory());Check(Return->GetObjectPropertyValue_InContainer(Frame.GetStructMemory())==AActor::StaticClass() && Output->GetObjectPropertyValue_InContainer(Frame.GetStructMemory())==AActor::StaticClass() && !Changed->GetObjectPropertyValue_InContainer(Frame.GetStructMemory()),TEXT("ProcessEvent_class_return_out_ref"));}
 const FString Script=Target->IsA<AActor>()?TEXT("dsl_spawn_target.das"):TEXT("dsl_dynamic_lifecycle.das");const FString Prefix=Target->IsA<AActor>()?TEXT("ue_SpawnTarget_"):TEXT("ue_DynamicLife_");
 FDasScalarValue V;V.Kind=15;V.ObjectClass=TEXT("/Script/Engine.Actor");V.ClassValue=USceneComponent::StaticClass();
 auto Bad=Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),Prefix+TEXT("EchoClass"),{V},EDasMethodSignature::Class,Target,TEXT("/Script/Engine.Actor"));Check(!Bad.bSucceeded && Bad.Error.Contains(TEXT("Class argument")),TEXT("wrong_input_base_rejected"));
 auto Result=Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),Prefix+TEXT("BadClassResult"),{},EDasMethodSignature::Class,Target,TEXT("/Script/Engine.Actor"));Check(!Result.bSucceeded && Result.Error.Contains(TEXT("Class result")),TEXT("wrong_result_base_rejected"));
 V.Direction=1;V.ClassValue=AActor::StaticClass();auto Out=Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),Prefix+TEXT("BadClassOutput"),{V},EDasMethodSignature::Void,Target);Check(!Out.bSucceeded && Out.OutputValues.IsEmpty() && Out.Error.Contains(TEXT("output class base")),TEXT("wrong_output_base_rejected_atomically"));
 Report+=FString::Printf(TEXT("DAS_CLASS_METHODS_RESULT=%s\n"),Good?TEXT("PASS"):TEXT("FAIL"));return Good;
}
