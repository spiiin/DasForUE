#include "DasAsyncPhysicsVMCommandlet.h"
#include "DasAsyncPhysicsVM.h"
#include "DasAsyncPhysicsBridge.h"
#include "DasCollisionProfileProbe.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include <limits>
UDasAsyncPhysicsVMCommandlet::UDasAsyncPhysicsVMCommandlet(){IsClient=false;IsServer=false;IsEditor=true;LogToConsole=true;}
int32 UDasAsyncPhysicsVMCommandlet::Main(const FString& Params){
 FString Path;if(!FParse::Value(*Params,TEXT("Probe="),Path))return 1;
 auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 int32 Failures=0,Checks=0;auto Check=[&](bool Good,const TCHAR* Name){++Checks;UE_LOG(LogTemp,Display,TEXT("DAS_PHYSICS_VM_%s %s"),Good?TEXT("PASS"):TEXT("FAIL"),Name);if(!Good)++Failures;};
 auto* Settings=GetMutableDefault<UPhysicsSettings>();const bool OldAsync=Settings->bTickPhysicsAsync;const float OldDt=Settings->AsyncFixedTimeStepSize;
 Settings->bTickPhysicsAsync=FParse::Param(*Params,TEXT("AsyncFixed"));Settings->AsyncFixedTimeStepSize=1.f/60.f;ON_SCOPE_EXIT{Settings->bTickPhysicsAsync=OldAsync;Settings->AsyncFixedTimeStepSize=OldDt;};
 const auto IVS=UWorld::InitializationValues().CreatePhysicsScene(true).ShouldSimulatePhysics(true).AllowAudioPlayback(false).CreateNavigation(false).CreateAISystem(false);
 TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game,false,NAME_None,nullptr,true,ERHIFeatureLevel::Num,&IVS));
 GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());World->InitializeActorsForPlay(FURL());World->BeginPlay();World->GetWorldSettings()->NotifyBeginPlay();
 ON_SCOPE_EXIT{UDasAsyncPhysicsBridge::RevokeAll();if(World){GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);}};
 auto Owner=[&](){auto* A=World->SpawnActor<ADasCollisionProfileProbe>();if(!A->HasActorBegunPlay())A->DispatchBeginPlay();return A;};
 auto Step=[&](int32 N){for(int32 I=0;I<N;++I){++GFrameCounter;World->Tick(LEVELTICK_All,1.f/60.f);}};
 auto* Actor=Owner();FString Error;FDasPhysicsOutput Output;Output.StepSequence=999;
 auto VM=CreateDasAsyncPhysicsVM(Path,Actor,Error);Check(VM.IsValid(),TEXT("compile_initialize_before_binding"));if(!VM){UE_LOG(LogTemp,Error,TEXT("%s"),*Error);return 1;}
 Check(!VM->Read(Output) && Output.StepSequence==999,TEXT("no_output_before_first_step_and_out_unchanged"));
 FDasPhysicsPacket Input;Input.Values[0]=3;Check(VM->Publish(Input,Error),TEXT("publish_first_packet"));Input.Values[0]=7;Check(VM->Publish(Input,Error),TEXT("latest_input_replaces_whole_packet"));Step(5);
 Check(VM->Read(Output) && Output.Packet.Values[0]==14 && Output.InputSequence==2 && Output.StepSequence>=2 && Output.Packet.Values[1]==double(Output.StepSequence),TEXT("latest_packet_and_persistent_isolated_state"));
 Check(Output.Packet.Values[2]>0 && FMath::IsFinite(Output.Packet.Values[3]) && Output.Packet.Values[4]==0,TEXT("engine_time_and_zero_unwritten_slots"));
 const auto Gameplay=Runtime.RunPersistent(Path,0,Actor);Check(Gameplay.bSucceeded && Gameplay.Value==0,TEXT("gameplay_globals_not_shared_with_physics_VM"));
 Input.Values[0]=std::numeric_limits<double>::infinity();Check(!VM->Publish(Input,Error),TEXT("reject_nonfinite_input"));Step(3);Check(VM->Read(Output) && Output.Packet.Values[0]==14 && Output.InputSequence==2,TEXT("failed_publish_is_atomic"));Input.Values[0]=7;
 // Physics VM source is not opened again after preparation.
 FString Original;FFileHelper::LoadFileToString(Original,*Path);ON_SCOPE_EXIT{FFileHelper::SaveStringToFile(Original,*Path);};
 FFileHelper::SaveStringToFile(TEXT("invalid source while prepared VM is ticking"),*Path);const auto Before=Output.StepSequence;Step(3);Check(VM->Read(Output) && Output.StepSequence>Before,TEXT("no_compile_or_source_lookup_during_tick"));
 auto Rejected=CreateDasAsyncPhysicsVM(Path,Actor,Error);Check(!Rejected && VM->IsLive(),TEXT("rejected_candidate_preserves_current_binding"));FFileHelper::SaveStringToFile(Original,*Path);
 auto Other=CreateDasAsyncPhysicsVM(Path,Actor,Error);Step(3);FDasPhysicsOutput OtherOut;
 Check(Other && Other->Read(OtherOut) && OtherOut.InputSequence==0 && OtherOut.Packet.Values[0]==0 && OtherOut.StepSequence<Output.StepSequence+4,TEXT("second_VM_has_independent_mailbox_and_globals"));if(Other)Other->Revoke();
 bool Inside=false;auto* Peer=UDasAsyncPhysicsBridge::Create(Actor,[&](const auto&,FString&){if(!Inside){Inside=true;FDasPhysicsOutput Unchanged;Unchanged.StepSequence=777;Check(!VM->Read(Unchanged) && Unchanged.StepSequence==777 && !VM->Publish(Input,Error),TEXT("gameplay_mailbox_access_forbidden_inside_physics"));auto Invalid=CreateDasAsyncPhysicsVM(Path,Actor,Error);Check(!Invalid,TEXT("cannot_prepare_during_physics"));}return true;},Error);Step(3);Check(Inside,TEXT("peer_callback_exercised"));if(Peer)Peer->Revoke();
 // Signature and initialization validation are performed before enabling tick.
 const FString Candidate=Path+TEXT(".candidate.das");
 FFileHelper::SaveStringToFile(TEXT("options gen2\n[export]\ndef physics_tick(x:int):int {return x}\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error) && VM->IsLive(),TEXT("wrong_signature_rejected_without_replacing_live_VM"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\nvar forbidden=ue_add(1,2)\n[export]\ndef physics_tick(dt:float;time:float) { forbidden+=1 }\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error) && VM->IsLive(),TEXT("UE_extern_forbidden_in_global_initializer"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\noptions stack=0\n[export]\ndef physics_tick(dt:float;time:float) {}\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error) && Error.Contains(TEXT("private stack")),TEXT("shared_stack_option_rejected"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\noptions stack=131072\n[export]\ndef physics_tick(dt:float;time:float) {}\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error) && Error.Contains(TEXT("private stack")),TEXT("oversized_stack_option_rejected"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\noptions no_global_heap=false\nvar data:array<int>\n[export]\ndef physics_tick(dt:float;time:float) { push(data,1) }\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error) && Error.Contains(TEXT("private no-heap")),TEXT("global_heap_source_override_rejected"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\nvar bad=ue_physics_input(0)\n[export]\ndef physics_tick(dt:float;time:float) { bad+=1.0lf }\n"),*Candidate);
 Check(!CreateDasAsyncPhysicsVM(Candidate,Actor,Error) && VM->IsLive(),TEXT("physics_IO_forbidden_during_initialization"));
 FFileHelper::SaveStringToFile(TEXT("options gen2\nrequire ue_foundation\n[export]\ndef main(x:int):int { return int(ue_physics_input(x)) }\n"),*Candidate);
 const auto Outside=Runtime.RunFile(Candidate,0,false,Actor);Check(!Outside.bSucceeded && Outside.Error.Contains(TEXT("isolated VM step")),TEXT("physics_IO_not_callable_from_gameplay"));
 VM->Revoke();const auto Sentinel=Output.StepSequence;Check(!VM->IsLive() && !VM->Read(Output) && Output.StepSequence==Sentinel && !VM->Publish(Input,Error),TEXT("revoke_invalidates_external_handle"));
 for(int32 Mode=1;Mode<=4;++Mode){auto Failure=CreateDasAsyncPhysicsVM(Path,Actor,Error);Input.Values[15]=Mode;Input.Values[0]=123;const bool Published=Failure && Failure->Publish(Input,Error);Step(4);FDasPhysicsOutput NoPartial;NoPartial.StepSequence=888;
  const auto Expected=Mode==1?TEXT("slot"):Mode==2?TEXT("finite"):Mode==3?TEXT("UE extern is forbidden"):TEXT("transaction rejected");
  Check(Published && !Failure->IsLive() && !Failure->Read(NoPartial) && NoPartial.StepSequence==888 && Failure->LastError().Contains(Expected),*FString::Printf(TEXT("failure_revokes_no_partial_output_%d"),Mode));UDasAsyncPhysicsBridge::FlushDeferred();}
 FFileHelper::SaveStringToFile(TEXT("options gen2\noptions heap_size_limit=0\nrequire ue_foundation\n[export]\ndef physics_tick(dt:float;time:float) { var data:array<int>; resize(data,600000); ue_physics_output(0,double(length(data))) }\n"),*Candidate);
 auto HeapBound=CreateDasAsyncPhysicsVM(Candidate,Actor,Error);Check(HeapBound.IsValid(),TEXT("prepare_heap_limit_probe"));Step(4);Check(HeapBound && !HeapBound->IsLive() && !HeapBound->LastError().IsEmpty(),TEXT("host_heap_cap_overrides_unlimited_source_option"));UDasAsyncPhysicsBridge::FlushDeferred();
 Input.Values[15]=0;
 auto Released=CreateDasAsyncPhysicsVM(Path,Actor,Error);Runtime.ReleaseObject(Actor);Check(Released && !Released->IsLive(),TEXT("ReleaseObject_revokes_physics_VM"));
 auto* Dying=Owner();auto Death=CreateDasAsyncPhysicsVM(Path,Dying,Error);TWeakObjectPtr<UObject> WeakOwner=Dying;Dying->Destroy();Dying=nullptr;Step(3);CollectGarbage(RF_NoFlags);Check(Death && !Death->IsLive() && !WeakOwner.IsValid(),TEXT("handle_does_not_retain_owner_or_live_VM_after_Destroy_GC"));
 auto* ReloadActor=Owner();const auto Initial=Runtime.RunPersistent(Path,0,ReloadActor);auto Reload=CreateDasAsyncPhysicsVM(Path,ReloadActor,Error);
 FFileHelper::SaveStringToFile(Original+TEXT("\n// accepted gameplay revision\n"),*Path);const auto Accepted=Runtime.RunPersistent(Path,0,ReloadActor);Check(Initial.bSucceeded && Accepted.bSucceeded && Accepted.Revision>Initial.Revision && Reload && !Reload->IsLive(),TEXT("accepted_gameplay_reload_revokes_prepared_VM"));FFileHelper::SaveStringToFile(Original,*Path);
 auto WorldBound=CreateDasAsyncPhysicsVM(Path,Actor,Error);GEngine->DestroyWorldContext(World.Get());World->DestroyWorld(false);World.Reset();Check(WorldBound && !WorldBound->IsLive(),TEXT("world_cleanup_revokes_physics_VM"));
 UE_LOG(LogTemp,Display,TEXT("DAS_PHYSICS_VM_RESULT=%s checks=%d"),Failures?TEXT("FAIL"):TEXT("PASS"),Checks);return Failures?1:0;
}
