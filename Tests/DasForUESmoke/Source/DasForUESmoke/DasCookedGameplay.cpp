#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "DasRuntime.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/SceneComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

namespace {
int32 Int(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):MIN_int32;}
bool Bool(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FBoolProperty>(O->GetClass(),N):nullptr;return P && P->GetPropertyValue_InContainer(O);}
UObject* Object(UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FObjectProperty>(O->GetClass(),N):nullptr;return P?P->GetObjectPropertyValue_InContainer(O):nullptr;}
struct FCookedGameplayState {
 TWeakObjectPtr<UWorld> World;TWeakObjectPtr<ADasProbeActor> First,Second,Native,Delegate,Single,Inherited,Life,TickOnly,Helper;
 TStrongObjectPtr<ADasProbeActor> Destroyed;
 double Started=FPlatformTime::Seconds();float StoppedAt=0;int32 Phase=0,Pulses=0,DestroyedPulses=0;bool Good=true;
 void Check(bool OK,const TCHAR* Name){Good &= OK;UE_LOG(LogTemp,Display,TEXT("DAS_COOKED_GAMEPLAY %s %s"),OK?TEXT("PASS"):TEXT("FAIL"),Name);}
 void Call(ADasProbeActor* O,const TCHAR* Name,int32 Value=0){auto* F=O?O->FindFunction(Name):nullptr;Check(F!=nullptr,Name);if(F)O->ProcessEvent(F,F->NumParms?&Value:nullptr);}
 bool Finish(){auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));for(auto* O:{First.Get(),Second.Get(),Native.Get(),Delegate.Get(),Single.Get(),Inherited.Get(),Life.Get(),TickOnly.Get(),Helper.Get()})if(O){R.ReleaseObject(O);O->Destroy();}Destroyed.Reset();Check(R.PersistentContextCount(World.Get())==0,TEXT("all_contexts_released"));UE_LOG(LogTemp,Display,TEXT("DAS_COOKED_GAMEPLAY_RESULT=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);return false;}
 bool Tick(float){
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));auto* W=World.Get();auto* A=First.Get();auto* B=Second.Get();
  if(FPlatformTime::Seconds()-Started>60){Check(false,TEXT("timeout"));if(A)UE_LOG(LogTemp,Error,TEXT("%s"),*A->LastDasError);return Finish();}
  if(Phase==0){if(!W || !W->HasBegunPlay() || !A || !B || Int(A,TEXT("Pulses"))<2 || Int(A,TEXT("Loads"))<1 || Int(A,TEXT("Delays"))<1)return true;
   Check(A->LastDasError.IsEmpty() && B->LastDasError.IsEmpty(),TEXT("runtime_without_editor_errors"));Check(Int(A,TEXT("SnapshotSum"))==7 && Int(A,TEXT("NextSum"))==13 && Bool(A,TEXT("LoadOK")) && Bool(A,TEXT("SceneOK")),TEXT("timer_owned_array_next_tick_latent_async_scene"));Check(Int(A,TEXT("CancelledCalls"))==0,TEXT("cancelled_callbacks_not_invoked"));
   A->EmitDasFloatSignal(2.5f);A->EmitDasVoidSignal();A->EmitDasStringSignal(TEXT("Cooked 世界"));A->EmitDasArrayIntSignal(TArray<int32>{11,12});A->EmitDasVector2DRefSignal(FVector2D(9,10));
   auto* F=FindFProperty<FFloatProperty>(A->GetClass(),TEXT("Floats"));auto* T=FindFProperty<FStrProperty>(A->GetClass(),TEXT("Text"));auto* V=FindFProperty<FStructProperty>(A->GetClass(),TEXT("Position"));Check(F && F->GetPropertyValue_InContainer(A)==2.5f && T && T->GetPropertyValue_InContainer(A)==TEXT("Cooked 世界") && Int(A,TEXT("Voids"))==1 && Int(A,TEXT("ArraySum"))==23 && V && *V->ContainerPtrToValuePtr<FVector2D>(A)==FVector2D(9,10),TEXT("scalar_string_array_constref_events"));Check(Int(B,TEXT("Voids"))==0 && Int(B,TEXT("ArraySum"))==0,TEXT("owner_isolation"));
   auto* N=Native.Get();auto* Pivot=Cast<USceneComponent>(Object(N,TEXT("Pivot")));auto* Child=Cast<USceneComponent>(Object(N,TEXT("Child")));auto* Grand=Cast<USceneComponent>(Object(N,TEXT("Grandchild")));auto* Behavior=Object(N,TEXT("Behavior"));auto* Other=Object(N,TEXT("OtherBehavior"));Check(N && Pivot && N->GetRootComponent()==Pivot,TEXT("cooked_native_root"));Check(Child && Grand && Child->GetAttachParent()==Pivot && Grand->GetAttachParent()==Child,TEXT("cooked_scene_attachment"));Check(Child && Child->GetRelativeLocation().Equals(FVector(0,0,50)) && Child->GetRelativeRotation().Equals(FRotator(0,30,0)) && Child->bHiddenInGame,TEXT("cooked_native_template_defaults"));Check(Int(Behavior,TEXT("Bonus"))==42 && Int(Other,TEXT("Bonus"))==10,TEXT("cooked_authored_template_defaults"));Check(Int(Behavior,TEXT("Begins"))==1 && Int(Behavior,TEXT("Ticks"))>0 && Int(Other,TEXT("Begins"))==1,TEXT("cooked_authored_component_lifecycle"));N->EmitDasSignal(7);Check(Int(Behavior,TEXT("Signals"))==7 && Int(Other,TEXT("Signals"))==7,TEXT("cooked_component_owner_events"));
   UE_LOG(LogTemp,Display,TEXT("COOKED_COMPONENT_DETAIL root=%s pivot=%s child=%s parent=%s loc=%s rot=%s hidden=%d bonus=%d/%d begins=%d/%d errors=%s/%s"),*GetNameSafe(N?N->GetRootComponent():nullptr),*GetNameSafe(Pivot),*GetNameSafe(Child),*GetNameSafe(Child?Child->GetAttachParent():nullptr),Child?*Child->GetRelativeLocation().ToString():TEXT("none"),Child?*Child->GetRelativeRotation().ToString():TEXT("none"),Child?Child->bHiddenInGame:false,Int(Behavior,TEXT("Bonus")),Int(Other,TEXT("Bonus")),Int(Behavior,TEXT("Begins")),Int(Other,TEXT("Begins")),Cast<UDasScriptComponent>(Behavior)?*CastChecked<UDasScriptComponent>(Behavior)->LastDasError:TEXT("none"),Cast<UDasScriptComponent>(Other)?*CastChecked<UDasScriptComponent>(Other)->LastDasError:TEXT("none"));
   auto* D=Delegate.Get();Call(D,TEXT("BindHandlers"));Call(D,TEXT("BindHandlers"));Call(D,TEXT("Emit"),4);Check(Int(D,TEXT("Score"))==12,TEXT("authored_dispatcher_two_unique_handlers"));Call(D,TEXT("UnbindSecond"));Call(D,TEXT("Emit"),1);Check(Int(D,TEXT("Score"))==13 && D->LastDasError.IsEmpty(),TEXT("authored_dispatcher_individual_remove"));
   auto* S=Single.Get();Check(S && FindFProperty<FDelegateProperty>(S->GetClass(),TEXT("Callback")),TEXT("cooked_single_delegate_property"));if(S){Call(S,TEXT("Bind"));auto InvokeSingle=[&](const TCHAR* N,int32 X){struct FArgs{int32 X,Result=0;};FArgs Args{X};auto* Fn=S->FindFunction(N);if(Fn)S->ProcessEvent(Fn,&Args);return Args.Result;};Check(InvokeSingle(TEXT("Invoke"),3)==3,TEXT("cooked_single_delegate_execute"));Call(S,TEXT("Clear"));Check(InvokeSingle(TEXT("InvokeCopy"),4)==7,TEXT("cooked_single_delegate_independent_copy"));const auto Ref=R.RunPersistent(R.ScriptPath(TEXT("single_delegate_reference_probe.das")),5,S);Check(Ref.bSucceeded && Ref.Value==12,TEXT("cooked_single_delegate_foreign_accessor"));}
   const auto DelegateMethods=R.RunNamedPersistent(R.ScriptPath(TEXT("single_delegate_reference_probe.das")),TEXT("method_probe"),2,Single.Get());Check(DelegateMethods.bSucceeded && DelegateMethods.Value==14,TEXT("single_delegate_method_parameter_and_result"));
   int32 DelegateRefValue=0;if(auto* Fn=Single->FindFunction(TEXT("ProbeRefOut")))Single->ProcessEvent(Fn,&DelegateRefValue);Check(DelegateRefValue==30 && Single->LastDasError.IsEmpty(),TEXT("delegate_ref_out_and_multicast_ref"));
   int32 ArrayRef=0;if(auto* Fn=Single->FindFunction(TEXT("ProbeRefArray")))Single->ProcessEvent(Fn,&ArrayRef);Check(ArrayRef==1,TEXT("delegate_array_ref_out"));
   struct FInheritanceArgs{int32 Input=2,Result=0;};FInheritanceArgs IA;auto* I=Inherited.Get();if(I)if(auto* Fn=I->FindFunction(TEXT("Probe")))I->ProcessEvent(Fn,&IA);Check(I && IA.Result==27 && I->LastDasError.IsEmpty(),TEXT("authored_inheritance_override_Super"));
   auto* L=Life.Get();TStrongObjectPtr<ADasProbeActor> KeepLife(L);TStrongObjectPtr<UObject> LifeBehavior(Object(L,TEXT("Behavior")));
   Check(L && Object(L,TEXT("Pivot"))==L->GetRootComponent() && Int(L,TEXT("BaseBegins"))==1 && Int(L,TEXT("ChildBegins"))==1 && Int(L,TEXT("BaseTicks"))>0,TEXT("inherited_actor_root_parent_BeginPlay_and_Tick"));
   Check(L && L->GetRootComponent() && L->GetRootComponent()->bHiddenInGame,TEXT("inherited_native_template_override"));
   Check(Int(LifeBehavior.Get(),TEXT("Bonus"))==55,TEXT("inherited_component_template_default"));
   Check(Int(LifeBehavior.Get(),TEXT("BaseBegins"))==1 && Int(LifeBehavior.Get(),TEXT("ChildBegins"))==1 && Int(LifeBehavior.Get(),TEXT("BaseTicks"))>0 && Int(LifeBehavior.Get(),TEXT("BaseTicks"))==Int(LifeBehavior.Get(),TEXT("ChildTicks")),TEXT("inherited_component_parent_lifecycle"));
   auto* Extension=Cast<USceneComponent>(Object(L,TEXT("Extension")));auto* Marker=Cast<USceneComponent>(Object(L,TEXT("Marker")));TStrongObjectPtr<UObject> ExtraBehavior(Object(L,TEXT("ExtraBehavior")));
   Check(L && Extension && Marker && Extension->GetAttachParent()==L->GetRootComponent() && Marker->GetAttachParent()==Extension && Extension->GetRelativeLocation().Equals(FVector(0,0,25)),TEXT("derived_scene_attachment_and_defaults"));
   Check(Int(ExtraBehavior.Get(),TEXT("Bonus"))==77 && Int(ExtraBehavior.Get(),TEXT("BaseBegins"))==1 && Int(ExtraBehavior.Get(),TEXT("ChildBegins"))==1,TEXT("derived_authored_component_defaults_and_lifecycle"));
   if(L)L->EmitDasSignal(4);
   Check(Int(L,TEXT("BaseSignals"))==4 && Int(L,TEXT("ChildSignals"))==40 && Int(LifeBehavior.Get(),TEXT("BaseSignals"))==4 && Int(LifeBehavior.Get(),TEXT("ChildSignals"))==40 && Int(ExtraBehavior.Get(),TEXT("BaseSignals"))==4 && Int(ExtraBehavior.Get(),TEXT("ChildSignals"))==40,TEXT("inherited_actor_component_events"));
   if(L)L->Destroy();
   Check(Int(ExtraBehavior.Get(),TEXT("BaseEnds"))==1 && Int(ExtraBehavior.Get(),TEXT("ChildEnds"))==1,TEXT("derived_component_EndPlay_once"));
   Check(Int(L,TEXT("BaseEnds"))==1 && Int(L,TEXT("ChildEnds"))==1 && Int(LifeBehavior.Get(),TEXT("BaseEnds"))==1 && Int(LifeBehavior.Get(),TEXT("ChildEnds"))==1,TEXT("inherited_actor_component_EndPlay_once"));
   if(TickOnly.IsValid())TickOnly->EmitDasSignal(3);
   Check(Int(TickOnly.Get(),TEXT("BaseSignals"))==3,TEXT("event_only_parent_initialization_without_BeginPlay_hook"));
   Check(TickOnly.IsValid() && Int(TickOnly.Get(),TEXT("Ticks"))>0 && TickOnly->LastDasError.IsEmpty(),TEXT("inherited_Tick_without_BeginPlay_or_parent_UFunction"));
   struct FHelperCookedArgs{int32 Input=2,Result=0;};FHelperCookedArgs HelperArgs;auto* H=Helper.Get();if(H)if(auto* Fn=H->FindFunction(TEXT("LocalProbe")))H->ProcessEvent(Fn,&HelperArgs);
   Check(H && HelperArgs.Result==21 && Int(H,TEXT("Score"))==12 && H->LastDasError.IsEmpty(),TEXT("three_level_local_helpers_self_override_Super"));
   Check(H && Int(H,TEXT("ConstructionValue"))==23 && Int(H,TEXT("ConstructionTrace"))==12,TEXT("construction_values_survive_cook"));
   auto* SpawnedHelper=H?W->SpawnActor<ADasProbeActor>(H->GetClass()):nullptr;
   Check(SpawnedHelper && Int(SpawnedHelper,TEXT("ConstructionValue"))==23 && Int(SpawnedHelper,TEXT("ConstructionTrace"))==12 && SpawnedHelper->LastDasError.IsEmpty(),TEXT("runtime_spawn_executes_construction_before_BeginPlay"));if(SpawnedHelper)SpawnedHelper->Destroy();
   Call(A,TEXT("Stop"));Pulses=Int(A,TEXT("Pulses"));Call(B,TEXT("ArmPending"));Destroyed.Reset(B);DestroyedPulses=Int(B,TEXT("Pulses"));B->Destroy();Check(R.SubscriptionCount(B)==0,TEXT("destroy_revokes_owner_subscriptions"));StoppedAt=W->GetTimeSeconds();Phase=1;return true;
  }
  if(W->GetTimeSeconds()-StoppedAt<0.2f)return true;Check(Int(A,TEXT("Pulses"))==Pulses && Int(Destroyed.Get(),TEXT("Pulses"))==DestroyedPulses && Int(Destroyed.Get(),TEXT("CancelledCalls"))==0,TEXT("timer_cancel_destroy_pending_cleanup"));return Finish();
 }
};
}
void StartDasCookedGameplayValidation(){auto S=MakeShared<FCookedGameplayState>();for(const auto& C:GEngine->GetWorldContexts())if(auto* W=C.World();W && W->IsGameWorld()){S->World=W;for(TActorIterator<ADasProbeActor> It(W);It;++It){auto* A=*It;if(A->ActorHasTag(TEXT("GameplayFirst")))S->First=A;if(A->ActorHasTag(TEXT("GameplaySecond")))S->Second=A;if(A->ActorHasTag(TEXT("NativeOwner")))S->Native=A;if(A->ActorHasTag(TEXT("DelegateOwner")))S->Delegate=A;if(A->ActorHasTag(TEXT("SingleOwner")))S->Single=A;if(A->ActorHasTag(TEXT("InheritedOwner")))S->Inherited=A;if(A->ActorHasTag(TEXT("LifeOwner")))S->Life=A;if(A->ActorHasTag(TEXT("TickOnlyOwner")))S->TickOnly=A;if(A->ActorHasTag(TEXT("HelperOwner")))S->Helper=A;}}FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float Delta){return S->Tick(Delta);}));}
