#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasGASCues.h"
#include "DasRuntime.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/PackageName.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
UDasGASCueAssetsCommandlet::UDasGASCueAssetsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASCueAssetsCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FEditorScriptExecutionGuard Guard;
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUE_ASSETS_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
        return OK;
    };
    FString Error;
    FDasGASDeclarations Model;
    if (!R.ReadGASScript(R.ScriptPath(TEXT("gas_cue_assets.das")), Model, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    if (!Check(Model.Cues.Num() == 2 && Model.Effects.Num() == 2, TEXT("dsl_cue_registry_and_repeated_cue_options")))
        return 1;
    TMap<FString, UBlueprint*> Assets;
    const FString Folder = TEXT("/Game/Tests/GASAuthored/CueAssets");
    if (!FDasGASAssetService::Apply(TEXT("gas_cue_assets.das"), Folder, Assets, Error, true))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (auto& Pair : Assets)
        Keep.Emplace(Pair.Value);
    const auto* Cue = Assets.FindChecked(TEXT("ImpactCue"))->GeneratedClass->GetDefaultObject<UDasGameplayCueNotify>();
    const auto* Strike =
        Assets.FindChecked(TEXT("CueStrike"))->GeneratedClass->GetDefaultObject<UDasAuthoredGameplayEffect>();
    const auto* Aura =
        Assets.FindChecked(TEXT("CueAura"))->GeneratedClass->GetDefaultObject<UDasAuthoredGameplayEffect>();
    if (!Check(
            !Cue->DefinitionId.IsEmpty() &&
            Cue->GameplayCueTag.GetTagName() ==
                TEXT("GameplayCue.Das.Sample.Impact") && Strike->CueAssets.Num() == 1 &&
                     Strike->CueAssets[0] == Cue->GetClass() && Strike->GameplayCues.Num() == 1 &&
                     Strike->GameplayCues[0].MinLevel == 1 && Strike->GameplayCues[0].MaxLevel == 5 &&
                     Aura->CueAssets.Num() == 2 && Aura->GameplayCues.Num() == 1 &&
                     Aura->GameplayCues[0].GameplayCueTags.Num() == 2 &&
                     FPackageName::DoesPackageExist(Assets.FindChecked(TEXT("ImpactCue"))->GetOutermost()->GetName()),
                                                    TEXT("saved_native_notify_and_effect_class_references")))
        return 1;
    TMap<FString, UBlueprint*> Again;
    if (!Check(FDasGASAssetService::Apply(TEXT("gas_cue_assets.das"), Folder, Again, Error, true) &&
                                          Again.FindRef(TEXT("ImpactCue")) == Assets.FindRef(TEXT("ImpactCue")),
                                                                                             TEXT("idempotent_import")))
        return 1;
    auto* Set = UAbilitySystemGlobals::Get().GetGameplayCueManager()->GetRuntimeCueSet();
    if (!Set)
        return 1;
    const auto AuraTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Aura"));
    const auto ImpactTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Impact"));
    Set->RemoveCuesByTags(FGameplayTagContainer(AuraTag));
    Set->RemoveCuesByTags(FGameplayTagContainer(ImpactTag));
    const FSoftObjectPath Foreign(UGameplayCueNotify_Static::StaticClass());
    Set->AddCues({FGameplayCueReferencePair(AuraTag, Foreign)});
    const int32 Before = Set->GameplayCueData.Num();
    if (!Check(
            !Aura->PrepareCues() && Set->GameplayCueData.Num() == Before &&
                !Set->GameplayCueData.ContainsByPredicate([&](const auto& D) { return D.GameplayCueTag == ImpactTag; }),
            TEXT("conflict_preflight_does_not_install_partial_routes")))
        return 1;
    Set->RemoveCuesByTags(FGameplayTagContainer(AuraTag));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASAuthoredProbeHost>();
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Host);
        if (World->HasBegunPlay())
            World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto* ASC = Host->Abilities.Get();
    if (!ASC->AddAuthoredAttributes(Assets.FindChecked(TEXT("CueStats"))->GeneratedClass.Get()))
        return 1;
    auto Run = [&](int32 Phase)
    {
        const auto Result = R.RunPersistent(R.ScriptPath(TEXT("gas_cue_assets.das")), Phase, Host);
        if (!Result.bSucceeded || Result.Value != 42)
        {
            UE_LOG(LogTemp, Error, TEXT("CUE_ASSET_SCRIPT %s"), *Result.Error);
            return false;
        }
        return true;
    };
    if (!Run(0))
        return 1;
    auto Instant = ASC->MakeAuthoredSpec(Strike->GetClass(), 3);
    if (!Check(ASC->ApplyAuthoredSpec(Instant).Applied &&
               ASC->ReadAuthoredAttribute(TEXT("CueStats"), TEXT("Health")) == 95 && Run(1),
                                          TEXT("native_effect_executed_and_normalized_snapshot_without_manual_watch")))
        return 1;
    auto Infinite = ASC->MakeAuthoredSpec(Aura->GetClass(), 1);
    const auto Active = ASC->ApplyAuthoredSpec(Infinite);
    if (!Check(Active.Applied && Active.HasActiveHandle && ASC->IsGameplayCueActive(AuraTag) &&
                   ASC->IsGameplayCueActive(ImpactTag) && Run(2),
               TEXT("two_native_active_cues_from_cue_only_effect")))
        return 1;
    CollectGarbage(RF_NoFlags);
    if (!Check(ASC->RemoveAuthoredEffect(Active.ActiveId) && !ASC->IsGameplayCueActive(AuraTag) &&
                   !ASC->IsGameplayCueActive(ImpactTag) && Run(3),
               TEXT("native_effect_removal_and_retained_snapshot")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUE_ASSETS_RESULT=PASS checks=%d"), Checks);
    return 0;
}
