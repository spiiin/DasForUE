#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "Containers/Ticker.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "DasRuntime.h"
#include "UObject/UnrealType.h"
static void StarterProbe(){
 struct FState {int32 Stage=0;double Start=FPlatformTime::Seconds();TWeakObjectPtr<ACharacter> Player;TWeakObjectPtr<AActor> Item;FString File,Original;~FState(){if(!Original.IsEmpty())FFileHelper::SaveStringToFile(Original,*File,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);}};
 auto S=MakeShared<FState>();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([S](float){
  auto Fail=[](const FString& E){UE_LOG(LogTemp,Error,TEXT("DAS_STARTER_RESULT=FAIL %s"),*E);FPlatformMisc::RequestExitWithStatus(false,1);return false;};
  if(FPlatformTime::Seconds()-S->Start>45)return Fail(TEXT("timeout stage ")+FString::FromInt(S->Stage));
  UWorld* W=nullptr;for(const auto& C:GEngine->GetWorldContexts())if(C.World() && C.World()->IsGameWorld() && C.World()->HasBegunPlay())W=C.World();if(!W || W->GetTimeSeconds()<.3f)return true;
  if(FParse::Param(FCommandLine::Get(),TEXT("DasStarterLightingProbe"))){
   if(W->GetTimeSeconds()<3.0f)return true;
   if(W->NumLightingUnbuiltObjects)return Fail(FString::Printf(TEXT("unbuilt lighting objects: %u"),W->NumLightingUnbuiltObjects));
   UE_LOG(LogTemp,Display,TEXT("DAS_STARTER_LIGHTING=PASS unbuilt_objects=0"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
  }
  auto Int=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FIntProperty>(O->GetClass(),N):nullptr;return P?P->GetPropertyValue_InContainer(O):INDEX_NONE;};
  auto Object=[](UObject* O,const TCHAR* N){auto* P=O?FindFProperty<FObjectPropertyBase>(O->GetClass(),N):nullptr;return P?P->GetObjectPropertyValue_InContainer(O):nullptr;};
  auto Error=[](UObject* O){auto* P=O?FindFProperty<FStrProperty>(O->GetClass(),TEXT("LastDasError")):nullptr;return P?P->GetPropertyValue_InContainer(O):FString(TEXT("missing DAS host"));};
  auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
  if(S->Stage==0){
   auto* PC=W->GetFirstPlayerController();auto* Player=PC?Cast<ACharacter>(PC->GetPawn()):nullptr;AActor* Item=nullptr;for(const auto& A:W->PersistentLevel->Actors)if(A && A->GetFName()==TEXT("Starter_Pickup"))Item=A;
   if(!Player || !Item)return Fail(TEXT("saved scene/possession missing"));auto* Logic=Object(Player,TEXT("Logic"));
   if(!Error(Player).IsEmpty() || !Error(Item).IsEmpty() || Int(Player,TEXT("LastPulse"))!=1 || Int(Logic,TEXT("Starts"))!=1)return Fail(Error(Player)+Error(Item)+TEXT(" project modules/component startup"));
   const FString Project=FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()/TEXT("Das/Scripts"));
   if(!R.ScriptPath(TEXT("starter_values.das")).StartsWith(Project))return Fail(TEXT("not using project scripts"));
   UE_LOG(LogTemp,Display,TEXT("PASS starter_project_scripts_component_and_possession"));
   Player->GetCharacterMovement()->DisableMovement();Player->SetActorLocation(Item->GetActorLocation()+FVector(0,0,40));Player->GetCapsuleComponent()->UpdateOverlaps();
   if(Int(Player,TEXT("Score"))!=10 || Int(Item,TEXT("Pickups"))!=1 || Int(Logic,TEXT("Count"))!=1)return Fail(TEXT("real overlap/native bindings/authored call failed"));
   UE_LOG(LogTemp,Display,TEXT("PASS starter_native_collision_and_authored_score"));Player->SetActorLocation(FVector(-500,0,100));S->Player=Player;S->Item=Item;S->Stage=1;return true;
  }
  auto* Player=S->Player.Get();auto* Item=S->Item.Get();if(!Player || !Item || !Error(Player).IsEmpty() || !Error(Item).IsEmpty())return Fail(TEXT("example expired/runtime error"));
  if(S->Stage==1){
   if(Int(Item,TEXT("Restores"))<1 || Int(Player,TEXT("LastPulse"))!=11)return true;
   UE_LOG(LogTemp,Display,TEXT("PASS starter_respawn_and_project_import"));
   S->File=R.ScriptPath(TEXT("starter_values.das"));if(!FFileHelper::LoadFileToString(S->Original,*S->File) || !S->Original.Contains(TEXT("return 1")))return Fail(TEXT("reload source missing"));
   const FString Changed=S->Original.Replace(TEXT("return 1"),TEXT("return 2"))+TEXT("\n");
   if(!FFileHelper::SaveStringToFile(Changed,*S->File,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return Fail(TEXT("cannot edit project helper"));S->Stage=2;return true;
  }
  if(S->Stage==2){
   if(Int(Player,TEXT("LastPulse"))!=12)return true;auto* Logic=Object(Player,TEXT("Logic"));
   if(Int(Player,TEXT("Score"))!=10 || Int(Logic,TEXT("Count"))!=1 || Int(Logic,TEXT("Starts"))!=1)return Fail(TEXT("reload lost object state"));
   if(!FFileHelper::SaveStringToFile(S->Original,*S->File,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))return Fail(TEXT("cannot restore helper"));S->Stage=3;return true;
  }
  if(Int(Player,TEXT("LastPulse"))!=11)return true;
  UE_LOG(LogTemp,Display,TEXT("PASS starter_import_hot_reload_preserves_state_and_restores_source"));
  UE_LOG(LogTemp,Display,TEXT("DAS_STARTER_RESULT=PASS"));FPlatformMisc::RequestExitWithStatus(false,0);return false;
 }));
}
class FDasStarterModule:public FDefaultGameModuleImpl {
 FDelegateHandle Handle;
public:
 virtual void StartupModule()override {if(FParse::Param(FCommandLine::Get(),TEXT("DasStarterProbe")) || FParse::Param(FCommandLine::Get(),TEXT("DasStarterLightingProbe")))Handle=FCoreDelegates::OnFEngineLoopInitComplete.AddStatic(&StarterProbe);}
 virtual void ShutdownModule()override {if(Handle.IsValid())FCoreDelegates::OnFEngineLoopInitComplete.Remove(Handle);}
};
IMPLEMENT_PRIMARY_GAME_MODULE(FDasStarterModule,DasStarter,"DasStarter")
