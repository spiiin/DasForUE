#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Misc/FileHelper.h"
#include "DasRuntime.h"
#include "DasExtendedDefaultsProbe.h"
#include "DasDefaultsMergeProbe.h"
#include "DasInheritedDefaultsProbe.h"
#include "DasComputedDefaultsProbe.h"
#include "DasNativeDefaultsProbe.h"
#include "Engine/World.h"
#include "DasActorComponentProbe.h"
#include "DasMethodValuesProbe.h"
#include "Engine/Engine.h"
#include "Engine/Level.h"

bool RunDasRegisteredCooked(FString& Report);
void StartDasClassCallbacksValidation();
void StartDasSubsystemsValidation();
void StartDasSaveGameCompatibilityValidation();
void StartDasSoftReferencesValidation();
void StartDasCookedGameplayValidation();
void StartDasPlayableMapValidation();
void StartDasPickupValidation();
void StartDasGameplayFeaturesValidation();
void StartDasInputUIValidation();
void StartDasMontageValidation();
void StartDasAIValidation();
void StartDasEffectsValidation();
void StartDasCookedCallsValidation();

class FDasForUESmokeModule : public FDefaultGameModuleImpl
{
    FDelegateHandle InitHandle;
public:
    virtual void StartupModule() override
    {
        if(FParse::Param(FCommandLine::Get(),TEXT("DasSubsystemsPackaged"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasSubsystemsValidation();});return;}
        if (FParse::Param(FCommandLine::Get(),TEXT("DasSaveGameCompatibility"))) {
            InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasSaveGameCompatibilityValidation();});return;
        }
        if(FParse::Param(FCommandLine::Get(),TEXT("DasPerformance"))){
            InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{
                int32 Iterations=64,Samples=9;FString Output,Report,Json;
                FParse::Value(FCommandLine::Get(),TEXT("PerfIterations="),Iterations);FParse::Value(FCommandLine::Get(),TEXT("PerfSamples="),Samples);
                FParse::Value(FCommandLine::Get(),TEXT("PerfOutput="),Output);
                bool Passed=RunDasPerformanceProbe(Report,Json,Iterations,Samples);
                if(!Output.IsEmpty())Passed &= FFileHelper::SaveStringToFile(Json,*Output,FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
                UE_LOG(LogTemp,Display,TEXT("%s"),*Report);UE_LOG(LogTemp,Display,TEXT("DAS_PERFORMANCE_RESULT=%s"),Passed?TEXT("PASS"):TEXT("FAIL"));
                FPlatformMisc::RequestExitWithStatus(false,Passed?0:1);
            });return;
        }
        if(FParse::Param(FCommandLine::Get(),TEXT("DasNativeDefaults"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld()){FString Report;Good=RunDasNativeDefaultsProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_NATIVE_DEFAULTS_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});return;}
        if(FParse::Param(FCommandLine::Get(),TEXT("DasComputedDefaults"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld()){FString Report;Good=RunDasComputedDefaultsProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_COMPUTED_DEFAULTS_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasInheritedDefaults"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld()){FString Report;Good=RunDasInheritedDefaultsProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_INHERITED_DEFAULTS_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasDefaultsMerge"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld()){FString Report;Good=RunDasDefaultsMergeProbe(World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_DEFAULTS_MERGE_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasExtendedDefaults"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{bool Good=false;for(const auto& Context:GEngine->GetWorldContexts())if(auto* World=Context.World())if(World->IsGameWorld())for(const auto& Actor:TArray<TObjectPtr<AActor>>(World->PersistentLevel->Actors))if(Actor && Actor->ActorHasTag(TEXT("ExtendedDefaultsFixture"))){FString Report;Good=RunDasExtendedDefaultsProbe(Actor->GetClass(),World,Report);UE_LOG(LogTemp,Display,TEXT("%s"),*Report);}UE_LOG(LogTemp,Display,TEXT("DAS_EXTENDED_DEFAULTS_PACKAGED=%s"),Good?TEXT("PASS"):TEXT("FAIL"));FPlatformMisc::RequestExitWithStatus(false,Good?0:1);});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasCookedCalls"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasCookedCallsValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasAI"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasAIValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasEffects"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasEffectsValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasMontage"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasMontageValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasInputUI"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasInputUIValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasGameplayFeatures"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasGameplayFeaturesValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasPickup"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasPickupValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasPlayableMap"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasPlayableMapValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasSoftReferences"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasSoftReferencesValidation();});}
        else if(FParse::Param(FCommandLine::Get(),TEXT("DasClassCallbacks"))){InitHandle=FCoreDelegates::OnFEngineLoopInitComplete.AddLambda([]{StartDasClassCallbacksValidation();});}
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
