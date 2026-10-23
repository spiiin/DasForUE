#include "DasSubsystems.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Settings/LevelEditorPlaySettings.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
#include "UObject/UObjectGlobals.h"
#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
namespace
{
int32 SubsystemInt(UObject* O, const TCHAR* Name)
{
    auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), Name) : nullptr;
    return P ? P->GetPropertyValue_InContainer(O) : -1;
}
int32 AdvanceSubsystem(UObject* O)
{
    auto* F = O ? O->FindFunction(TEXT("Advance")) : nullptr;
    if (!F)
        return -1;
    int32 Value = 0;
    O->ProcessEvent(F, &Value);
    return Value;
}
struct FSubsystemPIEState
{
    TStrongObjectPtr<UBlueprint> WorldBP, GIBP;
    TArray<TWeakObjectPtr<UDasWorldSubsystem>> Worlds;
    TArray<UDasWorldSubsystem*> ObserveWorlds;
    TArray<bool> WorldEnds;
    TArray<TWeakObjectPtr<UDasGameInstanceSubsystem>> Instances;
    TArray<UDasGameInstanceSubsystem*> ObserveInstances;
    TArray<bool> InstanceEnds;
    FDelegateHandle BeforeGC;
    FString File, Path, Source;
    EPlayNetMode OldMode = PIE_Standalone;
    int32 OldClients = 1;
    bool OldSingle = true;
    bool Passed = true;
    ~FSubsystemPIEState()
    {
        FCoreUObjectDelegates::GetPreGarbageCollectDelegate().Remove(BeforeGC);
        auto* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
        Settings->SetPlayNetMode(OldMode);
        Settings->SetPlayNumberOfClients(OldClients);
        Settings->SetRunUnderOneProcess(OldSingle);
        if (!Path.IsEmpty())
            IFileManager::Get().Delete(*Path, false, true);
    }
};
class FSubsystemPIECommand : public IAutomationLatentCommand
{
    TSharedRef<FSubsystemPIEState> S;
    FAutomationTestBase* Test;
    bool Ending = false;
    double Started = FPlatformTime::Seconds();

public:
    FSubsystemPIECommand(TSharedRef<FSubsystemPIEState> In, FAutomationTestBase* T) : S(In), Test(T) {}
    bool Update() override
    {
        auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
        auto Check = [&](bool OK, const TCHAR* Name)
        {
            S->Passed &= Test->TestTrue(Name, OK);
            UE_LOG(LogTemp, Display, TEXT("DAS_SUBSYSTEMS_PIE_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        };
        TArray<UWorld*> Active;
        for (const auto& Context : GEngine->GetWorldContexts())
            if (Context.WorldType == EWorldType::PIE && Context.World() && Context.World()->HasBegunPlay())
                Active.Add(Context.World());
        if (Ending)
        {
            if ((!Active.IsEmpty() || GEditor->PlayWorld) && FPlatformTime::Seconds() - Started < 45)
                return false;
            Check(Active.IsEmpty() && !GEditor->PlayWorld, TEXT("all_PIE_worlds_ended"));
            Check(S->WorldEnds.Num() == S->Worlds.Num() && !S->WorldEnds.IsEmpty(),
                  TEXT("world_deinitialize_observed_before_GC"));
            for (bool Ended : S->WorldEnds)
                Check(Ended, TEXT("WorldSubsystem_deinitialize_once"));
            for (bool Ended : S->InstanceEnds)
                Check(Ended, TEXT("GameInstanceSubsystem_deinitialize_once"));
            for (const auto& W : S->Worlds)
                Check(!W.IsValid(), TEXT("PIE_world_service_is_collected"));
            for (const auto& GI : S->Instances)
                Check(!GI.IsValid(), TEXT("PIE_session_service_is_collected"));
            Check(R.PersistentContextCount() == 0 && R.ScriptTimerCount() == 0 && R.SubscriptionCount() == 0,
                  TEXT("EndPIE_releases_all_sessions_and_callbacks"));
            UE_LOG(LogTemp, Display, TEXT("DAS_SUBSYSTEMS_PIE_RESULT=%s"), S->Passed ? TEXT("PASS") : TEXT("FAIL"));
            return true;
        }
        if (Active.Num() < 2 && FPlatformTime::Seconds() - Started < 60)
            return false;
        Check(Active.Num() >= 2, TEXT("multiple_real_PIE_worlds"));
        for (auto* World : Active)
        {
            auto* W = Cast<UDasWorldSubsystem>(World->GetSubsystemBase(S->WorldBP->GeneratedClass.Get()));
            auto* GI = World->GetGameInstance()
                           ? Cast<UDasGameInstanceSubsystem>(
                                 World->GetGameInstance()->GetSubsystemBase(S->GIBP->GeneratedClass.Get()))
                           : nullptr;
            Check(W && GI && W->GetWorld() == World && GI->GetGameInstance() == World->GetGameInstance(),
                  TEXT("native_collection_owner_per_PIE_world"));
            if (W && GI)
            {
                S->Worlds.Emplace(W);
                S->Instances.Emplace(GI);
            }
        }
        if (S->Worlds.Num() >= 2)
        {
            for (const auto& W : S->Worlds)
                if (auto* F = W->FindFunction(TEXT("ArmTimer")))
                    W->ProcessEvent(F, nullptr);
            for (const auto& GI : S->Instances)
                if (auto* F = GI->FindFunction(TEXT("ArmTimer")))
                    GI->ProcessEvent(F, nullptr);
            Check(R.ScriptTimerCount() == S->Worlds.Num() + S->Instances.Num(),
                  TEXT("owned_timers_armed_for_each_service"));
            Check(S->Worlds[0].Get() != S->Worlds[1].Get() && S->Instances[0].Get() != S->Instances[1].Get(),
                  TEXT("PIE_instances_are_distinct"));
            Check(AdvanceSubsystem(S->Worlds[0].Get()) == 1 && SubsystemInt(S->Worlds[1].Get(), TEXT("Score")) == 0,
                                                                            TEXT("PIE_world_state_isolated"));
            Check(AdvanceSubsystem(S->Instances[0].Get()) == 1 &&
                  SubsystemInt(S->Instances[1].Get(), TEXT("Score")) == 0, TEXT("PIE_session_state_isolated"));
            FFileHelper::SaveStringToFile(S->Source.Replace(TEXT("self.Score += 1"), TEXT("self.Score += 10")),
                                                            *S->Path,
                                                            FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
            Check(AdvanceSubsystem(S->Worlds[0].Get()) == 11 && AdvanceSubsystem(S->Worlds[1].Get()) == 10,
                  TEXT("reload_updates_both_PIE_VMs_independently"));
            Check(R.ScriptTimerCount() == S->Worlds.Num() + S->Instances.Num(),
                  TEXT("reload_keeps_one_owned_timer_per_service"));
            Check(SubsystemInt(S->Worlds[0].Get(), TEXT("Starts")) == 1 &&
                               SubsystemInt(S->Worlds[1].Get(), TEXT("Starts")) == 1,
                                            TEXT("reload_does_not_replay_initialize"));
        }
        // Observational raw addresses are not GC roots. They are used only in PreGC while memory is alive,
        // then cleared as soon as native Deinitialize has been observed; no pointer survives collection.
        for (const auto& W : S->Worlds)
            S->ObserveWorlds.Add(W.Get());
        for (const auto& GI : S->Instances)
            S->ObserveInstances.Add(GI.Get());
        S->WorldEnds.Init(false, S->Worlds.Num());
        S->InstanceEnds.Init(false, S->Instances.Num());
        S->BeforeGC = FCoreUObjectDelegates::GetPreGarbageCollectDelegate().AddLambda(
            [Weak = TWeakPtr<FSubsystemPIEState>(S)]
            {
                if (auto State = Weak.Pin())
                {
                    for (int I = 0; I < State->ObserveWorlds.Num(); ++I)
                        if (auto* W = State->ObserveWorlds[I]; W && (!IsValid(W) || !W->IsDasActive()))
                        {
                            State->WorldEnds[I] = SubsystemInt(W, TEXT("Stops")) == 1 && !W->IsDasActive();
                            State->ObserveWorlds[I] = nullptr;
                        }
                    for (int I = 0; I < State->ObserveInstances.Num(); ++I)
                        if (auto* GI = State->ObserveInstances[I]; GI && (!IsValid(GI) || !GI->IsDasActive()))
                        {
                            State->InstanceEnds[I] = SubsystemInt(GI, TEXT("Stops")) == 1 && !GI->IsDasActive();
                            State->ObserveInstances[I] = nullptr;
                        }
                }
            });
        GEditor->RequestEndPlayMap();
        Ending = true;
        Started = FPlatformTime::Seconds();
        return false;
    }
};
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDasSubsystemsPIETest, "DasForUE.Subsystems.PIE",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FDasSubsystemsPIETest::RunTest(const FString&)
{
    if (!TestTrue(TEXT("No existing PIE session"), !GEditor->PlayWorld))
        return false;
    auto S = MakeShared<FSubsystemPIEState>();
    auto* Settings = GetMutableDefault<ULevelEditorPlaySettings>();
    Settings->GetPlayNetMode(S->OldMode);
    Settings->GetPlayNumberOfClients(S->OldClients);
    Settings->GetRunUnderOneProcess(S->OldSingle);
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits), Root = TEXT("/Game/DasSubsystems/PIE_") + Tag;
    S->File = TEXT("subsystems_pie_") + Tag + TEXT(".das");
    S->Path = R.ScriptPath(S->File);
    FFileHelper::LoadFileToString(S->Source, *R.ScriptPath(TEXT("dsl_world_subsystem.das")));
    FFileHelper::SaveStringToFile(S->Source, *S->Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    FAutomationEditorCommonUtils::CreateNewMap();
    UBlueprint *W = nullptr, *GI = nullptr;
    FString Error;
    if (!TestTrue(TEXT("Apply world service"),
                       FDasDSLService::Apply(S->File, Root + TEXT("/BP_Missions"), TEXT("subsystemdemo.MissionService"),
                                                                                        W, Error)))
    {
        AddError(Error);
        return false;
    }
    S->WorldBP.Reset(W);
    if (!TestTrue(TEXT("Apply session service"),
                       FDasDSLService::Apply(TEXT("dsl_gameinstance_subsystem.das"),
                                                  Root + TEXT("/BP_Session"), TEXT("subsystemdemo.SessionService"), GI,
                                                                                   Error)))
    {
        AddError(Error);
        return false;
    }
    S->GIBP.Reset(GI);
    Settings->SetPlayNetMode(PIE_Standalone);
    Settings->SetPlayNumberOfClients(2);
    Settings->SetRunUnderOneProcess(true);
    ADD_LATENT_AUTOMATION_COMMAND(FStartPIECommand(false));
    FAutomationTestFramework::Get().EnqueueLatentCommand(MakeShared<FSubsystemPIECommand>(S, this));
    return true;
}
#endif
