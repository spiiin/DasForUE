#include "CoreMinimal.h"
#include "DasRuntime.h"
#include "DasAIMoveProxy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavigationSystem.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Containers/Ticker.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"
void StartDasAIValidation(){
 struct FState {
  int32 Stage=0,Before=0,Sentinel=0;uint32 Revision=0;double Started=FPlatformTime::Seconds();float MaxY=0;
  TStrongObjectPtr<ACharacter> Enemy,Player;
  TStrongObjectPtr<AAIController> AI;
  TStrongObjectPtr<UDasAIMoveProxy> Old,Current;
  FString ReloadPath,Template;
  FDelegateHandle Listener;
  ~FState(){if(AI.IsValid() && AI->GetPathFollowingComponent())AI->GetPathFollowingComponent()->OnRequestFinished.Remove(Listener);if(!ReloadPath.IsEmpty())IFileManager::Get().Delete(*ReloadPath);}
 };auto S=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float){
  auto Fail=[&](const FString& E){UE_LOG(LogTemp,Error,TEXT("DAS_AI_RESULT=FAIL stage=%d %s"),S->Stage,*E);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-S->Started>70)return Fail(TEXT("timeout"));
  UWorld* W=nullptr;for(const auto& C:GEngine->GetWorldContexts())if(C.World() && C.World()->IsGameWorld() && C.World()->HasBegunPlay()){W=C.World();break;}if(!W || W->GetTimeSeconds()<.5f)return true;
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  auto Int=[](UObject* O,const TCHAR* N){auto* P=FindFProperty<FIntProperty>(O->GetClass(),N);return P?P->GetPropertyValue_InContainer(O):-999;};
  auto Error=[](UObject* O){auto* P=FindFProperty<FStrProperty>(O->GetClass(),TEXT("LastDasError"));return P?P->GetPropertyValue_InContainer(O):FString();};
  auto Call=[](UObject* O,const TCHAR* N){auto* F=O->FindFunction(N);if(!F)return false;O->ProcessEvent(F,nullptr);return true;};
  auto Find=[&](){UDasAIMoveProxy* Found=nullptr;for(TObjectIterator<UDasAIMoveProxy> It;It;++It)if(It->Owner.Get()==S->Enemy.Get() && It->Active && (!Found || It->SubscriptionId>Found->SubscriptionId))Found=*It;return Found;};
  auto Probe=[&](int32 Input){return R.RunNamedPersistent(S->ReloadPath,TEXT("main"),Input,S->Enemy.Get());};
  auto Synthetic=[&](){auto* P=Find();if(P)P->Queue(EDasMoveCode::Success);};
  if(S->Stage==0){
   for(const auto& A:W->PersistentLevel->Actors)if(A && A->GetFName()==TEXT("DAS_Enemy"))S->Enemy.Reset(Cast<ACharacter>(A));auto* PC=W->GetFirstPlayerController();S->Player.Reset(PC?Cast<ACharacter>(PC->GetPawn()):nullptr);
   if(!S->Enemy.IsValid() || !S->Player.IsValid())return Fail(TEXT("missing demo actors"));S->AI.Reset(Cast<AAIController>(S->Enemy->GetController()));
   if(!S->AI.IsValid())return Fail(TEXT("enemy AIController missing"));if(!Error(S->Enemy.Get()).IsEmpty())return Fail(Error(S->Enemy.Get()));
   auto* Nav=FNavigationSystem::GetCurrent<UNavigationSystemV1>(W);FNavLocation Point;if(!Nav || !Nav->ProjectPointToNavigation(S->Enemy->GetActorLocation(),Point,FVector(100,100,300)))return Fail(TEXT("saved NavMesh missing"));
   S->Stage=1;return true;
  }
  auto* A=S->Enemy.Get();auto* PF=S->AI->GetPathFollowingComponent();
  if(S->Stage==1){
   if(!Error(A).IsEmpty())return Fail(Error(A));S->MaxY=FMath::Max(S->MaxY,FMath::Abs(float(A->GetActorLocation().Y)));
   if(Int(A,TEXT("Attacks"))<1)return true;
   // The player keeps health in GAS, which ApplyDamage does not change: the enemy example records the damage
   // the engine accepted (ApplyDamage returns 0 for a target that cannot be damaged).
   auto* DealtProperty=FindFProperty<FFloatProperty>(A->GetClass(),TEXT("DamageDealt"));const float Dealt=DealtProperty?DealtProperty->GetPropertyValue_InContainer(A):-1.f;
   if(S->MaxY<160 || A->GetDistanceTo(S->Player.Get())>155 || Dealt<=0.f)return Fail(FString::Printf(TEXT("path/damage maxY=%f distance=%f damage_dealt=%f"),S->MaxY,A->GetDistanceTo(S->Player.Get()),Dealt));
   if(!Call(A,TEXT("StopBrain")) || R.SubscriptionCount(A)!=0)return Fail(TEXT("brain cancel retained callbacks"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_navmesh_pursuit_obstacle_and_damage"));
   if(!FFileHelper::LoadFileToString(S->Template,*R.ScriptPath(TEXT("ai_reload_probe.das"))))return Fail(TEXT("missing reload template"));S->ReloadPath=R.ScriptPath(TEXT("ai_reload_")+FGuid::NewGuid().ToString(EGuidFormats::Digits)+TEXT(".das"));if(!FFileHelper::SaveStringToFile(S->Template,*S->ReloadPath))return Fail(TEXT("write probe"));
   auto Result=Probe(0);if(!Result.bSucceeded || Result.Value!=1 || !Find())return Fail(Result.Error);S->Stage=2;return true;
  }
  if(S->Stage==2){
   auto Result=Probe(1);if(!Result.bSucceeded)return Fail(Result.Error);if(Result.Value==0)return true;
   if(Result.Value!=1 || Probe(4).Value!=0 || R.SubscriptionCount(A)!=0 || PF->GetStatus()!=EPathFollowingStatus::Idle)return Fail(TEXT("location completion result/count"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_location_move_natural_success_once"));
   Result=Probe(5);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);S->Stage=3;return true;
  }
  if(S->Stage==3){
   if(Probe(1).Value!=2 || Probe(4).Value!=0)return Fail(TEXT("already-at-goal completion missing"));
   auto Result=Probe(6);if(!Result.bSucceeded || Result.Value!=0)return Fail(TEXT("unreachable request should fail"));S->Stage=4;return true;
  }
  if(S->Stage==4){
   if(Probe(1).Value!=3 || Probe(4).Value!=int32(EDasMoveCode::Invalid) || R.SubscriptionCount(A)!=0)return Fail(TEXT("unreachable completion missing"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_already_at_goal_and_failed_request_results"));
   A->SetActorLocation(FVector(-200,-300,100),false,nullptr,ETeleportType::TeleportPhysics);
   S->Listener=PF->OnRequestFinished.AddLambda([Weak=TWeakPtr<FState>(S)](FAIRequestID,const FPathFollowingResult&){if(auto State=Weak.Pin())++State->Sentinel;});
   if(Probe(0).Value!=1)return Fail(TEXT("start replacement probe"));S->Old.Reset(Find());auto OldId=S->Old->RequestId;
   if(Probe(0).Value!=1)return Fail(TEXT("replace probe"));S->Current.Reset(Find());
   if(S->Old->Active || !S->Current.IsValid() || S->Current->RequestId==OldId || S->Sentinel<1)return Fail(TEXT("replacement did not isolate request/listener"));
   Synthetic();if(Probe(3).Value!=1 || S->Current->Active)return Fail(TEXT("cancel queued result"));S->Stage=5;return true;
  }
  if(S->Stage==5){
   if(Probe(1).Value!=3)return Fail(TEXT("canceled result delivered"));
   // A newer external request must survive old binding cleanup.
   if(Probe(0).Value!=1)return Fail(TEXT("start external override probe"));
   FAIMoveRequest External;External.SetGoalLocation(FVector(1300,-1000,0));External.SetAcceptanceRadius(10);External.SetAllowPartialPath(false);auto Request=S->AI->MoveTo(External);
   if(Request.Code!=EPathFollowingRequestResult::RequestSuccessful || Probe(3).Value!=1 || PF->GetCurrentRequestId()!=Request.MoveId || PF->GetStatus()!=EPathFollowingStatus::Moving)return Fail(TEXT("cancel affected newer external movement"));
   PF->AbortMove(*S->AI,FPathFollowingResultFlags::OwnerFinished,Request.MoveId);
   UE_LOG(LogTemp,Display,TEXT("PASS ai_replace_cancel_pending_preserves_external_request_and_listener"));
   if(Probe(7).Value!=1)return Fail(TEXT("no-handler move"));S->Stage=6;return true;
  }
  if(S->Stage==6){
   if(R.SubscriptionCount(A)>0)return true;if(Probe(1).Value!=3)return Fail(TEXT("no-handler move changed callback count"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_no_handler_move_releases_subscription"));
   // Return pawn to a distant navigable position so the next request is active.
   A->SetActorLocation(FVector(-200,-300,100),false,nullptr,ETeleportType::TeleportPhysics);
   if(Probe(0).Value!=1)return Fail(TEXT("start reload probe"));S->Old.Reset(Find());S->Revision=Probe(2).Revision;
   FFileHelper::SaveStringToFile(S->Template+TEXT("\n// accepted active\n"),*S->ReloadPath);auto Result=Probe(1);
   if(!Result.bSucceeded || Result.Revision<=S->Revision || Result.Value!=3 || S->Old->Active || PF->GetStatus()!=EPathFollowingStatus::Idle || R.SubscriptionCount(A)!=0)return Fail(TEXT("active reload cleanup: ")+Result.Error);
   if(Probe(0).Value!=1)return Fail(TEXT("start queued reload probe"));Synthetic();S->Revision=Probe(2).Revision;
   FFileHelper::SaveStringToFile(S->Template+TEXT("\n// accepted pending\n"),*S->ReloadPath);Result=Probe(1);if(!Result.bSucceeded || Result.Revision<=S->Revision || Result.Value!=3 || R.SubscriptionCount(A)!=0)return Fail(TEXT("pending reload cleanup"));S->Stage=7;return true;
  }
  if(S->Stage==7){
   if(Probe(1).Value!=3)return Fail(TEXT("old result reached new VM"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_reload_cancels_active_request_and_pending_callback"));
   if(Probe(0).Value!=1)return Fail(TEXT("start rejected reload probe"));S->Revision=Probe(2).Revision;
   FFileHelper::SaveStringToFile(S->Template+TEXT("\nBROKEN SOURCE {\n"),*S->ReloadPath);auto Result=Probe(2);
   if(!Result.bSucceeded || Result.ReloadError.IsEmpty() || Result.Revision!=S->Revision || Result.Value!=1)return Fail(TEXT("rejected reload lost move"));
   // Finish the real request through UE, delivering Aborted to the retained VM.
   PF->AbortMove(*S->AI,FPathFollowingResultFlags::OwnerFinished,PF->GetCurrentRequestId());S->Stage=8;return true;
  }
  if(S->Stage==8){
   if(Probe(1).Value!=4 || Probe(4).Value!=int32(EDasMoveCode::Aborted))return Fail(TEXT("rejected reload lost completion"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_rejected_reload_preserves_request_and_callback"));
   FFileHelper::SaveStringToFile(S->Template+TEXT("\n// recovered\n"),*S->ReloadPath);if(!Probe(1).bSucceeded || Probe(0).Value!=1)return Fail(TEXT("recover probe"));S->Old.Reset(Find());
   Synthetic();S->Before=Int(A,TEXT("MoveCompletions"));
   A->Destroy();if(S->Old->Active || R.SubscriptionCount(A)!=0 || PF->GetStatus()!=EPathFollowingStatus::Idle)return Fail(TEXT("Destroy did not abort owned request"));S->Stage=9;return true;
  }
  if(S->Stage==9){
   if(Int(A,TEXT("MoveCompletions"))!=S->Before || S->Old->Active || PF->OnRequestFinished.IsBoundToObject(S->Old.Get()))return Fail(TEXT("destroyed owner retained callback"));
   UE_LOG(LogTemp,Display,TEXT("PASS ai_destroy_aborts_owned_request_and_removes_delegate"));
   UE_LOG(LogTemp,Display,TEXT("DAS_AI_RESULT=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
  }
  return true;
 }));
}
