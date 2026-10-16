#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "DasRuntime.h"
#include "DasExtendedDefaultsProbe.h"
#include "DasDefaultsMergeProbe.h"
#include "DasInheritedDefaultsProbe.h"
#include "Engine/World.h"
#include "DasActorComponentProbe.h"
#include "DasMethodValuesProbe.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"

bool RunDasRegisteredCooked(FString& Report);
void StartDasCookedGameplayValidation();
void StartDasCookedCallsValidation();

class FDasForUESmokeModule : public FDefaultGameModuleImpl
{
    FDelegateHandle InitHandle;
public:
    virtual void StartupModule() override
    {
        if(FParse::Param(FCommandLine::Get(),TEXT("DasInheritedDefaults"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld()){FString Report;Good=RunDasInheritedDefaultsProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_INHERITED_DEFAULTS_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasDefaultsMerge"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld()){FString Report;Good=RunDasDefaultsMergeProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULTS_MERGE_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasExtendedDefaults"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld())for(const auto& Actor:TArray<TObjectPtr<AActor>>(World->PersistentLevel->Actors))if(Actor && Actor->ActorHasTag(TEXT("ExtendedDefaultsFixture"))){FString Report;Good=RunDasExtendedDefaultsProbe(Actor->GetClass(),World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_EXTENDED_DEFAULTS_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasCookedCalls"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasCookedCallsValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasCookedGameplay"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasCookedGameplayValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasMethodValues"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{UClass* Driver=nullptr;UClass* Target=nullptr;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld())for(const auto& Actor:World->PersistentLevel->Actors)if(auto* Probe=Cast<ADasProbeActor>(Actor)){if(Probe->Peer){Driver=Probe->GetClass();Target=Probe->Peer->GetClass();}}FString Report;const bool Good=Driver && Target && RunDasMethodValuesProbe(Driver,Target,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasActorComponent"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{UClass* Class=nullptr;for(const auto& Context:GEngine->GetWorldContexts())if(auto* W=Context.World())if(W->IsGameWorld())for(const auto& A:W->PersistentLevel->Actors)if(A && A->IsA<ADasProbeActor>() && A->GetClass()!=ADasProbeActor::StaticClass())Class=A->GetClass();FString Report;const bool Good=Class && RunDasActorComponentProbe(Class,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasRegisteredCooked"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{FString Report;const bool Success=RunDasRegisteredCooked(Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);FPlatformMisc::RequestExitWithStatus(false,Success?0:1);});}
        else if (FParse::Param(FCommandLine::Get(), TEXT("DasSmoke")))
        {
            InitHandle = FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]
            {
                FString Report;
                bool Success = RunDasFoundationTests(Report);
                UE_LOG(LogTemp, Display, TEXT("%s"), *Report);
                FPlatformMisc::RequestExitWithStatus(false, Success ? 0 : 1);
            });
        }
    }
    virtual void ShutdownModule() override
    {
        FCoreDelegates::OnFEngineLoopInitComplete.Remove(InitHandle);
    }
};
IMPLEMENT_PRIMARY_GAME_MODULE(FDasForUESmokeModule, DasForUESmoke, "DasForUESmoke");
