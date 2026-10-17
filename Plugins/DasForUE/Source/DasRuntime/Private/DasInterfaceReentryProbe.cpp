#include "DasStaticLibraryProbe.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "Modules/ModuleManager.h"
bool RunDasInterfaceReentryProbe(ADasStaticLibraryProbe* Carrier,FString& Report){
 bool Good=true;auto Check=[&](bool V,const TCHAR* N){Good &= V;Report+=FString(V?TEXT("PASS "):TEXT("FAIL "))+N+TEXT("\n");};auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 auto* World=Carrier->GetWorld();FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
 if(!Carrier->InterfaceReentryActorClass || !Carrier->InterfaceReentryComponentClass)return false;
 auto* A=World->SpawnActor<AActor>(Carrier->InterfaceReentryActorClass,FTransform::Identity,Spawn);auto* B=World->SpawnActor<AActor>(Carrier->InterfaceReentryActorClass,FTransform::Identity,Spawn);if(!A || !B)return false;TStrongObjectPtr<AActor> KeepA(A),KeepB(B);
 auto* Component=NewObject<UDasScriptComponent>(A,Carrier->InterfaceReentryComponentClass);A->AddInstanceComponent(Component);Component->RegisterComponent();TStrongObjectPtr<UDasScriptComponent> KeepC(Component);
 auto Target=[](UObject* O,UObject* V){auto* P=FindFProperty<FObjectProperty>(O->GetClass(),TEXT("Target"));if(P)P->SetObjectPropertyValue_InContainer(O,V);return P!=nullptr;};
 auto Bounce=[&](UObject* O,int32 N){return R.RunNamedPersistent(R.ScriptPath(TEXT("ue_interface_reentry_calls.das")),TEXT("probe_bounce"),N,O);};
 Check(Target(A,A),TEXT("interface_reentry_target_field"));auto Self=Bounce(A,6);Check(Self.bSucceeded && Self.Value==7,TEXT("interface_reentry_same_object_finite_recursion"));if(!Self.bSucceeded)Report+=Self.Error+TEXT("\n");
 Target(A,B);Target(B,A);auto Cross=Bounce(A,6);Check(Cross.bSucceeded && Cross.Value==7,TEXT("interface_reentry_two_actor_cycle_terminates"));if(!Cross.bSucceeded)Report+=Cross.Error+TEXT("\n");
 Target(A,Component);Target(Component,A);auto Mixed=Bounce(A,6);Check(Mixed.bSucceeded && Mixed.Value==7,TEXT("interface_reentry_actor_component_cycle"));if(!Mixed.bSucceeded)Report+=Mixed.Error+TEXT("\n");
 auto Limit=Bounce(A,-1);Check(!Limit.bSucceeded && Limit.Error.Contains(TEXT("recursion limit")),TEXT("interface_reentry_unbounded_cycle_rejected"));if(Limit.bSucceeded || !Limit.Error.Contains(TEXT("recursion limit")))Report+=Limit.Error+TEXT("\n");
 auto Failure=R.RunNamedVoidPersistent(R.ScriptPath(TEXT("ue_interface_reentry_calls.das")),TEXT("fault"),Component);Check(!Failure.bSucceeded && Failure.Error.Contains(TEXT("interface failure sentinel")),TEXT("interface_reentry_callee_error_propagates"));
 auto Recovered=R.RunNamedFramePersistent(R.ScriptPath(TEXT("ue_interface_reentry_calls.das")),TEXT("recover_fault"),{},EDasMethodSignature::Bool,Component);Check(Recovered.bSucceeded && Recovered.BoolValue,TEXT("interface_reentry_recover_blocks_partial_results"));if(!Recovered.bSucceeded)Report+=Recovered.Error+TEXT("\n");
 auto Legacy=R.RunNamedPersistent(R.ScriptPath(TEXT("ue_interface_reentry_calls.das")),TEXT("legacy_bounce"),2,A);Check(Legacy.bSucceeded && Legacy.Value==3,TEXT("interface_reentry_legacy_int_adapter"));
 auto Label=R.RunNamedFramePersistent(R.ScriptPath(TEXT("ue_interface_reentry_calls.das")),TEXT("legacy_label"),{},EDasMethodSignature::Bool,Component);Check(Label.bSucceeded && Label.BoolValue,TEXT("interface_reentry_legacy_string_adapter"));
 auto After=Bounce(A,2);Check(After.bSucceeded && After.Value==3,TEXT("interface_reentry_contexts_usable_after_failure"));
 // Generated receiver.Method syntax uses the same interface frame path.
 auto* Values=World->SpawnActor<AActor>(Carrier->TextByteEventsClass,FTransform::Identity,Spawn);if(!Values)return false;TStrongObjectPtr<AActor> KeepValues(Values);auto* Probe=Values->FindFunction(TEXT("ProbeInterface"));bool Result=false;if(Probe)Values->ProcessEvent(Probe,&Result);Check(Probe && Result,TEXT("interface_reentry_generated_same_context_text_byte_array_ref_out"));
 FDasScalarValue Arg;Arg.Kind=8;Arg.ObjectClass=TEXT("/Script/CoreUObject.Object");Arg.ObjectValue=Values;auto Typed=R.RunNamedFramePersistent(R.ScriptPath(TEXT("ue_interface_reentry_values.das")),TEXT("probe_values"),{Arg},EDasMethodSignature::Bool,A);Check(Typed.bSucceeded && Typed.BoolValue,TEXT("interface_reentry_generated_cross_context_text_byte_array_ref_out"));if(!Typed.bSucceeded)Report+=Typed.Error+TEXT("\n");
 for(auto* O:{static_cast<UObject*>(A),static_cast<UObject*>(B),static_cast<UObject*>(Component),static_cast<UObject*>(Values)})R.ReleaseObject(O);Component->DestroyComponent();A->Destroy();B->Destroy();Values->Destroy();return RunDasTextByteTimerProbe(Carrier,Report) && Good;
}
