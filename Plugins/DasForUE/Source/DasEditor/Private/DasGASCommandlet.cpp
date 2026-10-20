#include "DasGASCommandlet.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Editor.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"

ADasGASProbeHost::ADasGASProbeHost()
{
    Abilities = CreateDefaultSubobject<UDasHealthAbilitySystemComponent>(TEXT("Abilities"));
}
UDasGASCommandlet::UDasGASCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASCommandlet::Main(const FString& Params)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    if (!World.IsValid())
        return 1;
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASProbeHost>();
    if (!Host)
        return 1;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Host);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    FEditorScriptExecutionGuard ScriptGuard;
    const auto Path = Runtime.ScriptPath(TEXT("gas_health_probe.das"));
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        ++Checks;
        return OK;
    };
    auto Run = [&](int32 Phase)
    {
        const auto Result = Runtime.RunPersistent(Path, Phase, Host);
        if (!Result.bSucceeded || Result.Value != 42)
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_GAS_SCRIPT phase=%d %s"), Phase, *Result.Error);
            return false;
        }
        return true;
    };
    if (!Check(Run(0), TEXT("real_damage_100_to_40_no_vm_reentry")) ||
               !Check(Runtime.SubscriptionCount(Host) == 1, TEXT("managed_attribute_snapshot_subscription")))
        return 1;
    CollectGarbage(RF_NoFlags);
    if (!Check(Run(1), TEXT("snapshot_survives_callback_and_gc_heal_to_65")) ||
               !Check(Run(2), TEXT("instant_effect_success_without_active_handle_cap_100")) ||
                      !Check(Run(3), TEXT("invalid_magnitude_refused_and_snapshot_unchanged")))
        return 1;
    // Releasing the session removes the reflected subscriber while the ASC remains live.
    Runtime.ReleaseObject(Host);
    if (!Check(Runtime.SubscriptionCount(Host) == 0 && !Host->Abilities->AttributeChanged.IsBound(),
               TEXT("released_session_unsubscribed")))
        return 1;
    auto Spec = Host->Abilities->MakeHealthSpec(EDasHealthEffect::Damage, 1);
    Spec.MagnitudeTag = TEXT("Data.Damage");
    Spec.Magnitude = 10;
    Spec.HasMagnitude = true;
    if (!Check(Host->Abilities->ApplyHealthSpec(Spec).Applied &&
                   Host->Abilities->ReadHealthAttribute(EDasHealthAttribute::Health) == 90,
               TEXT("native_gas_continues_after_script_release")))
        return 1;
    if (!Check(Run(4), TEXT("fresh_session_no_stale_callback_or_health_reset")))
        return 1;
    Runtime.ReleaseObject(Host);
    Host->Abilities->SetNumericAttributeBase(UDasHealthAttributeSet::MaxHealthAttribute(), 80);
    if (!Check(Host->Abilities->ReadHealthAttribute(EDasHealthAttribute::Health) == 80 &&
                   Host->Abilities->GetNumericAttributeBase(UDasHealthAttributeSet::HealthAttribute()) == 80,
               TEXT("lower_maximum_clamps_current_and_base_health")))
        return 1;
    for (const TCHAR* Name : {TEXT("dsl_playable_character.das"), TEXT("dsl_playable_pickup.das")})
    {
        FDasAuthoringModule Model;
        FString Error;
        if (!Runtime.ReadAuthoringScript(Runtime.ScriptPath(Name), Model, Error))
        {
            UE_LOG(LogTemp, Error, TEXT("DAS_GAS_EXAMPLE %s: %s"), Name, *Error);
            return 1;
        }
    }
    if (!Check(true, TEXT("updated_character_and_pickup_compile")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_RESULT=PASS checks=%d"), Checks);
    return 0;
}
