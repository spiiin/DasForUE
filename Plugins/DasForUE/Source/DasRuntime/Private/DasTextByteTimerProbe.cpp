#include "DasStaticLibraryProbe.h"
#include "DasRuntime.h"
#include "DasTypeRegistry.h"
#include "DasCallFrame.h"
#include "UObject/GarbageCollection.h"
#include "DasProbeActor.h"
#include "Engine/World.h"
#include "GameFramework/DamageType.h"
#include "EngineUtils.h"
#include "Modules/ModuleManager.h"
#include "UObject/TextProperty.h"
#include <initializer_list>
namespace {
int32 TimerInt(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
bool TimerBool(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FBoolProperty>(O->GetClass(),N):nullptr;return P && P->GetPropertyValue_InContainer(O);}
void LogCallbackReadiness(UObject* Owner,const TCHAR* Label,std::initializer_list<const TCHAR*> Fields){
 FString Values;for(const auto* Field:Fields)Values+=FString::Printf(TEXT(" %s=%d"),Field,TimerInt(Owner,Field));
 auto* Host=Cast<ADasProbeActor>(Owner);
 UE_LOG(LogTemp,Display,TEXT("Callback readiness %s: owner=%s ready=%d timers=%d%s error=%s"),Label,*GetNameSafe(Owner),TimerBool(Owner,TEXT("Ready")),FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime")).ScriptTimerCount(Owner),*Values,Host?*Host->LastDasError:TEXT("missing host"));
}
}
bool DasTextByteTimersReady(UWorld* World){
 if(!World)return false;
 for(TActorIterator<ADasStaticLibraryProbe> It(World);It;++It)if(auto* A=It->TextByteTimerActor.Get()){
  if(TimerBool(A,TEXT("Ready")) && TimerBool(It->ContainerCallbackActor.Get(),TEXT("Ready")))return true;
  static double LastDetail=0;const double DetailNow=FPlatformTime::Seconds();if(DetailNow-LastDetail>5){LastDetail=DetailNow;
   LogCallbackReadiness(A,TEXT("scalar"),{TEXT("StateRuns"),TEXT("RecordRuns"),TEXT("ValueNext"),TEXT("ValueEvents"),TEXT("FloatRuns"),TEXT("BoolRuns"),TEXT("NameRuns"),TEXT("ArrayNext"),TEXT("ByteCalls"),TEXT("ByteRuns"),TEXT("TextRuns"),TEXT("BytesRuns"),TEXT("TextsRuns"),TEXT("BytesNext"),TEXT("TextsNext"),TEXT("PausedCalls"),TEXT("InvariantCalls"),TEXT("Errors")});
   LogCallbackReadiness(It->ContainerCallbackActor.Get(),TEXT("container"),{TEXT("PreservedRuns"),TEXT("RejectedReplacements"),TEXT("ShapesNext"),TEXT("ShapesEvents"),TEXT("MixedRuns"),TEXT("MixedNext"),TEXT("MixedEvents"),TEXT("MixedUnexpected"),TEXT("Next"),TEXT("Events"),TEXT("NativeStatesRuns"),TEXT("StatesRuns"),TEXT("RecordsRuns"),TEXT("MathRuns"),TEXT("RecordMapRuns"),TEXT("TextsRuns"),TEXT("StatesSetRuns"),TEXT("IntsRuns"),TEXT("Errors")});
  }
  return false;
 }
 return false;
}

bool RunDasTextByteTimerProbe(ADasStaticLibraryProbe* Carrier,FString& Report){
 bool Good=true;auto Check=[&](bool V,const TCHAR* N){Good &= V;Report+=FString(V?TEXT("PASS "):TEXT("FAIL "))+N+TEXT("\n");};auto* A=Carrier->TextByteTimerActor.Get();if(!A){Check(false,TEXT("text_byte_timer_saved_actor"));return false;}auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
 Check(TimerBool(A,TEXT("Ready")) && TimerInt(A,TEXT("Errors"))==0 && TimerInt(A,TEXT("Unexpected"))==0,TEXT("text_byte_timer_all_payloads_and_snapshot_isolation"));
 Check(TimerInt(A,TEXT("ByteCalls"))==2 && TimerInt(A,TEXT("ByteSum"))==255,TEXT("text_byte_timer_byte_boundaries"));
 Check(TimerInt(A,TEXT("ByteRuns"))==3 && TimerInt(A,TEXT("TextRuns"))==3 && TimerInt(A,TEXT("BytesRuns"))==3 && TimerInt(A,TEXT("TextsRuns"))==3,TEXT("text_byte_timer_repeated_owned_delivery"));
 Check(TimerInt(A,TEXT("FloatRuns"))==3 && TimerInt(A,TEXT("BoolRuns"))==3 && TimerInt(A,TEXT("NameRuns"))==3 && TimerInt(A,TEXT("ArrayNext"))==3,TEXT("all_primitive_timer_arrays_repeat_next_tick_owned_copy"));
 if(auto* Host=Cast<ADasProbeActor>(A)){Host->OnDasCallbackActor.Broadcast(A);Check(TimerInt(A,TEXT("NativeObjectEvents"))==1,TEXT("typed_native_actor_event_receiver"));FVector4 Record(1.25,2.,3.,4.);Host->OnDasCallbackEnumRef.Broadcast(EDasCallbackMode::Active);Host->OnDasCallbackRecordRef.Broadcast(Record);Check(Record.X==1.25,TEXT("native_const_ref_struct_source_unchanged"));}
 Check(TimerInt(A,TEXT("StateRuns"))==3 && TimerInt(A,TEXT("RecordRuns"))==3 && TimerInt(A,TEXT("ValueNext"))==4 && TimerInt(A,TEXT("ValueEvents"))==6 && TimerBool(A,TEXT("EventCopyOK")),TEXT("registered_enum_struct_native_and_authored_timers_events_owned_delivery"));
 auto* Text=FindFProperty<FTextProperty>(A->GetClass(),TEXT("LastText"));const auto Value=Text?Text->GetPropertyValue_InContainer(A):FText();const auto Namespace=FTextInspector::GetNamespace(Value),Key=FTextInspector::GetKey(Value);Check(Namespace.IsSet() && Namespace.GetValue()==TEXT("TimerCapture") && Key.IsSet() && Key.GetValue()==TEXT("Source"),TEXT("text_byte_timer_localization_history_retained"));
 Check(TimerBool(A,TEXT("InvalidRejected")) && TimerInt(A,TEXT("PausedCalls"))==1 && R.ScriptTimerCount(A)==0,TEXT("text_byte_timer_replace_reject_pause_cancel"));if(auto* Host=Cast<ADasProbeActor>(A))Check(Host->LastDasError.IsEmpty(),TEXT("text_byte_timer_no_runtime_error"));
 auto* Arm=A->FindFunction(TEXT("ArmPending"));if(Arm)A->ProcessEvent(Arm,nullptr);Check(Arm && R.ScriptTimerCount(A)==5,TEXT("text_byte_timer_pending_native_snapshots"));A->SetActorTickEnabled(false);R.ReleaseObject(A);Check(R.ScriptTimerCount(A)==0,TEXT("text_byte_timer_release_clears_pending_snapshots"));
 Check(R.SubscriptionCount(A)==0,TEXT("registered_value_event_release_detaches_listeners"));
 const int32 BeforeEvents=TimerInt(A,TEXT("ValueEvents"));if(auto* Host=Cast<ADasProbeActor>(A)){FVector4 Record(1.25,2.,3.,4.);Host->OnDasCallbackEnumRef.Broadcast(EDasCallbackMode::Idle);Host->OnDasCallbackRecordRef.Broadcast(Record);}
 Check(TimerInt(A,TEXT("ValueEvents"))==BeforeEvents && TimerInt(A,TEXT("Errors"))==0,TEXT("registered_value_native_broadcast_after_release_is_ignored"));
 FActorSpawnParameters Spawn;Spawn.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;auto* Released=Carrier->GetWorld()->SpawnActor<AActor>(A->GetClass(),FTransform::Identity,Spawn);Carrier->TextByteTimerReleasedActor=Released;Check(Released && R.ScriptTimerCount(Released)>0,TEXT("text_byte_timer_new_owner_pending"));if(Released){Released->SetActorTickEnabled(false);R.ReleaseObject(Released);Check(R.ScriptTimerCount(Released)==0 && R.SubscriptionCount(Released)==0,TEXT("text_byte_timer_new_owner_release"));}
 auto* ObjectOwner=Carrier->GetWorld()->SpawnActor<AActor>(A->GetClass(),FTransform::Identity,Spawn);Carrier->ObjectCallbackActor=ObjectOwner;
 FDasScalarValue Captured;Captured.Kind=8;Captured.ObjectClass=TEXT("/Script/CoreUObject.Object");Captured.ObjectValue=NewObject<UDasTypeRegistry>();
 if(ObjectOwner){auto Result=R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_text_byte_timers.das")),TEXT("ue_TextByteTimerActor_ArmObjects"),{Captured},EDasMethodSignature::Void,ObjectOwner);Check(Result.bSucceeded,TEXT("object_callback_timers_and_nested_event_armed"));}
 auto* DestroyedOwner=Carrier->GetWorld()->SpawnActor<AActor>(A->GetClass(),FTransform::Identity,Spawn);Carrier->DestroyedObjectCallbackActor=DestroyedOwner;
 auto* Victim=Carrier->GetWorld()->SpawnActor<AActor>();Captured.ObjectValue=Victim;
 if(DestroyedOwner && Victim){auto Result=R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_text_byte_timers.das")),TEXT("ue_TextByteTimerActor_ArmObjects"),{Captured},EDasMethodSignature::Void,DestroyedOwner);Check(Result.bSucceeded,TEXT("destroyed_actor_callback_captures_armed"));Victim->Destroy();}
 Captured={};CollectGarbage(RF_NoFlags);
 Check(ObjectOwner && TimerInt(ObjectOwner,TEXT("ReferenceEvents"))==1 && TimerInt(ObjectOwner,TEXT("ObjectEvents"))==1,TEXT("typed_object_and_nested_reference_events_delivered"));
 auto* Container=Carrier->ContainerCallbackActor.Get();Check(Container && TimerBool(Container,TEXT("Ready")) && TimerInt(Container,TEXT("Errors"))==0,TEXT("container_callbacks_enum_struct_math_arrays_maps_sets_owned_delivery"));
 if(auto* Host=Cast<ADasProbeActor>(Container)){TArray<AActor*> NativeActors{Container};Host->OnDasCallbackActors.Broadcast(NativeActors);Check(NativeActors.Num()==1 && NativeActors[0]==Container && TimerInt(Container,TEXT("NativeEvents"))==1,TEXT("native_object_array_callback_const_ref_copy"));}
 if(auto* Host=Cast<ADasProbeActor>(Container)){Host->OnTakeAnyDamage.Broadcast(Host,12.5f,GetDefault<UDamageType>(),nullptr,Host);Check(TimerInt(Host,TEXT("NativeDamageCalls"))==1 && TimerInt(Host,TEXT("Errors"))==0,TEXT("multi_native_damage_event_typed_object_upcast"));}
 Check(TimerInt(Container,TEXT("MixedRuns"))==3 && TimerInt(Container,TEXT("MixedNext"))==1 && TimerInt(Container,TEXT("MixedEvents"))==1 && TimerInt(Container,TEXT("MixedUnexpected"))==0,TEXT("multi_callbacks_mixed_timers_events_owned_snapshot"));
 Check(TimerInt(Container,TEXT("ShapesNext"))==1 && TimerInt(Container,TEXT("ShapesEvents"))==1 && TimerInt(Container,TEXT("Errors"))==0,TEXT("multi_callbacks_math_name_map_set_and_FText_identity"));
 Check(TimerInt(Container,TEXT("PreservedRuns"))==1 && TimerInt(Container,TEXT("RejectedReplacements"))==1 && TimerInt(Container,TEXT("MixedUnexpected"))==0,TEXT("multi_callbacks_rejected_replacement_preserves_previous"));

 Captured.Kind=8;Captured.ObjectClass=TEXT("/Script/CoreUObject.Object");Captured.ObjectValue=NewObject<UDasTypeRegistry>();
 if(Container){auto Result=R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_callback_containers.das")),TEXT("ue_ContainerCallbackActor_ArmReferences"),{Captured},EDasMethodSignature::Void,Container);Check(Result.bSucceeded,TEXT("container_reference_callbacks_armed"));}
 auto* ContainerDestroyed=Container?Carrier->GetWorld()->SpawnActor<AActor>(Container->GetClass(),FTransform::Identity,Spawn):nullptr;Carrier->DestroyedContainerCallbackActor=ContainerDestroyed;
 auto* ContainerVictim=Carrier->GetWorld()->SpawnActor<AActor>();Captured.ObjectValue=ContainerVictim;
 if(ContainerDestroyed && ContainerVictim){auto Result=R.RunNamedFramePersistent(R.ScriptPath(TEXT("dsl_callback_containers.das")),TEXT("ue_ContainerCallbackActor_ArmReferences"),{Captured},EDasMethodSignature::Void,ContainerDestroyed);Check(Result.bSucceeded,TEXT("container_destroyed_reference_callbacks_armed"));ContainerVictim->Destroy();}
 Captured={};CollectGarbage(RF_NoFlags);
 return Good;
}
bool CheckDasTextByteTimerCleanup(UWorld* World){if(!World)return false;auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));for(TActorIterator<ADasStaticLibraryProbe> It(World);It;++It){auto* A=It->TextByteTimerActor.Get();auto* B=It->TextByteTimerReleasedActor.Get();if(A && B){auto* C=It->ObjectCallbackActor.Get();auto* D=It->DestroyedObjectCallbackActor.Get();const bool DestructionClean=D && TimerInt(D,TEXT("ObjectNext"))==0 && TimerInt(D,TEXT("ReferenceNext"))==0 && TimerInt(D,TEXT("ObjectRuns"))==0 && TimerInt(D,TEXT("ReferenceRuns"))==0 && TimerInt(D,TEXT("Errors"))==0 && R.ScriptTimerCount(D)==0;const bool ObjectClean=DestructionClean && C && TimerInt(C,TEXT("ObjectNext"))==1 && TimerInt(C,TEXT("ReferenceNext"))==1 && TimerInt(C,TEXT("ObjectRuns"))==3 && TimerInt(C,TEXT("ReferenceRuns"))==3 && TimerInt(C,TEXT("Errors"))==0 && TimerInt(C,TEXT("CancelledObjectRuns"))==0;auto* E=It->ContainerCallbackActor.Get();auto* F=It->DestroyedContainerCallbackActor.Get();const bool ContainerClean=E && F && TimerInt(E,TEXT("RefMixedRuns"))==3 && TimerInt(E,TEXT("RefMixedNext"))==1 && TimerInt(E,TEXT("RefArrays"))==3 && TimerInt(E,TEXT("RefMaps"))==3 && TimerInt(E,TEXT("RefNext"))==2 && TimerInt(E,TEXT("RefEvents"))==2 && TimerInt(E,TEXT("Errors"))==0 && R.ScriptTimerCount(E)==0 && TimerInt(F,TEXT("RefMixedRuns"))==0 && TimerInt(F,TEXT("RefMixedNext"))==0 && TimerInt(F,TEXT("RefArrays"))==0 && TimerInt(F,TEXT("RefMaps"))==0 && TimerInt(F,TEXT("RefNext"))==0 && TimerInt(F,TEXT("Errors"))==0 && R.ScriptTimerCount(F)==0;UE_LOG(LogTemp,Display,TEXT("Container callbacks: arrays=%d maps=%d next=%d events=%d errors=%d timers=%d; destroyed arrays=%d maps=%d next=%d errors=%d timers=%d"),TimerInt(E,TEXT("RefArrays")),TimerInt(E,TEXT("RefMaps")),TimerInt(E,TEXT("RefNext")),TimerInt(E,TEXT("RefEvents")),TimerInt(E,TEXT("Errors")),R.ScriptTimerCount(E),TimerInt(F,TEXT("RefArrays")),TimerInt(F,TEXT("RefMaps")),TimerInt(F,TEXT("RefNext")),TimerInt(F,TEXT("Errors")),R.ScriptTimerCount(F));const bool Clean=ContainerClean && ObjectClean && TimerInt(A,TEXT("Unexpected"))==0 && TimerInt(A,TEXT("ByteCalls"))==2 && TimerInt(B,TEXT("Unexpected"))==0 && TimerInt(B,TEXT("ByteCalls"))==0 && TimerInt(B,TEXT("ValueEvents"))==0 && TimerInt(B,TEXT("StateRuns"))==0 && TimerInt(B,TEXT("RecordRuns"))==0 && R.ScriptTimerCount(A)==0 && R.ScriptTimerCount(B)==0;if(!Clean)UE_LOG(LogTemp,Display,TEXT("Timer release state: A calls=%d unexpected=%d timers=%d tick=%d; B calls=%d unexpected=%d timers=%d tick=%d"),TimerInt(A,TEXT("ByteCalls")),TimerInt(A,TEXT("Unexpected")),R.ScriptTimerCount(A),A->IsActorTickEnabled(),TimerInt(B,TEXT("ByteCalls")),TimerInt(B,TEXT("Unexpected")),R.ScriptTimerCount(B),B->IsActorTickEnabled());for(auto* Owner:{C,D,E,F})if(Owner){UE_LOG(LogTemp,Display,TEXT("Object callback state: destroyed=%d object=%d record=%d errors=%d cancelled=%d timers=%d host=%s"),Owner==D,TimerInt(Owner,TEXT("ObjectRuns")),TimerInt(Owner,TEXT("ReferenceRuns")),TimerInt(Owner,TEXT("Errors")),TimerInt(Owner,TEXT("CancelledObjectRuns")),R.ScriptTimerCount(Owner),*CastChecked<ADasProbeActor>(Owner)->LastDasError);Owner->SetActorTickEnabled(false);R.ReleaseObject(Owner);Owner->Destroy();}return Clean;}}return false;}
