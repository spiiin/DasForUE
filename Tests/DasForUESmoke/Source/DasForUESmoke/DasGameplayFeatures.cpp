#include "CoreMinimal.h"
#include "DasRuntime.h"
#include "Containers/Ticker.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Components/ActorComponent.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "Modules/ModuleManager.h"
#include "UObject/UnrealType.h"
#include "UObject/StrongObjectPtr.h"

void StartDasGameplayFeaturesValidation(){
 struct FState {int32 Stage=0;double Started=FPlatformTime::Seconds();float Deadline=0;TWeakObjectPtr<ACharacter> Player;TWeakObjectPtr<AActor> Pickup;TStrongObjectPtr<UUserWidget> OldHUD;TStrongObjectPtr<ACharacter> DeadPlayer;};
 auto S=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float){
  auto Fail=[](const FString& E){UE_LOG(LogTemp,Error,TEXT("DAS_GAMEPLAY_FEATURES_RESULT=FAIL %s"),*E);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-S->Started>45)return Fail(TEXT("timeout"));
  UWorld* W=nullptr;for(const auto& C:GEngine->GetWorldContexts())if(C.World() && C.World()->IsGameWorld() && C.World()->HasBegunPlay()){W=C.World();break;}if(!W || W->GetTimeSeconds()<.5f)return true;
  auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):-999;};
  auto Float=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FFloatProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):-999.f;};
  auto Object=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FObjectPropertyBase>(O->GetClass(),N):nullptr;return P?P->GetObjectPropertyValue_InContainer(O):nullptr;};
  auto Error=[](UObject* O){auto* P=O?FindFProperty<FStrProperty>(O->GetClass(),TEXT("LastDasError")):nullptr;return P?P->GetPropertyValue_InContainer(O):FString();};
  auto Call=[](UObject* O,const TCHAR* N,void* Args=nullptr){auto* F=O?O->FindFunction(N):nullptr;if(!F)return false;O->ProcessEvent(F,Args);return true;};
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  if(S->Stage==0){
   auto* PC=W->GetFirstPlayerController();auto* P=PC?Cast<ACharacter>(PC->GetPawn()):nullptr;AActor* Pickup=nullptr;for(const auto& A:W->PersistentLevel->Actors)if(A && A->GetFName()==TEXT("DAS_Pickup"))Pickup=A;
   if(!P || !Pickup)return Fail(TEXT("missing saved demo"));if(!Error(P).IsEmpty() || !Error(Pickup).IsEmpty())return Fail(Error(P)+Error(Pickup));
   auto* Ready=FindFProperty<FBoolProperty>(Pickup->GetClass(),TEXT("MeshReady"));if(!Ready || !Ready->GetPropertyValue_InContainer(Pickup))return true;
   P->GetCharacterMovement()->DisableMovement();P->SetActorLocation(FVector(0,0,100));PC->SetControlRotation(FRotator::ZeroRotator);Pickup->SetActorLocation(FVector(300,0,150));
   auto* H=Cast<UUserWidget>(Object(P,TEXT("HUD")));auto* Health=Object(P,TEXT("Health"));
   if(!H || !H->IsInViewport() || H->GetOwningPlayer()!=PC || !Health || Float(Health,TEXT("Current"))!=100.f || !Error(Health).IsEmpty())return Fail(TEXT("HUD/health startup: ")+Error(P)+Error(Health));
   auto* T=Cast<UTextBlock>(H->GetWidgetFromName(TEXT("Score")));if(!T || T->GetText().ToString()!=TEXT("Score: 0"))return Fail(TEXT("HUD text startup"));
   Call(P,TEXT("SetupHUD"));if(Object(P,TEXT("HUD"))!=H)return Fail(TEXT("duplicate HUD creation"));
   auto WidgetResult=R.RunNamedPersistent(R.ScriptPath(TEXT("gameplay_features_probe.das")),TEXT("main"),3,P);if(!WidgetResult.bSucceeded || WidgetResult.Value!=1)return Fail(WidgetResult.Error);
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_widget_remove_revokes_and_invalid_bindings_refused"));
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_hud_local_player_text_health_component"));
   auto Result=R.RunNamedPersistent(R.ScriptPath(TEXT("gameplay_features_probe.das")),TEXT("main"),1,P);if(!Result.bSucceeded || Result.Value!=1)return Fail(Result.Error);
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_line_sphere_ignore_math_and_rejected_inputs"));
   auto* Sub=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());auto* Action=Cast<UInputAction>(Object(P,TEXT("InteractAction")));if(!Sub || !Action)return Fail(TEXT("interaction input asset"));
   Sub->InjectInputForAction(Action,FInputActionValue(true),{},{});S->Player=P;S->Pickup=Pickup;S->Stage=1;S->Deadline=W->GetTimeSeconds()+2;return true;
  }
  auto* P=S->Player.Get();
  if(S->Stage==1){
   if(!P)return Fail(TEXT("player expired"));if(Int(P,TEXT("Score"))!=10){if(!Error(P).IsEmpty() || W->GetTimeSeconds()>S->Deadline)return Fail(TEXT("E interaction did not award: ")+Error(P));return true;}
   if(Int(P,TEXT("Interactions"))!=1 || Int(S->Pickup.Get(),TEXT("Pickups"))!=1)return Fail(TEXT("typed interface interaction"));
   auto* H=Cast<UUserWidget>(Object(P,TEXT("HUD")));auto* T=H?Cast<UTextBlock>(H->GetWidgetFromName(TEXT("Score"))):nullptr;if(!T || T->GetText().ToString()!=TEXT("Score: 10"))return Fail(TEXT("score HUD refresh"));
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_enhanced_E_trace_interface_pickup_score_hud"));
   auto* PC=Cast<APlayerController>(P->GetController());auto* Sub=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());auto* Action=Cast<UInputAction>(Object(P,TEXT("HurtAction")));if(!Action)return Fail(TEXT("damage action missing"));Sub->InjectInputForAction(Action,FInputActionValue(true),{},{});S->Stage=2;S->Deadline=W->GetTimeSeconds()+2;return true;
  }
  if(S->Stage==2){
   if(!P)return Fail(TEXT("player expired"));auto* Health=Object(P,TEXT("Health"));if(Float(Health,TEXT("Current"))!=75){if(!Error(P).IsEmpty() || !Error(Health).IsEmpty() || W->GetTimeSeconds()>S->Deadline)return Fail(TEXT("native damage/health callback: ")+Error(P)+Error(Health));return true;}
   if(Int(P,TEXT("HealthChanges"))!=1 || Int(Health,TEXT("DamageEvents"))!=1)return Fail(TEXT("damage/changed callback count"));
   auto* H=Cast<UUserWidget>(Object(P,TEXT("HUD")));auto* Text=H?Cast<UTextBlock>(H->GetWidgetFromName(TEXT("Health"))):nullptr;if(!Text || Text->GetText().ToString()!=TEXT("Health: 75 / 100"))return Fail(TEXT("health HUD refresh"));
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_H_native_damage_component_signal_hud"));
   auto* B=Cast<UButton>(H->GetWidgetFromName(TEXT("Damage")));if(!B || !B->OnClicked.IsBound())return Fail(TEXT("button binding"));B->OnClicked.Broadcast();
   if(Float(Health,TEXT("Current"))!=75)return Fail(TEXT("button reentered VM synchronously"));S->Stage=3;S->Deadline=W->GetTimeSeconds()+2;return true;
  }
  if(S->Stage==3){
   if(!P)return Fail(TEXT("player expired"));auto* Health=Object(P,TEXT("Health"));if(Float(Health,TEXT("Current"))!=50){if(!Error(P).IsEmpty() || W->GetTimeSeconds()>S->Deadline)return Fail(TEXT("button deferred callback: ")+Error(P));return true;}
   if(Int(P,TEXT("HUDClicks"))!=1 || Int(P,TEXT("HealthChanges"))!=2)return Fail(TEXT("button fired more than once"));
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_umg_button_deferred_damage"));
   float Amount=500;Call(P,TEXT("ReceiveDamage"),&Amount);if(Float(Health,TEXT("Current"))!=0)return Fail(TEXT("lethal clamp"));Call(Health,TEXT("Heal"),&Amount);if(Float(Health,TEXT("Current"))!=100)return Fail(TEXT("heal maximum clamp"));
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_health_zero_and_maximum_clamp"));
   bool Pressed=true;Call(P,TEXT("OnToggleUI"),&Pressed);auto* PC=Cast<APlayerController>(P->GetController());if(!PC || !PC->bShowMouseCursor)return Fail(TEXT("UI input/cursor"));
   auto* H=Cast<UUserWidget>(Object(P,TEXT("HUD")));S->OldHUD.Reset(H);auto* B=Cast<UButton>(H->GetWidgetFromName(TEXT("Damage")));B->OnClicked.Broadcast();
   auto* Pickup=S->Pickup.Get();Pickup->Destroy();auto Result=R.RunNamedPersistent(R.ScriptPath(TEXT("gameplay_features_probe.das")),TEXT("main"),2,P);if(!Result.bSucceeded || Result.Value!=1)return Fail(TEXT("stale hit: ")+Result.Error);
   UE_LOG(LogTemp,Display,TEXT("PASS gameplay_trace_weak_references_after_destroy"));
   S->DeadPlayer.Reset(P);P->Destroy();if(H->IsInViewport() || B->OnClicked.IsBound() || PC->bShowMouseCursor)return Fail(TEXT("EndPlay retained widget/delegate/input"));S->Stage=4;S->Deadline=W->GetTimeSeconds()+.15f;return true;
  }
  if(W->GetTimeSeconds()<S->Deadline)return true;
  if(Int(S->DeadPlayer.Get(),TEXT("HUDClicks"))!=1 || !Error(S->DeadPlayer.Get()).IsEmpty())return Fail(TEXT("queued click reached destroyed owner"));
  UE_LOG(LogTemp,Display,TEXT("PASS gameplay_endplay_removes_hud_callbacks_input_and_pending_click"));
  UE_LOG(LogTemp,Display,TEXT("DAS_GAMEPLAY_FEATURES_RESULT=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
 }));
}
