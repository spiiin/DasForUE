#include "CoreMinimal.h"
#include "DasRuntime.h"
#include "DasEffectProxy.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"
void StartDasEffectsValidation(){
 struct FState {
  int32 Stage=0,PlayerBindings=0,PickupBindings=0,Before=0;uint32 Revision=0;double Started=FPlatformTime::Seconds(),Deadline=0;
  TStrongObjectPtr<AActor> Player,Pickup;
  TStrongObjectPtr<UDasEffectProxy> Old,Current,Hurt;
  TStrongObjectPtr<UAudioComponent> OldAudio,External;
  TStrongObjectPtr<UNiagaraComponent> OldNiagara;
  FString ReloadPath,Template;
  ~FState(){if(!ReloadPath.IsEmpty())IFileManager::Get().Delete(*ReloadPath);}
 };auto S=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float){
  auto Fail=[](const FString& E){UE_LOG(LogTemp,Error,TEXT("DAS_EFFECTS_RESULT=FAIL %s"),*E);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-S->Started>60)return Fail(TEXT("timeout stage ")+FString::FromInt(S->Stage));
  UWorld* W=nullptr;for(const auto& C:GEngine->GetWorldContexts())if(C.World() && C.World()->IsGameWorld() && C.World()->HasBegunPlay()){W=C.World();break;}if(!W || W->GetTimeSeconds()<.5f)return true;
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  auto Int=[](UObject* O,const TCHAR* N){auto* P=FindFProperty<FIntProperty>(O->GetClass(),N);return P?P->GetPropertyValue_InContainer(O):-999;};
  auto Flag=[](UObject* O,const TCHAR* N){auto* P=FindFProperty<FBoolProperty>(O->GetClass(),N);return P && P->GetPropertyValue_InContainer(O);};
  auto Error=[](UObject* O){auto* P=FindFProperty<FStrProperty>(O->GetClass(),TEXT("LastDasError"));return P?P->GetPropertyValue_InContainer(O):FString();};
  auto Call=[](UObject* O,const TCHAR* N,void* Args=nullptr){auto* F=O->FindFunction(N);if(!F)return false;O->ProcessEvent(F,Args);return true;};
  auto Find=[](UObject* Owner){UDasEffectProxy* Found=nullptr;for(TObjectIterator<UDasEffectProxy> It;It;++It)if(It->Owner.Get()==Owner && It->Active && (!Found || It->SubscriptionId>Found->SubscriptionId))Found=*It;return Found;};
  auto Probe=[&](int32 Input){return R.RunNamedPersistent(S->ReloadPath,TEXT("main"),Input,S->Pickup.Get());};
  auto Queue=[](UDasEffectProxy* P){if(P->Audio)P->Audio->OnAudioFinished.Broadcast();if(P->Niagara)P->Niagara->OnSystemFinished.Broadcast(P->Niagara);};
  if(S->Stage==0){
   auto* PC=W->GetFirstPlayerController();auto* Player=PC?Cast<ACharacter>(PC->GetPawn()):nullptr;AActor* Pickup=nullptr;for(const auto& A:W->PersistentLevel->Actors)if(A && A->GetFName()==TEXT("DAS_Pickup"))Pickup=A;
   if(!Player || !Pickup)return Fail(TEXT("playable map missing"));if(!Error(Player).IsEmpty() || !Error(Pickup).IsEmpty())return Fail(Error(Player)+Error(Pickup));if(!Flag(Pickup,TEXT("MeshReady")) || !Flag(Player,TEXT("ControlsBound")))return true;
   S->Player.Reset(Player);S->Pickup.Reset(Pickup);S->PlayerBindings=R.SubscriptionCount(Player);S->PickupBindings=R.SubscriptionCount(Pickup);
   for(auto Pair:{TPair<UObject*,const TCHAR*>(Player,TEXT("HurtVFX")),TPair<UObject*,const TCHAR*>(Pickup,TEXT("PickupVFX"))}){auto* P=FindFProperty<FObjectPropertyBase>(Pair.Key->GetClass(),Pair.Value);auto* System=P?Cast<UNiagaraSystem>(P->GetObjectPropertyValue_InContainer(Pair.Key)):nullptr;UE_LOG(LogTemp,Display,TEXT("EFFECT_SYSTEM=%s ready=%d"),*GetNameSafe(System),System && System->IsReadyToRun());}
   AActor* Collector=Player;Call(Pickup,TEXT("Interact"),&Collector);float Damage=25;Call(Player,TEXT("ReceiveDamage"),&Damage);
   S->Current.Reset(Find(Pickup));S->Hurt.Reset(Find(Player));
   if(Int(Player,TEXT("Score"))!=10 || Int(Pickup,TEXT("EffectsStarted"))!=1 || Int(Player,TEXT("HurtEffectsStarted"))!=1 || !S->Current.IsValid() || !S->Hurt.IsValid())return Fail(FString::Printf(TEXT("pickup/damage cues score=%d pickup_started=%d hurt_started=%d pickup_proxy=%d hurt_proxy=%d: "),Int(Player,TEXT("Score")),Int(Pickup,TEXT("EffectsStarted")),Int(Player,TEXT("HurtEffectsStarted")),S->Current.IsValid(),S->Hurt.IsValid())+Error(Player)+Error(Pickup));
   for(auto* P:{S->Current.Get(),S->Hurt.Get()})if(!P->Audio || !P->Niagara || !P->Audio->IsRegistered() || !P->Niagara->IsRegistered() || P->Audio->GetOwner()!=P->Owner.Get() || P->Niagara->GetOwner()!=P->Owner.Get())return Fail(TEXT("owned registered audio/Niagara components"));
   if(!FMath::IsNearlyEqual(S->Current->Audio->PitchMultiplier,1.4f) || !FMath::IsNearlyEqual(S->Hurt->Audio->PitchMultiplier,.7f))return Fail(TEXT("cue audio settings"));
   S->OldAudio.Reset(S->Current->Audio);S->OldNiagara.Reset(S->Current->Niagara);S->Deadline=FPlatformTime::Seconds()+12;S->Stage=1;
   UE_LOG(LogTemp,Display,TEXT("PASS effects_pickup_score_and_damage_spawn_owned_audio_niagara"));return true;
  }
  if(S->Stage==1){
   // Real CPU-system simulation; rendering is deliberately outside this check.
   for(auto* P:{S->Current.Get(),S->Hurt.Get()})if(P && P->Active && P->Niagara && P->Niagara->IsActive()){P->Niagara->SetForceSolo(true);P->Niagara->AdvanceSimulation(2,1.f/30.f);}
   if(Int(S->Pickup.Get(),TEXT("EffectsFinished"))!=1 || Int(S->Player.Get(),TEXT("HurtEffectsFinished"))!=1){if(FPlatformTime::Seconds()>S->Deadline)return Fail(FString::Printf(TEXT("natural completion audio=%d niagara=%d hurt_audio=%d hurt_niagara=%d"),S->Current->AudioDone,S->Current->NiagaraDone,S->Hurt->AudioDone,S->Hurt->NiagaraDone));return true;}
   if(S->Current->Active || S->Hurt->Active || S->OldAudio->IsRegistered() || S->OldNiagara->IsRegistered() || R.SubscriptionCount(S->Pickup.Get())!=S->PickupBindings || R.SubscriptionCount(S->Player.Get())!=S->PlayerBindings)return Fail(TEXT("completed cue retained components/subscription"));
   UE_LOG(LogTemp,Display,TEXT("PASS effects_natural_audio_and_niagara_completion_once_and_release"));
   S->External.Reset(NewObject<UAudioComponent>(S->Pickup.Get()));S->External->SetAutoActivate(false);S->Pickup->AddInstanceComponent(S->External.Get());S->External->RegisterComponentWithWorld(W);
   if(!FFileHelper::LoadFileToString(S->Template,*R.ScriptPath(TEXT("effects_reload_probe.das"))))return Fail(TEXT("reload template missing"));S->ReloadPath=R.ScriptPath(TEXT("effects_reload_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".das"));FFileHelper::SaveStringToFile(S->Template,*S->ReloadPath);
   auto Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);S->Old.Reset(Find(S->Pickup.Get()));S->OldAudio.Reset(S->Old->Audio);S->OldNiagara.Reset(S->Old->Niagara);
   Result=Probe(0);S->Current.Reset(Find(S->Pickup.Get()));if(!Result.bSucceeded || Result.Value!=1 || S->Old->Active || S->OldAudio->IsRegistered() || S->OldNiagara->IsRegistered() || S->Current.Get()==S->Old.Get())return Fail(TEXT("same-name replacement failed: ")+Result.Error);
   Queue(S->Current.Get());Result=Probe(3);if(!Result.bSucceeded || Result.Value!=1 || S->Current->Active)return Fail(TEXT("stop failed"));S->Stage=2;return true;
  }
  if(S->Stage==2){
   auto Result=Probe(1);if(!Result.bSucceeded || Result.Value!=0 || !S->External->IsRegistered())return Fail(TEXT("canceled completion delivered or unrelated component removed"));
   UE_LOG(LogTemp,Display,TEXT("PASS effects_replace_stop_drop_queued_and_preserve_unrelated_component"));
   Result=Probe(4);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);auto* Audio=Find(S->Pickup.Get());if(!Audio || !Audio->Audio || Audio->Niagara)return Fail(TEXT("sound-only cue"));
   Result=Probe(5);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);auto* FX=Find(S->Pickup.Get());if(!FX || FX->Audio || !FX->Niagara)return Fail(TEXT("Niagara-only cue"));Result=Probe(6);if(!Result.bSucceeded || Result.Value!=2 || !S->External->IsRegistered())return Fail(TEXT("stop-all count/ownership"));
   UE_LOG(LogTemp,Display,TEXT("PASS effects_sound_only_niagara_only_and_stop_all"));
   Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);S->Old.Reset(Find(S->Pickup.Get()));Queue(S->Old.Get());S->Revision=Result.Revision;FFileHelper::SaveStringToFile(S->Template+TEXT("\n// accepted revision\n"),*S->ReloadPath);Result=Probe(1);if(!Result.bSucceeded || Result.Value!=0 || Result.Revision<=S->Revision || S->Old->Active)return Fail(TEXT("accepted reload did not revoke components: ")+Result.Error);S->Stage=3;return true;
  }
  if(S->Stage==3){
   auto Result=Probe(1);if(!Result.bSucceeded || Result.Value!=0)return Fail(TEXT("old queued completion reached new VM"));
   UE_LOG(LogTemp,Display,TEXT("PASS effects_hot_reload_revokes_components_and_pending_callback"));
   Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);S->Current.Reset(Find(S->Pickup.Get()));S->Revision=Result.Revision;FFileHelper::SaveStringToFile(S->Template+TEXT("\nBROKEN SOURCE {\n"),*S->ReloadPath);Result=Probe(2);if(!Result.bSucceeded || Result.ReloadError.IsEmpty() || Result.Revision!=S->Revision || Result.Value!=1 || !S->Current->Active)return Fail(TEXT("rejected reload lost effect"));Queue(S->Current.Get());S->Stage=4;return true;
  }
  if(S->Stage==4){
   auto Result=Probe(1);if(!Result.bSucceeded || Result.Value!=1)return Fail(TEXT("old callback lost after rejected reload: ")+Result.Error);
   UE_LOG(LogTemp,Display,TEXT("PASS effects_rejected_reload_preserves_effect_and_callback"));
   FFileHelper::SaveStringToFile(S->Template+TEXT("\n// recovered\n"),*S->ReloadPath);Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1 || !Result.ReloadError.IsEmpty())return Fail(Result.Error);S->Current.Reset(Find(S->Pickup.Get()));S->Before=Int(S->Pickup.Get(),TEXT("EffectsFinished"));Queue(S->Current.Get());
   float Damage=25;Call(S->Player.Get(),TEXT("ReceiveDamage"),&Damage);S->Hurt.Reset(Find(S->Player.Get()));if(!S->Hurt.IsValid())return Fail(TEXT("damage cue missing before Destroy"));Queue(S->Hurt.Get());S->Pickup->Destroy();S->Player->Destroy();
   if(S->Current->Active || S->Hurt->Active || R.SubscriptionCount(S->Pickup.Get())!=0 || R.SubscriptionCount(S->Player.Get())!=0)return Fail(TEXT("Destroy retained cue subscriptions"));S->Stage=5;return true;
  }
  if(Int(S->Pickup.Get(),TEXT("EffectsFinished"))!=S->Before || Int(S->Player.Get(),TEXT("HurtEffectsFinished"))!=1 || S->Current->Audio || S->Current->Niagara || S->Hurt->Audio || S->Hurt->Niagara)return Fail(TEXT("late callback or retained component after Destroy"));
  UE_LOG(LogTemp,Display,TEXT("PASS effects_destroy_cleans_pickup_damage_and_late_callbacks"));
  UE_LOG(LogTemp,Display,TEXT("DAS_EFFECTS_RESULT=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
 }));
}
