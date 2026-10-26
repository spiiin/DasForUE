#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasGASCalculations.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Modules/ModuleManager.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "HAL/FileManager.h"
UDasGASAdvancedCommandlet::UDasGASAdvancedCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASAdvancedCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ADVANCED_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
        return OK;
    };
    FString Error;
    TMap<FString, UBlueprint*> Assets;
    const auto Folder = TEXT("/Game/Tests/GASAuthored/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
    if (!Check(FDasGASAssetService::Apply(TEXT("gas_advanced_abilities.das"), Folder, Assets, Error, true),
                                          TEXT("import_calculations_and_example")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    FString Fixture, Source;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Source, *Fixture))
        return 2;
    Source.ReplaceInline(TEXT("@ASSET_FOLDER@"), *Folder);
    const auto File = TEXT("gas_advanced_probe_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das");
    const auto Path = R.ScriptPath(File);
    if (!FFileHelper::SaveStringToFile(Source, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Path, false, true);
    };
    TMap<FString, UBlueprint*> Probe;
    if (!Check(FDasGASAssetService::Apply(File, Folder, Probe, Error, true), TEXT("compile_task_probe")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (const auto& P : Assets)
        Keep.Emplace(P.Value);
    for (const auto& P : Probe)
        Keep.Emplace(P.Value);
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASAdvancedProbeHost>();
    auto* Target = World->SpawnActor<ADasGASAdvancedProbeHost>();
    if (!Host || !Target)
        return 1;
    Host->Other = Target;
    Host->ProbeHit = FHitResult(Target, nullptr, Target->GetActorLocation(), FVector::UpVector);
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Host);
        R.ReleaseObject(Target);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* A = Host->Abilities.Get();
    auto* B = Target->Abilities.Get();
    auto* Stats = Assets.FindChecked(TEXT("BattleStats"))->GeneratedClass.Get();
    if (!A->AddAuthoredAttributes(Stats) || !B->AddAuthoredAttributes(Stats))
        return 1;
    const auto* Set = CastChecked<UDasAuthoredAttributeSet>(Stats->GetDefaultObject());
    auto Health = Set->Attribute(TEXT("Health"));
    auto Power = Set->Attribute(TEXT("Power"));
    auto Armor = Set->Attribute(TEXT("Armor"));
    FEditorScriptExecutionGuard Guard;
    auto Advance = [&](int32 Frames)
    {
        for (int32 I = 0; I < Frames; ++I)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, 0.01f);
        }
    };
    // DAS caller -> native target data -> native MMC -> another DAS calculation VM.
    const auto Call = R.RunPersistent(Path, 0, Host);
    if (!Check(Call.bSucceeded && Call.Value == 42 && FMath::IsNearlyEqual(B->GetNumericAttribute(Health), 82.f),
               TEXT("typed_targets_deduplicate_and_magnitude_capture")))
    {
        UE_LOG(LogTemp, Warning, TEXT("%s"), *Call.Error);
        return 1;
    }
    // Native spec creation is the snapshot boundary. Change source afterwards;
    // target non-snapshot capture is read when the effect is applied.
    auto* Effect = Assets.FindChecked(TEXT("ScaledHit"))->GeneratedClass.Get();
    auto Native = A->MakeOutgoingSpec(Effect, 1.f, A->MakeEffectContext());
    A->SetNumericAttributeBase(Power, 30.f);
    B->SetNumericAttributeBase(Armor, 5.f);
    const auto Applied = B->ApplyGameplayEffectSpecToSelf(*Native.Data.Get());
    if (!Check(Applied.WasSuccessfullyApplied() && FMath::IsNearlyEqual(B->GetNumericAttribute(Health), 75.f),
               TEXT("native_source_snapshot_and_live_target_capture")))
        return 1;
    auto Exec = A->MakeAuthoredSpec(Assets.FindChecked(TEXT("ExecutedHit"))->GeneratedClass.Get(), 1.f);
    const auto Executed = B->ApplyAuthoredSpec(Exec);
    if (!Check(Executed.Applied && FMath::IsNearlyEqual(B->GetNumericAttribute(Health), 55.f),
               TEXT("native_execution_output_modifier")))
        return 1;
    // Owned hit snapshot, native single-target hit conversion and weak actor validation.
    FHitResult Hit(Target, nullptr, Target->GetActorLocation(), FVector::UpVector);
    auto Targets = A->MakeAuthoredTargets({Target}, {A->RegisterAuthoredTargetHit(Hit)});
    const auto Results = A->ApplyAuthoredTargets(Exec, Targets);
    if (!Check(Results.Num() == 1 && Results[0].Applied && Results[0].Target == B &&
                   FMath::IsNearlyEqual(B->GetNumericAttribute(Health), 35.f),
               TEXT("hit_target_context_and_actor_deduplication")))
        return 1;
    const auto Grant = A->GiveAuthoredAbility(Probe.FindChecked(TEXT("TaskProbe"))->GeneratedClass.Get(), 1);
    if (!Check(Grant.Id > 0 && A->ActivateAuthoredAbility(Grant.Id), TEXT("start_real_ability_tasks")))
        return 1;
    UDasAuthoredGameplayAbility* Instance = nullptr;
    for (const auto& S : A->GetActivatableAbilities())
        if (A->AuthoredAbilityId(S.Handle) == Grant.Id)
            Instance = Cast<UDasAuthoredGameplayAbility>(S.GetPrimaryInstance());
    TStrongObjectPtr<UDasAuthoredGameplayAbility> Pin(Instance);
    if (!Check(Instance && Instance->ScriptTaskCount() == 2 &&
                   Instance->PlayScriptMontage(Instance->ScriptAbilityContext().ActivationId, nullptr) == 0,
               TEXT("live_task_count_and_invalid_montage_refused")))
        return 1;
    for (int32 I = 0; I < 2; ++I)
        if (!A->SendAuthoredGameplayEvent(TEXT("Data.Healing"), 7.f, Host, Target))
            return 1;
    Advance(12);
    if (!Check(!Instance->IsActive() && Instance->ScriptTaskCount() == 0 &&
                   A->AuthoredAbilityState(Grant.Id).Error.IsEmpty(),
               TEXT("delay_event_snapshots_and_normal_end_cleanup")))
        return 1;
    if (!A->ActivateAuthoredAbility(Grant.Id) || !A->CancelAuthoredAbility(Grant.Id))
        return 1;
    Advance(12);
    if (!Check(Instance->ScriptTaskCount() == 0 && A->AuthoredAbilityState(Grant.Id).Error.IsEmpty(),
               TEXT("cancel_discards_pending_callbacks")))
        return 1;
    if (!A->ActivateAuthoredAbility(Grant.Id) || !A->RemoveAuthoredAbility(Grant.Id))
        return 1;
    Advance(12);
    if (!Check(Instance->ScriptTaskCount() == 0 && !Instance->IsActive(), TEXT("revoke_discards_tasks")))
        return 1;
    Target->Destroy();
    if (!Check(A->ApplyAuthoredTargets(Exec, Targets).IsEmpty(), TEXT("stale_target_snapshot_refused")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ADVANCED_RESULT=PASS checks=%d"), Checks);
    return 0;
}
