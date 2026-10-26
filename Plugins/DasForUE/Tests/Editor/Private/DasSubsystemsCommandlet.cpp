#include "DasSubsystemsCommandlet.h"
#include "DasSubsystems.h"
#include "DasDSLService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Engine/Blueprint.h"
#include "Engine/GameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"
UDasSubsystemsCommandlet::UDasSubsystemsCommandlet()
{
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasSubsystemsCommandlet::Main(const FString&)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int Checks = 0, Failures = 0;
    auto Check = [&](bool OK, const TCHAR* Name, const FString& Error = FString())
    {
        ++Checks;
        Failures += !OK;
        UE_LOG(LogTemp, Display, TEXT("DAS_SUBSYSTEMS_%s %s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name, *Error);
    };
    const FString Tag = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString WorldFile = TEXT("world_subsystem_") + Tag + TEXT(".das");
    const FString LookupFile = TEXT("subsystem_lookup_") + Tag + TEXT(".das");
    const FString GIFile = TEXT("gi_subsystem_") + Tag + TEXT(".das");
    FString WorldSource, GISource, Error;
    FFileHelper::LoadFileToString(WorldSource, *R.ScriptPath(TEXT("dsl_world_subsystem.das")));
    FFileHelper::LoadFileToString(GISource, *R.ScriptPath(TEXT("dsl_gameinstance_subsystem.das")));
    auto Write = [&](const FString& File, const FString& Source)
    {
        return FFileHelper::SaveStringToFile(Source, *R.ScriptPath(File),
                                             FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
    };
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*R.ScriptPath(WorldFile));
        IFileManager::Get().Delete(*R.ScriptPath(GIFile));
        IFileManager::Get().Delete(*R.ScriptPath(LookupFile));
    };
    Write(WorldFile, WorldSource);
    Write(GIFile, GISource);
    UBlueprint *WorldBP = nullptr, *GIBP = nullptr;
    const FString Root = TEXT("/Game/DasSubsystems/") + Tag;
    Check(FDasDSLService::Apply(WorldFile, Root + TEXT("/BP_Missions"),
                                                       TEXT("subsystemdemo.MissionService"), WorldBP, Error),
                                               TEXT("Apply_WorldSubsystem"), Error);
    Check(FDasDSLService::Apply(GIFile, Root + TEXT("/BP_Session"), TEXT("subsystemdemo.SessionService"), GIBP, Error),
                                            TEXT("Apply_GameInstanceSubsystem"), Error);
    if (!WorldBP || !GIBP)
        return 1;
    TStrongObjectPtr<UBlueprint> KeepW(WorldBP), KeepG(GIBP);
    auto Read = [](UObject* O, const TCHAR* Name)
    {
        auto* P = O ? FindFProperty<FIntProperty>(O->GetClass(), Name) : nullptr;
        return P ? P->GetPropertyValue_InContainer(O) : -1;
    };
    auto Advance = [](UObject* O)
    {
        if (O)
            if (auto* F = O->FindFunction(TEXT("Advance")))
            {
                int Result = 0;
                O->ProcessEvent(F, &Result);
                return Result;
            }
        return -1;
    };
    TStrongObjectPtr<UGameInstance> A(NewObject<UGameInstance>(GEngine)), B(NewObject<UGameInstance>(GEngine));
    TStrongObjectPtr<UGameInstance> NoWorld(NewObject<UGameInstance>(GEngine));
    NoWorld->Init();
    auto* Early = NoWorld->GetSubsystemBase(GIBP->GeneratedClass.Get());
    Check(Early && !Early->GetWorld() && Read(Early, TEXT("Starts")) == 1, TEXT("GI_initialize_before_world"));
    NoWorld->Shutdown();
    A->InitializeStandalone();
    B->InitializeStandalone();
    auto* SA = Cast<UDasGameInstanceSubsystem>(A->GetSubsystemBase(GIBP->GeneratedClass.Get()));
    auto* SB = Cast<UDasGameInstanceSubsystem>(B->GetSubsystemBase(GIBP->GeneratedClass.Get()));
    Check(SA && SB && SA != SB &&
          Read(SA, TEXT("Starts")) == 1 && Read(SB, TEXT("Starts")) == 1,
                                                TEXT("GameInstances_independent_initialize_without_world"));
    if (!SA || !SB)
    {
        A->Shutdown();
        B->Shutdown();
        return 1;
    }
    Check(Advance(SA) == 1 && Advance(SA) == 2 && Read(SB, TEXT("Score")) == 0, TEXT("session_state_isolated"));
    TStrongObjectPtr<UWorld> W1(A->GetWorld()), W2(B->GetWorld());
    auto* M1 = Cast<UDasWorldSubsystem>(W1->GetSubsystemBase(WorldBP->GeneratedClass.Get()));
    auto* M2 = Cast<UDasWorldSubsystem>(W2->GetSubsystemBase(WorldBP->GeneratedClass.Get()));
    TStrongObjectPtr<UDasWorldSubsystem> KeepM1(M1), KeepM2(M2);
    Check(M1 && M2 && M1 != M2 &&
          Read(M1, TEXT("Starts")) == 1 && Read(M2, TEXT("Starts")) == 1,
                                                TEXT("world_collection_auto_creation_isolated"));
    Check(M1 && Advance(M1) == 1 && Read(M2, TEXT("Score")) == 0, TEXT("world_state_isolated"));
    FString Lookup = FString::Printf(
        TEXT("options gen2\nrequire ue_subsystems\n[export]\n")
            TEXT("def world_ref():UObjectHandle { return ue_subsystem(type<WorldSubsystemRef>,")
                TEXT("ue_class_loaded(type<WorldSubsystemRef>,\"%s\")).handle }\n[export]\n")
                    TEXT("def session_ref():UObjectHandle { return ue_subsystem(type<GameInstanceSubsystemRef>,")
                        TEXT("ue_class_loaded(type<GameInstanceSubsystemRef>,\"%s\")).handle }\n"),
                             *WorldBP->GeneratedClass->GetPathName(), *GIBP->GeneratedClass->GetPathName());
    Write(LookupFile, Lookup);
    auto WR =
        R.RunNamedFramePersistent(R.ScriptPath(LookupFile), TEXT("world_ref"), {}, EDasMethodSignature::Object, M1);
    auto GR =
        R.RunNamedFramePersistent(R.ScriptPath(LookupFile), TEXT("session_ref"), {}, EDasMethodSignature::Object, M1);
    Check(WR.bSucceeded && WR.ObjectValue.Get() == M1, TEXT("typed_world_lookup_returns_same_collection_instance"),
                                                            WR.Error);
    Check(GR.bSucceeded && GR.ObjectValue.Get() == SA, TEXT("typed_session_lookup_uses_owner_GameInstance"), GR.Error);
    Write(WorldFile, WorldSource.Replace(TEXT("self.Score += 1"), TEXT("self.Score += 10")));
    Check(M1 && Advance(M1) == 11 &&
          Read(M1, TEXT("Starts")) == 1, TEXT("body_reload_preserves_state_and_initialize_once"));
    Write(WorldFile, WorldSource + TEXT("\ninvalid syntax !!!\n"));
    Check(M1 && Advance(M1) == 21 && !M1->LastDasError.IsEmpty(), TEXT("failed_reload_retains_last_good_VM"));
    Write(WorldFile, WorldSource);
    W1->DestroyWorld(false);
    Check(M1 && Read(M1, TEXT("Stops")) == 1 && !M1->IsDasActive(), TEXT("world_deinitialize_once"));
    auto Stopped = R.RunNamedVoidPersistent(R.ScriptPath(WorldFile), TEXT("__ue_initialize"), M1);
    Check(!Stopped.bSucceeded && Stopped.Error.Contains(TEXT("deinitialized")),
                                                        TEXT("retained_reference_cannot_resurrect_stopped_VM"),
                                                             Stopped.Error);
    Check(Advance(SA) == 3 && SA->IsDasActive() &&
          Read(SA, TEXT("Starts")) == 1, TEXT("session_survives_world_cleanup"));
    Check(M2 && Advance(M2) == 1 && M2->IsDasActive(), TEXT("other_world_unaffected"));
    auto IV = UWorld::InitializationValues()
                  .CreatePhysicsScene(false)
                  .CreateNavigation(false)
                  .CreateAISystem(false)
                  .AllowAudioPlayback(false);
    TStrongObjectPtr<UWorld> W3(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IV));
    W3->SetGameInstance(A.Get());
    A->GetWorldContext()->SetCurrentWorld(W3.Get());
    Check(Advance(SA) == 4 && Read(SA, TEXT("Starts")) == 1 && A->GetSubsystemBase(GIBP->GeneratedClass.Get()) == SA,
                                   TEXT("session_state_survives_world_replacement"));
    auto* Fresh = W3->GetSubsystemBase(WorldBP->GeneratedClass.Get());
    Check(Fresh && Fresh != M1 &&
          Read(Fresh, TEXT("Starts")) == 1 && Read(Fresh, TEXT("Score")) == 0,
                                                   TEXT("replacement_world_has_fresh_service"));
    W3->DestroyWorld(false);
    W2->DestroyWorld(false);
    A->Shutdown();
    B->Shutdown();
    GEngine->DestroyWorldContext(W3.Get());
    GEngine->DestroyWorldContext(W2.Get());
    Check(Read(SA, TEXT("Stops")) == 1 && Read(SB, TEXT("Stops")) == 1 && !SA->IsDasActive() && !SB->IsDasActive(),
                                               TEXT("GameInstance_deinitialize_once"));
    Check(R.PersistentContextCount() == 0 && R.ScriptTimerCount() == 0 && R.SubscriptionCount() == 0,
          TEXT("teardown_releases_all_VM_resources"));
    FDasAuthoringModule Model;
    FDasClassSchema Schema;
    Write(WorldFile, WorldSource.Replace(TEXT("def Initialize()"), TEXT("def BeginPlay()")));
    const bool ReadActorLifecycle = R.ReadAuthoringScript(R.ScriptPath(WorldFile), Model, Error);
    Check((!ReadActorLifecycle || !Model.ToActorSchema(TEXT("subsystemdemo.MissionService"), Schema, Error)) &&
           Error.Contains(TEXT("Subsystem supports")), TEXT("actor_lifecycle_rejected"), Error);
    UE_LOG(LogTemp, Display,
           TEXT("DAS_SUBSYSTEMS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL") : TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
