#include "DasInputUIProbe.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetNavigation.h"
#include "Components/Button.h"
#include "UObject/UObjectIterator.h"
#include "UObject/StructOnScope.h"
#include "Containers/Ticker.h"
#include "HAL/PlatformMisc.h"

void StartDasInputUIValidation(){
 struct FState {int32 Stage=0,MoveBefore=0,ClicksBefore=0;double Start=FPlatformTime::Seconds();TWeakObjectPtr<ACharacter> Player;TWeakObjectPtr<ADasInputUIProbe> Host;};
 auto S=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float){
  auto Fail=[](const FString& E){UE_LOG(LogTemp,Error,TEXT("DAS_INPUT_UI=FAIL %s"),*E);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-S->Start>30)return Fail(TEXT("timeout"));
  UWorld* W=nullptr;for(const auto& C:GEngine->GetWorldContexts())if(C.World() && C.World()->IsGameWorld() && C.World()->HasBegunPlay()){W=C.World();break;}if(!W || W->GetTimeSeconds()<.5f)return true;
  auto Obj=[](UObject* O,const TCHAR* N)->UObject*{auto* P=O?FindFProperty<FObjectPropertyBase>(O->GetClass(),N):nullptr;return P?P->GetObjectPropertyValue_InContainer(O):nullptr;};
  auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):-1;};
  auto Error=[](UObject* O){auto* P=O?FindFProperty<FStrProperty>(O->GetClass(),TEXT("LastDasError")):nullptr;return P?P->GetPropertyValue_InContainer(O):FString();};
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  auto* PC=W->GetFirstPlayerController();auto* Player=PC?Cast<ACharacter>(PC->GetPawn()):nullptr;
  if(!PC || !Player)return true;if(!Error(Player).IsEmpty())return Fail(Error(Player));
  auto* HUD=Cast<UUserWidget>(Obj(Player,TEXT("HUD")));if(!HUD || !HUD->IsInViewport())return true;
  if(S->Stage==0){
   auto* Host=W->SpawnActor<ADasInputUIProbe>();S->Host=Host;Host->Action=NewObject<UInputAction>(Host);Host->Action->ValueType=EInputActionValueType::Axis2D;
   Host->InputComponent=NewObject<UEnhancedInputComponent>(Host);Host->AddInstanceComponent(Host->InputComponent);Host->InputComponent->RegisterComponent();Host->Widget=NewObject<UButton>(Host);
   auto Result=R.RunNamedPersistent(R.ScriptPath(TEXT("input_ui_probe.das")),TEXT("main"),0,Host);if(!Result.bSucceeded || Result.Value!=42)return Fail(Result.Error);
   if(!Host->Widget->Navigation || !Host->Widget->Navigation->Right.CustomDelegate.IsBound() || Host->Widget->Navigation->Right.CustomDelegate.Execute(EUINavigation::Right)!=Host->Widget || Host->NavigationCalls!=1)return Fail(TEXT("generated native UMG delegate did not execute"));
   auto* ProxyType=FindObject<UClass>(nullptr,TEXT("/Script/DasRuntime.DasEnhancedInputProxy"));if(!ProxyType)return Fail(TEXT("input proxy missing"));
   const FInputActionValue Values[]={FInputActionValue(true),FInputActionValue(.5f),FInputActionValue(FVector2D(3,4)),FInputActionValue(FVector(1,2,3))};
   for(const auto& Value:Values)for(TObjectIterator<UObject> It;It;++It)if(It->IsA(ProxyType)){
    auto* F=It->FindFunction(TEXT("Receive"));FStructOnScope Args(F);
    auto* V=FindFProperty<FStructProperty>(F,TEXT("ActionValue"));V->Struct->CopyScriptStruct(V->ContainerPtrToValuePtr<void>(Args.GetStructMemory()),&Value);
    auto* A=FindFProperty<FObjectPropertyBase>(F,TEXT("SourceAction"));A->SetObjectPropertyValue_InContainer(Args.GetStructMemory(),Host->Action);It->ProcessEvent(F,Args.GetStructMemory());
   }
   Result=R.RunNamedPersistent(R.ScriptPath(TEXT("input_ui_probe.das")),TEXT("main"),1,Host);if(!Result.bSucceeded || Result.Value!=42)return Fail(Result.Error);
   UE_LOG(LogTemp,Display,TEXT("PASS typed_input_four_shapes_owned_copies_and_native_umg_delegate"));
   S->MoveBefore=Int(Player,TEXT("MoveEvents"));S->ClicksBefore=Int(Player,TEXT("HUDClicks"));S->Player=Player;
   auto* Sub=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());auto* Action=Cast<UInputAction>(Obj(Player,TEXT("MoveAction")));if(!Sub || !Action)return Fail(TEXT("local input is not configured"));
   Sub->InjectInputForAction(Action,FInputActionValue(FVector2D(0,1)),{},{});S->Stage=1;return true;
  }
  if(S->Stage==1){
   if(Int(Player,TEXT("MoveEvents"))<=S->MoveBefore)return true;
   auto* Toggle=Player->FindFunction(TEXT("OnToggleUI"));if(!Toggle)return Fail(TEXT("UI toggle missing"));bool Pressed=true;Player->ProcessEvent(Toggle,&Pressed);if(!PC->bShowMouseCursor)return Fail(TEXT("UI mode/cursor not enabled"));
   auto* Button=Cast<UButton>(HUD->GetWidgetFromName(TEXT("Damage")));if(!Button)return Fail(TEXT("HUD button missing"));Button->OnClicked.Broadcast();S->Stage=2;return true;
  }
  if(Int(Player,TEXT("HUDClicks"))!=S->ClicksBefore+1)return true;
  bool Pressed=true;Player->ProcessEvent(Player->FindFunction(TEXT("OnToggleUI")),&Pressed);if(PC->bShowMouseCursor)return Fail(TEXT("game input/cursor not restored"));
  auto* Host=S->Host.Get();auto* Input=Cast<UEnhancedInputComponent>(Host->InputComponent);R.ReleaseObject(Host);if(Input->GetActionEventBindings().Num()!=0)return Fail(TEXT("input subscription survived release"));Host->Destroy();
  UE_LOG(LogTemp,Display,TEXT("PASS typed_authored_move_real_injection_hud_click_ui_game_and_release"));
  UE_LOG(LogTemp,Display,TEXT("DAS_INPUT_UI=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
 }));
}
