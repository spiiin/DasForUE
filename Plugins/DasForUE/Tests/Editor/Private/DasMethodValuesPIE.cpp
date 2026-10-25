#include "DasRuntime.h"
#include "DasActor.h"
#include "Engine/Blueprint.h"
#include "EngineUtils.h"
#include "Editor.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "UObject/StrongObjectPtr.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
struct FMethodValuesPIEState
{
    TStrongObjectPtr<UBlueprint> Driver, Target;
    bool Passed = true;
};
class FMethodValuesPIECommand : public IAutomationLatentCommand
{
    TSharedRef<FMethodValuesPIEState> State;
    FAutomationTestBase* Test;
    bool Ending = false;
    double Started = FPlatformTime::Seconds();

public:
    FMethodValuesPIECommand(TSharedRef<FMethodValuesPIEState> S, FAutomationTestBase* T) : State(S), Test(T) {}
    bool Update() override
    {
        auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        if (Ending)
        {
            if (GEditor->PlayWorld && FPlatformTime::Seconds() - Started < 30)
                return false;
            State->Passed &= Test->TestTrue(TEXT("EndPIE releases natural method contexts"),
                                                 !GEditor->PlayWorld && Runtime.PersistentContextCount() == 0);
            UE_LOG(LogTemp, Display,
                   TEXT("DAS_METHOD_VALUES_PIE_RESULT=%s"), State->Passed ? TEXT("PASS"): TEXT("FAIL"));
            return true;
        }
        auto* World = GEditor->PlayWorld.Get();
        ADasActor* Driver = nullptr;
        ADasActor* Target = nullptr;
        if (World)
            for (TActorIterator<ADasActor> It(World); It; ++It)
            {
                if (It->GetFName() == TEXT("MethodValueDriver"))
                    Driver = *It;
                if (It->GetFName() == TEXT("MethodValueTarget"))
                    Target = *It;
            }
        if (!World || !World->HasBegunPlay() || !Driver || !Target)
        {
            if (FPlatformTime::Seconds() - Started < 60)
                return false;
            Test->AddError(TEXT("Natural method PIE startup timeout"));
            State->Passed = false;
            GEditor->RequestEndPlayMap();
            Ending = true;
            Started = FPlatformTime::Seconds();
            return false;
        }
        struct FArgs
        {
            UObject* Target;
            int32 Result = 0;
        };
        FArgs Args{Target};
        auto* Function = Driver->FindFunction(TEXT("Exercise"));
        if (Function)
            Driver->ProcessEvent(Function, &Args);
        State->Passed &= Test->TestTrue(TEXT("PIE natural enum nested struct result ref/out and recover"),
                                             Function && Args.Result == 1 && Driver->LastDasError.IsEmpty());
        GEditor->RequestEndPlayMap();
        Ending = true;
        Started = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasMethodValuesPIETest, "DasForUE.MethodValues.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasMethodValuesPIETest::RunTest(const FString&)
{
    FString Id;
    FParse::Value(FCommandLine::Get(), TEXT("DasMethodValuesProbeId="), Id);
    FGuid Guid;
    if (!TestTrue(TEXT("Valid prepared probe ID"), FGuid::ParseExact(Id, EGuidFormats::Digits, Guid)))
        return false;
    const FString Base = TEXT("/Game/DasMethodValues/")+ Id;
    auto State = MakeShared<FMethodValuesPIEState>();
    State->Driver.Reset(LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Driver.BP_Driver"))));
    State->Target.Reset(LoadObject<UBlueprint>(nullptr, *(Base + TEXT("/BP_Target.BP_Target"))));
    if (!TestTrue(TEXT("Prepared natural method Blueprints"), State->Driver.IsValid() && State->Target.IsValid()))
        return false;
    auto* World = FAutomationEditorCommonUtils::CreateNewMap();
    FActorSpawnParameters Spawn;
    Spawn.Name = TEXT("MethodValueTarget");
    World->SpawnActor<ADasActor>(State->Target->GeneratedClass, FTransform::Identity, Spawn);
    Spawn.Name = TEXT("MethodValueDriver");
    World->SpawnActor<ADasActor>(State->Driver->GeneratedClass, FTransform::Identity, Spawn);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FMethodValuesPIECommand>(State, this));
    return true;
}
#endif
