#include "CoreMinimal.h"
#include "DasStaticLibraryProbe.h"
#include "Containers/Ticker.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
void StartDasPlayableMapValidation(){
 const double Started=FPlatformTime::Seconds();
 FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Started](float){
  for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World();World && World->IsGameWorld() && World->HasBegunPlay() && World->GetTimeSeconds()>.5f){FString Report;const bool Good=RunDasPlayableMapProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);UE_LOG(LogTemp,Display,TEXT("DAS_PLAYABLE_MAP_RESULT=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);return false;}
  if(FPlatformTime::Seconds()-Started>30){UE_LOG(LogTemp,Error,TEXT("DAS_PLAYABLE_MAP_RESULT=FAIL timeout"));FPlatformMisc::RequestExitWithStatus(false,1);return false;}return true;
 }));
}
