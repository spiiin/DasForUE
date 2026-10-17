#include "CoreMinimal.h"
#include "DasRuntime.h"
#include "DasMontageProxy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotifies/AnimNotify_PlayMontageNotify.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"
#include "Engine/World.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

void StartDasMontageValidation(){
 struct FState {
  int32 Stage=0,Ticks=0,Before=0;double Started=FPlatformTime::Seconds();uint32 Revision=0;
  TStrongObjectPtr<AActor> Actor;TStrongObjectPtr<UAnimMontage> Montage,External;
  TStrongObjectPtr<UAnimInstance> Anim;TStrongObjectPtr<UDasMontageProxy> Sentinel,DeadProxy;
  FString ReloadPath,GoodSource;
  ~FState(){if(!ReloadPath.IsEmpty())IFileManager::Get().Delete(*ReloadPath);}
 };auto S=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float){
  auto Fail=[](const FString& E){UE_LOG(LogTemp,Error,TEXT("DAS_MONTAGE_RESULT=FAIL %s"),*E);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-S->Started>35 || ++S->Ticks>3000)return Fail(TEXT("timeout stage ")+FString::FromInt(S->Stage));
  UWorld* W=nullptr;for(const auto& C:GEngine->GetWorldContexts())if(C.World() && C.World()->IsGameWorld() && C.World()->HasBegunPlay()){W=C.World();break;}if(!W)return true;
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  auto Int=[](UObject* O,const TCHAR* N){auto* P=FindFProperty<FIntProperty>(O->GetClass(),N);return P?P->GetPropertyValue_InContainer(O):-999;};
  auto Error=[](UObject* O){auto* P=FindFProperty<FStrProperty>(O->GetClass(),TEXT("LastDasError"));return P?P->GetPropertyValue_InContainer(O):FString();};
  auto Call=[](UObject* O,const TCHAR* N,void* Args=nullptr){auto* F=O->FindFunction(N);if(!F)return false;O->ProcessEvent(F,Args);return true;};
  auto Play=[&](){float Length=0;Call(S->Actor.Get(),TEXT("Play"),&Length);return Length>0 && Error(S->Actor.Get()).IsEmpty();};
  auto Probe=[&](int32 Input){return R.RunNamedPersistent(S->ReloadPath,TEXT("main"),Input,S->Actor.Get());};
  auto Notify=[&](int32 Delta=0){FBranchingPointNotifyPayload Payload;auto* Instance=S->Anim->GetActiveInstanceForMontage(S->Montage.Get());Payload.MontageInstanceID=Instance?Instance->GetInstanceID()+Delta:INDEX_NONE;Payload.SkelMeshComponent=Cast<USkeletalMeshComponent>(S->Actor->GetRootComponent());Payload.SequenceAsset=S->Montage.Get();S->Anim->OnPlayMontageNotifyBegin.Broadcast(TEXT("Injected"),Payload);};
  if(S->Stage==0){
   AActor* Actor=nullptr;for(const auto& A:W->PersistentLevel->Actors)if(A && A->GetFName()==TEXT("DAS_Montage"))Actor=A;if(!Actor)return Fail(TEXT("saved Montage actor missing"));S->Actor.Reset(Actor);
   auto* Mesh=Cast<USkeletalMeshComponent>(Actor->GetRootComponent());auto* Asset=LoadObject<USkeletalMesh>(nullptr,TEXT("/Engine/EngineMeshes/SkeletalCube.SkeletalCube"));if(!Mesh || !Asset)return Fail(TEXT("fixture skeletal mesh missing"));
   Mesh->SetSkeletalMesh(Asset);Mesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);Mesh->SetAnimInstanceClass(UAnimInstance::StaticClass());Mesh->SetComponentTickEnabled(false);auto* Anim=Mesh->GetAnimInstance();if(!Anim)return Fail(TEXT("fixture AnimInstance missing"));S->Anim.Reset(Anim);
   auto* M=NewObject<UAnimMontage>();S->Montage.Reset(M);M->SetSkeleton(Asset->GetSkeleton());M->SlotAnimTracks.Reset();M->SlotAnimTracks.AddDefaulted_GetRef().SlotName=TEXT("DefaultSlot");M->SetCompositeLength(.6f);M->BlendIn.SetBlendTime(.01f);M->BlendOut.SetBlendTime(.05f);M->CompositeSections.Reset();M->CompositeSections.AddDefaulted_GetRef().SectionName=TEXT("Default");M->CompositeSections[0].Link(M,0.f);
   auto* Window=NewObject<UAnimNotify_PlayMontageNotifyWindow>(M);FindFProperty<FNameProperty>(Window->GetClass(),TEXT("NotifyName"))->SetPropertyValue_InContainer(Window,TEXT("HitWindow"));
   auto& N=M->Notifies.AddDefaulted_GetRef();N.NotifyStateClass=Window;N.MontageTickType=EMontageNotifyTickType::BranchingPoint;N.Link(M,.12f);N.SetDuration(.12f);M->RefreshCacheData();M->SetCompositeLength(.6f);
   Asset->GetSkeleton()->SetSlotGroupName(TEXT("DasExternalSlot"),TEXT("DasExternalGroup"));
   auto* External=NewObject<UAnimMontage>();S->External.Reset(External);External->SetSkeleton(Asset->GetSkeleton());
   External->SlotAnimTracks.Reset();External->SlotAnimTracks.AddDefaulted_GetRef().SlotName=TEXT("DasExternalSlot");External->SetCompositeLength(100.f);
   External->CompositeSections.Reset();External->CompositeSections.AddDefaulted_GetRef().SectionName=TEXT("Default");External->CompositeSections[0].Link(External,0.f);
   if(External->GetGroupName()==M->GetGroupName() || Anim->Montage_Play(External)<=0.f)return Fail(TEXT("external group fixture failed"));
   auto* P=FindFProperty<FObjectPropertyBase>(Actor->GetClass(),TEXT("Montage"));if(!P)return Fail(TEXT("typed Montage property missing"));P->SetObjectPropertyValue_InContainer(Actor,M);
   S->Sentinel.Reset(NewObject<UDasMontageProxy>());Anim->OnPlayMontageNotifyBegin.AddDynamic(S->Sentinel.Get(),&UDasMontageProxy::NotifyBegin);
   if(!Play() || R.SubscriptionCount(Actor)!=1)return Fail(TEXT("DAS Play did not create a managed subscription: ")+Error(Actor));
   bool Playing=false;Call(Actor,TEXT("Playing"),&Playing);if(!Playing)return Fail(TEXT("DAS playing query"));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_typed_play_and_owned_subscription"));S->Stage=1;return true;
  }
  if(S->Stage<=3 || S->Stage==20){S->Anim->TickMontageOnly(.02f);}
  auto* A=S->Actor.Get();
  if(S->Stage==1){
   if(Int(A,TEXT("Completed"))<1)return true;
   auto* Name=FindFProperty<FNameProperty>(A->GetClass(),TEXT("LastNotify"));if(Int(A,TEXT("Completed"))!=1 || Int(A,TEXT("BlendOut"))!=1 || Int(A,TEXT("Interrupted"))!=0 || Int(A,TEXT("NotifyBegin"))!=1 || Int(A,TEXT("NotifyEnd"))!=1 || !Name || Name->GetPropertyValue_InContainer(A)!=TEXT("HitWindow") || R.SubscriptionCount(A)!=0)return Fail(TEXT("natural completion/real notify window payload/cleanup: ")+Error(A));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_natural_completion_blend_and_real_notify_window"));
   if(!Play())return Fail(Error(A));bool Stopped=false;Call(A,TEXT("Stop"),&Stopped);if(!Stopped)return Fail(TEXT("DAS stop failed"));S->Stage=2;return true;
  }
  if(S->Stage==2){
   if(Int(A,TEXT("Interrupted"))<1)return true;if(Int(A,TEXT("Interrupted"))!=1 || Int(A,TEXT("Completed"))!=1 || R.SubscriptionCount(A)!=0)return Fail(TEXT("stop should deliver exactly one interrupted event"));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_stop_interrupted_once"));
   if(!Play() || !Play() || !S->Anim->GetActiveInstanceForMontage(S->Montage.Get()))return Fail(TEXT("same-key replacement stopped new instance"));S->Stage=3;return true;
  }
  if(S->Stage==3){
   if(Int(A,TEXT("Completed"))<2)return true;if(Int(A,TEXT("Completed"))!=2 || Int(A,TEXT("Interrupted"))!=1 || Int(A,TEXT("NotifyBegin"))!=2 || Int(A,TEXT("NotifyEnd"))!=2)return Fail(TEXT("replacement mixed old/new playback events"));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_same_asset_replacement_isolates_instance_ids"));
   auto* Repeat=FindFProperty<FBoolProperty>(A->GetClass(),TEXT("RepeatOnComplete"));if(!Repeat)return Fail(TEXT("repeat property missing"));Repeat->SetPropertyValue_InContainer(A,true);
   if(!Play())return Fail(Error(A));S->Stage=20;return true;
  }
  if(S->Stage==20){
   if(Int(A,TEXT("Completed"))<4)return true;
   if(Int(A,TEXT("Completed"))!=4 || Int(A,TEXT("RearmStarted"))!=1 || Int(A,TEXT("Interrupted"))!=1 || R.SubscriptionCount(A)!=0 || !Error(A).IsEmpty())return Fail(TEXT("terminal callback rearm failed: ")+Error(A));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_terminal_callback_rearms_same_name"));
   if(!Play())return Fail(Error(A));Notify(100000);S->Before=Int(A,TEXT("NotifyBegin"));S->Stage=4;return true;
  }
  if(S->Stage==4){
   if(Int(A,TEXT("NotifyBegin"))!=S->Before)return Fail(TEXT("foreign instance notify delivered"));Notify();bool Canceled=false;Call(A,TEXT("Cancel"),&Canceled);if(!Canceled || R.SubscriptionCount(A)!=0)return Fail(TEXT("cancel did not revoke subscription"));S->Stage=5;return true;
  }
  if(S->Stage==5){
   if(Int(A,TEXT("NotifyBegin"))!=S->Before || !S->Anim->OnPlayMontageNotifyBegin.IsAlreadyBound(S->Sentinel.Get(),&UDasMontageProxy::NotifyBegin))return Fail(TEXT("cancel retained pending event or removed external listener"));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_cancel_discards_pending_and_preserves_external_listener"));
   auto* External=S->Anim->GetActiveInstanceForMontage(S->External.Get());
   if(!External || !External->IsPlaying())return Fail(TEXT("owned play/stop/replace/cancel stopped another montage group"));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_other_group_playback_preserved"));
   FString Template;if(!FFileHelper::LoadFileToString(Template,*R.ScriptPath(TEXT("montage_reload_probe.das"))))return Fail(TEXT("reload template missing"));S->GoodSource=Template;S->ReloadPath=R.ScriptPath(TEXT("montage_reload_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".das"));if(!FFileHelper::SaveStringToFile(Template,*S->ReloadPath))return Fail(TEXT("write unique reload probe"));
   auto Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);S->Revision=Result.Revision;Notify();
   if(!FFileHelper::SaveStringToFile(Template+TEXT("\n// accepted revision\n"),*S->ReloadPath))return Fail(TEXT("write candidate"));Result=Probe(1);if(!Result.bSucceeded || Result.Revision<=S->Revision || Result.Value!=0 || R.SubscriptionCount(A)!=0)return Fail(TEXT("reload did not revoke old playback: ")+Result.Error);S->Stage=6;return true;
  }
  if(S->Stage==6){
   auto Result=Probe(1);if(!Result.bSucceeded || Result.Value!=0)return Fail(TEXT("old queued notify reached new VM"));
   UE_LOG(LogTemp,Display,TEXT("PASS montage_hot_reload_revokes_playback_and_queued_events"));
   Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);S->Revision=Result.Revision;
   if(!FFileHelper::SaveStringToFile(S->GoodSource+TEXT("\nTHIS IS BROKEN SOURCE {\n"),*S->ReloadPath))return Fail(TEXT("write rejected candidate"));Result=Probe(2);if(!Result.bSucceeded || Result.ReloadError.IsEmpty() || Result.Revision!=S->Revision || Result.Value!=1)return Fail(TEXT("rejected reload lost active playback"));Notify();S->Stage=7;return true;
  }
  if(S->Stage==7){
   auto Result=Probe(1);if(!Result.bSucceeded || Result.Value!=1)return Fail(TEXT("rejected reload lost old callback: ")+Result.Error);
   UE_LOG(LogTemp,Display,TEXT("PASS montage_rejected_reload_preserves_working_callback"));
   if(!FFileHelper::SaveStringToFile(S->GoodSource+TEXT("\n// recovered revision\n"),*S->ReloadPath))return Fail(TEXT("restore candidate"));Result=Probe(1);if(!Result.bSucceeded || !Result.ReloadError.IsEmpty() || Result.Value!=1)return Fail(TEXT("reload recovery/state preservation"));
   if(!Play())return Fail(Error(A));Notify();S->Before=Int(A,TEXT("NotifyBegin"));for(TObjectIterator<UDasMontageProxy> It;It;++It)if(It->Owner.Get()==A && It->Active)S->DeadProxy.Reset(*It);
   if(!S->DeadProxy.IsValid())return Fail(TEXT("owned proxy missing before destroy"));A->Destroy();if(R.SubscriptionCount(A)!=0 || S->DeadProxy->Active)return Fail(TEXT("Destroy retained montage subscription"));S->Stage=8;return true;
  }
  if(Int(A,TEXT("NotifyBegin"))!=S->Before || S->DeadProxy->Active || S->Anim->OnPlayMontageNotifyBegin.IsAlreadyBound(S->DeadProxy.Get(),&UDasMontageProxy::NotifyBegin))return Fail(TEXT("destroyed owner received late callback"));
  UE_LOG(LogTemp,Display,TEXT("PASS montage_destroy_revokes_native_delegates_and_pending_events"));
  UE_LOG(LogTemp,Display,TEXT("DAS_MONTAGE_RESULT=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
 }));
}
