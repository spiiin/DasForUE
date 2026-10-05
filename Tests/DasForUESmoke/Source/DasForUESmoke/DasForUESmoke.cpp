#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/CoreDelegates.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "DasRuntime.h"

class FDasForUESmokeModule : public FDefaultGameModuleImpl
{
    FDelegateHandle InitHandle;
public:
    virtual void StartupModule() override
    {
        if (FParse::Param(FCommandLine::Get(), TEXT("DasSmoke")))
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
