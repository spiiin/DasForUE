#include "Modules/ModuleManager.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Containers/Ticker.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/UnrealType.h"
#if !WITH_EDITOR && (!defined(DAS_WITH_TEST_PROBES) || DAS_WITH_TEST_PROBES)
#error Ordinary game targets must not include DAS test probes.
#endif
class FDasIsolationGameModule : public FDefaultGameModuleImpl
{
    FDelegateHandle Init;
    FTSTicker::FDelegateHandle Ticker;

public:
    virtual void StartupModule() override
    {
        if (!FParse::Param(FCommandLine::Get(), TEXT("DasIsolation")))
            return;
        Init = FCoreDelegates::OnFEngineLoopInitComplete.AddLambda(
            [this]
            {
                UWorld* World = nullptr;
                for (const auto& Context : GEngine->GetWorldContexts())
                    if (Context.World() && Context.World()->IsGameWorld())
                        World = Context.World();
                auto* Class =
                    LoadClass<ADasActor>(nullptr, TEXT("/Game/RuntimeIsolation/BP_IsolationActor.BP_IsolationActor_C"));
                const bool Clean = !FindObject<UClass>(nullptr, TEXT("/Script/DasRuntime.DasProbeActor"));
                auto* Actor = World && Class && Clean ? World->SpawnActor<ADasActor>(Class) : nullptr;
                if (!Actor)
                {
                    UE_LOG(LogTemp, Error, TEXT("DAS_ISOLATION_PACKAGE=FAIL spawn or fixture leakage"));
                    FPlatformMisc::RequestExitWithStatus(false, 1);
                    return;
                }
                TWeakObjectPtr<ADasActor> Weak = Actor;
                const double Start = FPlatformTime::Seconds();
                Ticker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
                    [Weak, Start](float)
                    {
                        auto* A = Weak.Get();
                        auto ReadInt = [A](const TCHAR* Name)
                        {
                            auto* P = A ? FindFProperty<FIntProperty>(A->GetClass(), Name) : nullptr;
                            return P ? P->GetPropertyValue_InContainer(A) : -1;
                        };
                        auto* Ready = A ? FindFProperty<FBoolProperty>(A->GetClass(), TEXT("Ready")) : nullptr;
                        if (A && Ready && Ready->GetPropertyValue_InContainer(A) && ReadInt(TEXT("Ticks")) > 0)
                        {
                            struct
                            {
                                int32 Input = 5;
                                int32 Result = 0;
                            } Args;
                            auto* Function = A->FindFunction(TEXT("Add"));
                            if (Function)
                                A->ProcessEvent(Function, &Args);
                            const bool Good = Function && Args.Result == 15 &&
                                              ReadInt(TEXT("Score")) == 15 &&
                                                      ReadInt(TEXT("Begins")) == 1 && A->GetRootComponent() &&
                                                              A->LastDasError.IsEmpty() &&
                                                              !A->GetClass()->FindPropertyByName(TEXT("Peer"));
                            UE_LOG(LogTemp, Display,
                                   TEXT("DAS_ISOLATION_PACKAGE=%s"), Good ? TEXT("PASS") : TEXT("FAIL"));
                            A->Destroy();
                            FPlatformMisc::RequestExitWithStatus(false, Good ? 0 : 1);
                            return false;
                        }
                        if (!A || FPlatformTime::Seconds() - Start > 30)
                        {
                            UE_LOG(LogTemp, Error, TEXT("DAS_ISOLATION_PACKAGE=FAIL lifecycle/callback timeout"));
                            FPlatformMisc::RequestExitWithStatus(false, 1);
                            return false;
                        }
                        return true;
                    }));
            });
    }
    virtual void ShutdownModule() override
    {
        FCoreDelegates::OnFEngineLoopInitComplete.Remove(Init);
        FTSTicker::GetCoreTicker().RemoveTicker(Ticker);
    }
};
IMPLEMENT_PRIMARY_GAME_MODULE(FDasIsolationGameModule, DasRuntimeIsolation, "DasRuntimeIsolation");
