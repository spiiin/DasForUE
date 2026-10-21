#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasGASCueExampleAssets.h"
#include "DasDSLService.h"
#include "DasGASCues.h"
#include "DasRuntime.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "FXSystem.h"
#include "GameFramework/WorldSettings.h"
#include "Components/AudioComponent.h"
#include "NiagaraComponent.h"
#include "Sound/SoundBase.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/UnrealType.h"
#include "Misc/ScopeExit.h"
#include "Containers/Ticker.h"

UDasGASAbilityPlaygroundCommandlet::UDasGASAbilityPlaygroundCommandlet()
{
    IsClient = true;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASAbilityPlaygroundCommandlet::Main(const FString& Params)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FEditorScriptExecutionGuard Guard;
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PLAYGROUND_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        return OK;
    };
    const FString Folder = TEXT("/Game/DasExamples/GASCues");
    auto* Burst = PrepareDasCueExampleSystem(Folder, TEXT("RadialBurst"), TEXT("NS_Impact"));
    auto* Fountain = PrepareDasCueExampleSystem(Folder, TEXT("FountainLightweight"), TEXT("NS_Aura"));
    if (!Burst || !Fountain || !Burst->IsReadyToRun() || !Fountain->IsReadyToRun())
        return 1;
    FString Error;
    TMap<FString, UBlueprint*> Cues, Abilities;
    UBlueprint* CasterBP = nullptr;
    UBlueprint* TargetBP = nullptr;
    // Register actor schemas before compiling the ability's declaration-only typed reference.
    if (!FDasGASAssetService::Apply(TEXT("gas_cue_playground.das"), Folder, Cues, Error, true) ||
        !FDasDSLService::Apply(TEXT("dsl_gas_ability_playground.das"), Folder + TEXT("/BP_AbilityPlayground"), TEXT("dsl_gas_ability_playground.AbilityPlayground"), CasterBP, Error) ||
        !FDasDSLService::Apply(TEXT("dsl_gas_ability_target.das"), Folder + TEXT("/BP_AbilityTarget"), TEXT("dsl_gas_ability_target.AbilityTarget"), TargetBP, Error) ||
        !FDasGASAssetService::Apply(TEXT("gas_targeted_strike.das"), Folder, Abilities, Error, true))
    {
        UE_LOG(LogTemp, Error, TEXT("GAS_ABILITY_PLAYGROUND_IMPORT %s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (auto& P : Cues)
        Keep.Emplace(P.Value);
    for (auto& P : Abilities)
        Keep.Emplace(P.Value);
    Keep.Emplace(CasterBP);
    Keep.Emplace(TargetBP);
    auto Assign = [](UBlueprint* BP, const TCHAR* Field, UClass* Value)
    {
        auto* Object = BP->GeneratedClass->GetDefaultObject();
        auto* P = FindFProperty<FClassProperty>(Object->GetClass(), Field);
        if (!P)
            return false;
        P->SetObjectPropertyValue_InContainer(Object, Value);
        BP->MarkPackageDirty();
        return true;
    };
    if (!Check(Assign(
            CasterBP,
            TEXT("Attributes"), Cues.FindChecked(TEXT("PlaygroundStats"))->GeneratedClass) &&
                 Assign(CasterBP,
                        TEXT("HitEffect"), Cues.FindChecked(TEXT("PlaygroundHit"))->GeneratedClass) &&
                             Assign(
                                 CasterBP,
                                 TEXT("AuraEffect"), Cues.FindChecked(TEXT("PlaygroundTimedAura"))->GeneratedClass) &&
                                      Assign(CasterBP,
                                             TEXT("StrikeAbility"),
                                                  Abilities.FindChecked(TEXT("TargetedStrike"))->GeneratedClass) &&
                                                  Assign(TargetBP, TEXT("Attributes"),
                                                                        Cues.FindChecked(
                                                                            TEXT("PlaygroundStats"))->GeneratedClass) &&
                                                                        SaveDasCueExampleAsset(CasterBP) &&
                                                                        SaveDasCueExampleAsset(TargetBP),
                                                         TEXT("playable_caster_target_and_native_assets_saved")))
        return 1;
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    // Stock UWorld intentionally skips FX creation in commandlets; this isolated offscreen world needs it.
    World->FXSystem = FFXSystemInterface::Create(World->GetFeatureLevel(), World->Scene);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Caster = World->SpawnActor<AActor>(CasterBP->GeneratedClass, FVector::ZeroVector, FRotator::ZeroRotator);
    auto* Target = World->SpawnActor<AActor>(TargetBP->GeneratedClass, FVector(250, 0, 0), FRotator::ZeroRotator);
    if (!Caster || !Target)
        return 1;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Caster);
        Runtime.ReleaseObject(Target);
        if (World->HasBegunPlay())
            World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto* Source = Caster->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
    auto* Victim = Target->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
    auto* GrantProperty = FindFProperty<FStructProperty>(Caster->GetClass(), TEXT("Grant"));
    if (!Source || !Victim || !GrantProperty)
        return 1;
    const auto Grant = *GrantProperty->ContainerPtrToValuePtr<FDasAuthoredAbilityGrant>(Caster);
    auto State = [&]
    {
        return Source->AuthoredAbilityState(Grant.Id);
    };
    auto Call = [&](const TCHAR* Name)
    {
        auto* F = Caster->FindFunction(Name);
        if (!F)
            return false;
        Caster->ProcessEvent(F, nullptr);
        return true;
    };
    auto ActorField = [&](const TCHAR* Name)
    {
        return Cast<AActor>(
            FindFProperty<FObjectPropertyBase>(Caster->GetClass(), Name)->GetObjectPropertyValue_InContainer(Caster));
    };
    auto IntField = [&](const TCHAR* Name)
    {
        return FindFProperty<FIntProperty>(Caster->GetClass(), Name)->GetPropertyValue_InContainer(Caster);
    };
    auto Niagara = [&](UNiagaraSystem* Asset)
    {
        TArray<UNiagaraComponent*> Components;
        Target->GetComponents(Components);
        Components.RemoveAll([&](const auto* C) { return !IsValid(C) || C->GetAsset() != Asset; });
        return Components;
    };
    auto Health = [&]
    {
        return Victim->ReadAuthoredAttribute(TEXT("PlaygroundStats"), TEXT("Health"));
    };
    auto* Spec = Source->FindAbilitySpecFromClass(Abilities.FindChecked(TEXT("TargetedStrike"))->GeneratedClass.Get());
    auto* Instance = Spec ? Cast<UDasAuthoredGameplayAbility>(Spec->GetPrimaryInstance()) : nullptr;
    // The primary instance can be created lazily by the first activation.
    if (!Call(TEXT("BeginAim")))
        return 1;
    Spec = Source->FindAbilitySpecFromClass(Abilities.FindChecked(TEXT("TargetedStrike"))->GeneratedClass.Get());
    Instance = Spec ? Cast<UDasAuthoredGameplayAbility>(Spec->GetPrimaryInstance()) : nullptr;
    if (!Check(Instance && State().Active && !State().Committed && State().Error.IsEmpty() &&
               ActorField(TEXT("PreviewTarget")) == Target && Health() == 100 && Niagara(Burst).IsEmpty() &&
                          Niagara(Fountain).IsEmpty(),
                          TEXT("native_trace_preview_waits_for_confirmation")))
        return 1;
    if (!Check(Call(TEXT("CancelAim")) && !State().Active && !State().Committed && State().Error.IsEmpty() &&
                    Instance->ScriptTaskCount() == 0 &&
                    !ActorField(TEXT("PreviewTarget")) &&
                                IntField(TEXT("CancelledShots")) == 1 && Health() == 100,
                                         TEXT("cancel_discards_selection_without_damage_or_cues")))
        return 1;
    if (!Check(Call(
            TEXT("BeginAim")) &&
            Call(TEXT("ConfirmAim")) && !State().Active && State().Committed &&
                 State().Error.IsEmpty() && Instance->ScriptTaskCount() == 0 && Health() == 99 &&
                 Source->ReadAuthoredAttribute(
                     TEXT("PlaygroundStats"), TEXT("Health")) == 100 &&
                          IntField(TEXT("Shots")) == 1 &&
                                   ActorField(
                                       TEXT("LastTarget")) == Target &&
                                       !ActorField(TEXT("PreviewTarget")),
                                                   TEXT("confirmed_das_ability_commits_applies_both_effects_and_ends")))
    {
        UE_LOG(LogTemp, Error, TEXT("ABILITY_STATE %s"), *State().Error);
        return 1;
    }
    TArray<UAudioComponent*> Audio;
    Target->GetComponents(Audio);
    if (!Check(Audio.Num() == 1 && Audio[0]->Sound && Niagara(Burst).Num() == 1 &&
                   FMath::IsNearlyEqual(Niagara(Burst)[0]->GetComponentLocation().X, 225.0, 1.0) &&
                   Audio[0]->GetComponentLocation().Equals(Niagara(Burst)[0]->GetComponentLocation()),
               TEXT("real_trace_hit_positions_native_sound_and_niagara")))
        return 1;
    const auto AuraTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Playground.Aura"));
    TWeakObjectPtr<UNiagaraComponent> Aura = Niagara(Fountain).Num() == 1 ? Niagara(Fountain)[0] : nullptr;
    CollectGarbage(RF_NoFlags);
    if (!Check(!State().Active && Victim->IsGameplayCueActive(AuraTag) && Aura.IsValid() && Aura->IsRegistered() &&
                   !Aura->IsComplete(),
               TEXT("duration_aura_outlives_ability_and_survives_gc")))
        return 1;
    for (int32 I = 0; I < 230; ++I)
    {
        ++GFrameCounter;
        World->Tick(LEVELTICK_All, 0.01f);
        World->SendAllEndOfFrameUpdates();
        FTSTicker::GetCoreTicker().Tick(0.01f);
    }
    if (!Check(!Victim->IsGameplayCueActive(AuraTag) && Niagara(Fountain).IsEmpty() &&
                   (!Aura.IsValid() || Aura->IsBeingDestroyed()) && Health() == 99,
               TEXT("native_duration_expiry_calls_removed_and_destroys_aura")))
        return 1;
    if (!Call(TEXT("BeginAim")) || ActorField(TEXT("PreviewTarget")) != Target)
        return 1;
    Caster->SetActorRotation(FRotator(0, 90, 0));
    if (!Check(Call(TEXT("ConfirmAim")) && !State().Active && !State().Committed && State().Error.IsEmpty() &&
                    Instance->ScriptTaskCount() == 0 &&
                    IntField(TEXT("Shots")) == 1 &&
                             IntField(TEXT("CancelledShots")) == 2 && Health() == 99 &&
                                      !Victim->IsGameplayCueActive(AuraTag),
                                      TEXT("confirmation_requeries_current_aim_and_miss_is_harmless")))
        return 1;
    Caster->Destroy();
    Target->Destroy();
    if (!Check(Runtime.PersistentContextCount() == 0, TEXT("endplay_releases_ability_actor_and_cue_contexts")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_ABILITY_PLAYGROUND_RESULT=PASS checks=%d"), Checks);
    return 0;
}
