#include "DasExecutionRoleCommandlet.h"
#include "DasExecutionRole.h"
#include "DasExecutionGuardProbe.h"
#include "DasAsyncPhysicsBridge.h"
#include "DasCollisionProfileProbe.h"
#include "Engine/Engine.h"
#include "Components/SceneComponent.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
UDasExecutionRoleCommandlet::UDasExecutionRoleCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasExecutionRoleCommandlet::Main(const FString& Params)
{
    FString Path;
    if (!FParse::Value(*Params, TEXT("Probe="), Path))
        return 1;
    int32 Failures = 0, Checks = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_EXECUTION_ROLE_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
        if (!Good)
            ++Failures;
    };
    auto* Settings = GetMutableDefault<UPhysicsSettings>();
    const bool OldAsync = Settings->bTickPhysicsAsync;
    const float OldDt = Settings->AsyncFixedTimeStepSize;
    Settings->bTickPhysicsAsync = FParse::Param(*Params, TEXT("AsyncFixed"));
    Settings->AsyncFixedTimeStepSize = 1.f / 60.f;
    ON_SCOPE_EXIT
    {
        Settings->bTickPhysicsAsync = OldAsync;
        Settings->AsyncFixedTimeStepSize = OldDt;
    };
    const auto IVS = UWorld::InitializationValues()
                         .CreatePhysicsScene(true)
                         .ShouldSimulatePhysics(true)
                         .AllowAudioPlayback(false)
                         .CreateNavigation(false)
                         .CreateAISystem(false);
    TStrongObjectPtr<UWorld> World(
        UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS)),
        Other(UWorld::CreateWorld(EWorldType::Game, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &IVS));
    for (auto* W : {World.Get(), Other.Get()})
    {
        GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(W);
        W->InitializeActorsForPlay(FURL());
        W->BeginPlay();
        W->GetWorldSettings()->NotifyBeginPlay();
    }
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    ON_SCOPE_EXIT
    {
        UDasAsyncPhysicsBridge::RevokeAll();
        for (auto* W : {World.Get(), Other.Get()})
            if (W)
            {
                GEngine->DestroyWorldContext(W);
                W->DestroyWorld(false);
            }
    };
    // Spawned actors in a world without GameMode need the normal BeginPlay route.
    auto Owner = [&](UWorld* W)
    {
        auto* A = W->SpawnActor<ADasCollisionProfileProbe>();
        if (!A->HasActorBegunPlay())
            A->DispatchBeginPlay();
        return A;
    };
    auto* Actor = Owner(World.Get());
    auto* Foreign = Owner(Other.Get());
    auto Step = [&](int32 N)
    {
        for (int32 I = 0; I < N; ++I)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, 1.f / 60.f);
        }
    };
    FString Error;
    auto Probe = PrepareDasExecutionGuardProbeForTests(Path, Actor, Error);
    auto ForeignProbe = PrepareDasExecutionGuardProbeForTests(Path, Foreign, Error);
    auto NoSelfProbe = PrepareDasExecutionGuardProbeForTests(Path, nullptr, Error);
    if (!Probe || !ForeignProbe || !NoSelfProbe)
    {
        UE_LOG(LogTemp, Error, TEXT("probe compile: %s"), *Error);
        return 1;
    }
    Check(Probe->ExternRegistrationCount() >= 500, TEXT("central_factory_registration_coverage"));
    for (int32 Mode = 0; Mode <= 5; ++Mode)
    {
        auto R = Probe->Evaluate(Mode);
        Check(R.bSucceeded && R.Value == 42, *FString::Printf(TEXT("ordinary_scalar_CMRES_or_property_%d"), Mode));
        if (!R.bSucceeded)
            UE_LOG(LogTemp, Error, TEXT("%s"), *R.Error);
    }
    Actor->Counter = 7;
    FDasAsyncPhysicsStep Saved;
    int32 Count = 0;
    bool Matrix = false, OldExpired = false;
    auto* B = UDasAsyncPhysicsBridge::Create(
        Actor,
        [&](const FDasAsyncPhysicsStep& S, FString&)
        {
            ++Count;
            OldExpired |= Saved.Epoch && !Saved.IsCurrent();
            Saved = S;
            if (!Matrix)
            {
                Matrix = true;
                Check(S.IsCurrent() && GetDasExecutionRole() == EDasExecutionRole::PhysicsPreSim &&
                          IsDasFrozenPhysicsWorld(Actor),
                      TEXT("engine_issued_step_and_role"));
                const auto Pure = Probe->Evaluate(0);
                Check(Pure.bSucceeded && Pure.Value == 42, TEXT("pure_DAS_computation_in_prepared_test_context"));
                for (int32 Mode = 1; Mode <= 12; ++Mode)
                {
                    const auto R = Probe->Evaluate(Mode);
                    Check(!R.bSucceeded &&
                          R.Error.Contains(TEXT("UE extern is forbidden in PhysicsPreSim")),
                                           *FString::Printf(TEXT("UE_extern_family_rejected_%d"), Mode));
                }
                const auto Run = Runtime.RunFile(Path, 0, false, Actor);
                Check(!Run.bSucceeded && Run.Error.Contains(TEXT("gameplay VM entry")),
                                                            TEXT("gameplay_VM_entry_rejected_before_compile"));
                auto Fake = UDasAsyncPhysicsBridge::Create(Actor, [](const auto&, FString&) { return true; }, Error);
                Check(!Fake, TEXT("registration_forbidden_during_callback"));
            }
            return true;
        },
        Error);
    Check(B != nullptr, TEXT("bridge_registration"));
    if (!B)
        return 1;
    Step(5);
    Check(Count >= 2 && OldExpired && !Saved.IsCurrent(), TEXT("epoch_expires_on_exit_and_next_callback"));
    Check(GetDasExecutionRole() == EDasExecutionRole::Gameplay && Actor->Counter == 7 &&
              Runtime.ScriptTimerCount(Actor) == 0,
          TEXT("role_restored_no_property_or_timer_side_effects"));
    B->Revoke();
    const int32 Before = Count;
    Step(3);
    Check(Count == Before, TEXT("external_revoke_stops_callbacks"));
    int32 CancelCount = 0, PeerCount = 0;
    UDasAsyncPhysicsBridge* Cancel = nullptr;
    auto* Peer = UDasAsyncPhysicsBridge::Create(
        Actor,
        [&](const auto&, FString&)
        {
            ++PeerCount;
            return true;
        },
        Error);
    Cancel = UDasAsyncPhysicsBridge::Create(
        Actor,
        [&](const FDasAsyncPhysicsStep& S, FString&)
        {
            ++CancelCount;
            Cancel->Deactivate();
            Check(!S.IsCurrent() && !Cancel->IsBeingDestroyed(),
                  TEXT("deactivate_revokes_now_without_mutating_tick_iteration"));
            UDasAsyncPhysicsBridge::FlushDeferred();
            Check(!Cancel->IsBeingDestroyed(), TEXT("flush_inside_callback_is_deferred"));
            return true;
        },
        Error);
    Check(Peer && Cancel, TEXT("peer_and_cancel_registration"));
    Step(5);
    UDasAsyncPhysicsBridge::FlushDeferred();
    Check(CancelCount == 1 && PeerCount >= 2, TEXT("deferred_unregistration_preserves_peer_callback"));
    Peer->Revoke();
    int32 FailedCount = 0;
    auto* Failing = UDasAsyncPhysicsBridge::Create(
        Actor,
        [&](const auto&, FString& E)
        {
            ++FailedCount;
            E = TEXT("expected callback failure");
            return false;
        },
        Error);
    Step(4);
    Check(FailedCount == 1 && !Failing->IsLiveBinding() &&
          Failing->GetLastError() == TEXT("expected callback failure"), TEXT("callback_error_revokes_once"));
    UDasAsyncPhysicsBridge::FlushDeferred();
    int32 NestedCount = 0;
    auto* Nested = UDasAsyncPhysicsBridge::Create(
        Actor,
        [&](const auto&, FString&)
        {
            ++NestedCount;
            return true;
        },
        Error);
    bool NestedChecked = false;
    auto* Caller = UDasAsyncPhysicsBridge::Create(
        Actor,
        [&](const FDasAsyncPhysicsStep& S, FString&)
        {
            if (!NestedChecked)
            {
                NestedChecked = true;
                const int32 Previous = NestedCount;
                Nested->AsyncPhysicsTickComponent(S.DeltaTime, S.SimTime);
                Check(NestedCount == Previous && !Nested->IsLiveBinding(),
                      TEXT("nested_native_bridge_cannot_forge_second_scope"));
            }
            return true;
        },
        Error);
    Step(4);
    UDasAsyncPhysicsBridge::FlushDeferred();
    Caller->Revoke();
    Check(NestedChecked, TEXT("nested_scope_attempt_exercised"));
    // Engine callback with no bridge-issued TLS role must not bypass the guard by
    // selecting a foreign WorldContext or a static/no-self context.
    auto* Unscoped = NewObject<UDasUnscopedPhysicsRoleProbe>(Actor);
    Actor->AddInstanceComponent(Unscoped);
    Unscoped->RegisterComponent();
    Unscoped->Activate(true);
    bool UnscopedChecked = false;
    Unscoped->Callback = [&]
    {
        if (UnscopedChecked)
            return;
        UnscopedChecked = true;
        Check(GetDasExecutionRole() == EDasExecutionRole::Gameplay && IsDasFrozenPhysicsWorld(Actor),
              TEXT("unscoped_engine_physics_phase_detected"));
        for (auto* P : {ForeignProbe.Get(), NoSelfProbe.Get()})
        {
            const auto R = P->Evaluate(1);
            Check(!R.bSucceeded && R.Error.Contains(TEXT("PhysicsPreSim")),
                                                    TEXT("foreign_or_no_self_cannot_bypass_phase"));
        }
        const auto R = Runtime.RunFile(Path, 0, false, Foreign);
        Check(!R.bSucceeded && R.Error.Contains(TEXT("gameplay VM entry")), TEXT("foreign_gameplay_VM_entry_rejected"));
    };
    Unscoped->Enable(true);
    Step(4);
    Unscoped->Enable(false);
    Unscoped->DestroyComponent();
    Check(UnscopedChecked, TEXT("unscoped_callback_executed"));
    auto* ReloadOwner = Owner(World.Get());
    auto Initial = Runtime.RunPersistent(Path, 0, ReloadOwner);
    Check(Initial.bSucceeded, TEXT("ordinary_persistent_VM_initialization"));
    auto* Reload = UDasAsyncPhysicsBridge::Create(ReloadOwner, [](const auto&, FString&) { return true; }, Error);
    FString Original;
    FFileHelper::LoadFileToString(Original, *Path);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Path);
    };
    FFileHelper::SaveStringToFile(Original + TEXT("\nthis is invalid DAS syntax\n"), *Path);
    const auto Rejected = Runtime.RunPersistent(Path, 0, ReloadOwner);
    Check(Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty() && Reload->IsLiveBinding(),
          TEXT("rejected_reload_keeps_existing_binding"));
    FFileHelper::SaveStringToFile(Original + TEXT("\n// accepted native bridge reload\n"), *Path);
    const auto Accepted = Runtime.RunPersistent(Path, 0, ReloadOwner);
    Check(Accepted.bSucceeded && Accepted.Revision > Initial.Revision && !Reload->IsLiveBinding(),
          TEXT("accepted_reload_revokes_old_binding"));
    FFileHelper::SaveStringToFile(Original, *Path);
    auto* Released = UDasAsyncPhysicsBridge::Create(ReloadOwner, [](const auto&, FString&) { return true; }, Error);
    Runtime.ReleaseObject(ReloadOwner);
    Check(!Released->IsLiveBinding(), TEXT("ReleaseObject_revokes_native_binding_without_session_reentry"));
    auto* Ending = Owner(World.Get());
    auto* EndBridge = UDasAsyncPhysicsBridge::Create(Ending, [](const auto&, FString&) { return true; }, Error);
    const auto Ended = Runtime.RunLifecycle(Path, EDasCallback::EndPlay, Ending);
    Check(Ended.bSucceeded && !EndBridge->IsLiveBinding(),
          TEXT("explicit_script_EndPlay_revokes_bridge_without_session"));
    auto* ComponentOwner = NewObject<USceneComponent>(Actor);
    Actor->AddInstanceComponent(ComponentOwner);
    ComponentOwner->RegisterComponent();
    int32 ComponentCount = 0;
    auto* ComponentBridge = UDasAsyncPhysicsBridge::Create(
        ComponentOwner,
        [&](const auto&, FString&)
        {
            ++ComponentCount;
            return true;
        },
        Error);
    Check(ComponentBridge != nullptr, TEXT("component_owner_supported"));
    Step(3);
    const int32 ComponentBefore = ComponentCount;
    ComponentOwner->UnregisterComponent();
    Step(3);
    Check(ComponentCount == ComponentBefore && !ComponentBridge->IsLiveBinding(),
          TEXT("component_owner_unregister_stops_dispatch"));
    UDasAsyncPhysicsBridge::FlushDeferred();
    auto* Dying = Owner(World.Get());
    int32 DeathCount = 0;
    auto* Death = UDasAsyncPhysicsBridge::Create(
        Dying,
        [&](const auto&, FString&)
        {
            ++DeathCount;
            return true;
        },
        Error);
    Step(3);
    const int32 DeathBefore = DeathCount;
    TWeakObjectPtr<UObject> WeakOwner = Dying, WeakBridge = Death;
    Dying->Destroy();
    Dying = nullptr;
    Step(3);
    CollectGarbage(RF_NoFlags);
    Check(DeathCount == DeathBefore && !WeakOwner.IsValid() && !WeakBridge.IsValid(),
          TEXT("Destroy_and_GC_without_retaining_owner"));
    auto* Invalid = UDasAsyncPhysicsBridge::Create(
        GetMutableDefault<ADasCollisionProfileProbe>(), [](const auto&, FString&) { return true; }, Error);
    Check(!Invalid, TEXT("template_owner_rejected"));
    auto* Manual = UDasAsyncPhysicsBridge::Create(Actor, [](const auto&, FString&) { return true; }, Error);
    Manual->AsyncPhysicsTickComponent(1.f / 60.f, 0);
    Check(!Manual->IsLiveBinding(), TEXT("manual_callback_cannot_issue_scope"));
    auto* WorldBound = UDasAsyncPhysicsBridge::Create(Foreign, [](const auto&, FString&) { return true; }, Error);
    GEngine->DestroyWorldContext(Other.Get());
    Other->DestroyWorld(false);
    Other.Reset();
    Check(!WorldBound->IsLiveBinding(), TEXT("world_cleanup_revokes_binding"));
    UDasAsyncPhysicsBridge::RevokeAll();
    Check(GetDasExecutionRole() == EDasExecutionRole::Gameplay, TEXT("final_role_restored"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_EXECUTION_ROLES_RESULT=%s checks=%d externs=%d"),
                Failures ? TEXT("FAIL"): TEXT("PASS"), Checks, Probe->ExternRegistrationCount());
    return Failures ? 1 : 0;
}
