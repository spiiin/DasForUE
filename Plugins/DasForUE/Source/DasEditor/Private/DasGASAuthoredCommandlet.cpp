#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasRuntime.h"
#include "DasAuthoring.h"
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/Package.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
ADasGASAuthoredProbeHost::ADasGASAuthoredProbeHost()
{
    Abilities = CreateDefaultSubobject<UDasAuthoredAbilitySystemComponent>(TEXT("Abilities"));
}
UDasGASAuthoredCommandlet::UDasGASAuthoredCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASAuthoredCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_AUTHORED_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
        return OK;
    };
    FDasGASDeclarations Model;
    FString Error;
    if (!Check(R.ReadGASScript(R.ScriptPath(TEXT("gas_combat_definitions.das")), Model, Error) &&
                               Model.Sets.Num() == 1 && Model.Effects.Num() == 10 && Model.Abilities.Num() == 2,
                               TEXT("declarations_compile")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    const auto Expected = Model;
    if (!Check(!R.ReadGASScript(R.ScriptPath(TEXT("gas_authored_invalid.das")), Model, Error),
                                TEXT("invalid_linked_limit_refused")))
        return 1;
    if (!Check(!R.ReadGASScript(R.ScriptPath(TEXT("gas_duration_invalid.das")), Model, Error),
                                TEXT("negative_duration_refused_before_asset_creation")))
        return 1;
    // Exercise policy validation without creating assets or starting another scenario.
    const FString Prefix = TEXT("{\"declarations\":[{\"kind\":\"attributes\",\"name\":\"Values\",\"id\":\"test.Values\",\"fields\":[{\"name\":\"Value\",\"value\":0}]},{\"kind\":\"effect\",\"name\":\"Effect\",\"id\":\"test.Effect\",\"attributes\":\"Values\",\"fields\":[{\"name\":\"Value\",\"value\":1}],");
    for (
        const auto* Options : {TEXT("\"duration\":\"Instant\",\"period\":0.1"),
            TEXT("\"duration\":\"Infinite\",\"stacking\":\"AggregateByTarget\",\"stack_limit\":0"),
                TEXT("\"duration\":\"Infinite\",\"stacking\":\"AggregateByTarget\",\"stack_limit\":2,\"stack_duration\":\"NeverRefresh\""),
                    TEXT("\"duration\":\"HasDuration\",\"seconds\":0.5,\"stacking\":\"AggregateByTarget\",\"stack_limit\":2,\"stack_expiration\":\"Unknown\"")})
        if (!ParseDasGASDeclarations(Prefix + Options + TEXT("}]}"), Model, Error))
            continue;
        else
            return 1;
    if (!Check(true, TEXT("invalid_duration_period_and_stack_policies_refused")))
        return 1;
    const bool MissingEffectRejected = !R.ReadGASScript(R.ScriptPath(TEXT("gas_ability_invalid.das")), Model, Error);
    const FString PositiveCost =
        Prefix +
        TEXT("\"duration\":\"Instant\"},{\"kind\":\"ability\",\"name\":\"Ability\",\"id\":\"test.Ability\",\"effect\":\"Effect\",\"cost\":\"Effect\",\"fields\":[]}]}");
    if (!Check(MissingEffectRejected && !ParseDasGASDeclarations(PositiveCost, Model, Error),
               TEXT("ability_unknown_effect_and_positive_cost_refused")))
        return 1;
    FString Folder;
    const bool Reopen = FParse::Value(*Params, TEXT("Reopen="), Folder);
    TMap<FString, UBlueprint*> Assets;
    if (Reopen)
    {
        if (!Folder.StartsWith(TEXT("/Game/Tests/GASAuthored/")))
            return 2;
        TArray<TPair<FString, FString>> ExpectedAssets;
        for (const auto& Set : Expected.Sets)
            ExpectedAssets.Emplace(Set.Name, TEXT("AS_"));
        for (const auto& Effect : Expected.Effects)
            ExpectedAssets.Emplace(Effect.Name, TEXT("GE_"));
        for (const auto& Ability : Expected.Abilities)
            ExpectedAssets.Emplace(Ability.Name, TEXT("GA_"));
        for (const auto& Pair : ExpectedAssets)
        {
            const FString AssetName = Pair.Value + Pair.Key;
            auto* BP = LoadObject<UBlueprint>(nullptr, *(Folder + TEXT("/") + AssetName + TEXT(".") + AssetName));
            if (!BP)
            {
                UE_LOG(LogTemp, Error, TEXT("Cannot reopen GAS Blueprint %s"), *AssetName);
                return 1;
            }
            Assets.Add(Pair.Key, BP);
        }
    }
    else
    {
        Folder = TEXT("/Game/Tests/GASAuthored/")+ FGuid::NewGuid().ToString(EGuidFormats::Digits);
        if (!FDasGASAssetService::Apply(TEXT("gas_combat_definitions.das"), Folder, Assets, Error, true))
        {
            UE_LOG(LogTemp, Error, TEXT("GAS asset import: %s"), *Error);
            return 1;
        }
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (const auto& P : Assets)
        Keep.Emplace(P.Value);
    if (!Check(Assets.Num() == 13, TEXT("real_attribute_and_effect_blueprints_saved")))
        return 1;
    auto* AttrClass = Assets.FindChecked(TEXT("CombatAttributes"))->GeneratedClass.Get();
    auto* P = FindFProperty<FStructProperty>(AttrClass, TEXT("Health"));
    auto* Heal = CastChecked<UDasAuthoredGameplayEffect>(
        Assets.FindChecked(TEXT("PotionHeal"))->GeneratedClass->GetDefaultObject());
    if (!Check(P && P->Struct == FGameplayAttributeData::StaticStruct() && Heal->Modifiers.Num() == 1 &&
                   Heal->Modifiers[0].Attribute.GetUProperty() == P,
               TEXT("real_gameplay_attribute_data_and_property_reference")))
        return 1;
    TMap<FString, UBlueprint*> Again;
    if (!Check(FDasGASAssetService::Apply(
            TEXT("gas_combat_definitions.das"), Folder, Again, Error, false) &&
            Again.FindChecked(TEXT("CombatAttributes")) ==
                              Assets.FindChecked(TEXT("CombatAttributes")),
                                                 TEXT("identical_reimport_preserves_classes")))
        return 1;
    FString Fixture, Source;
    if (!FParse::Value(*Params, TEXT("Fixture="), Fixture) || !FFileHelper::LoadFileToString(Source, *Fixture))
        return 2;
    Source.ReplaceInline(TEXT("@ASSET_FOLDER@"), *Folder);
    const FString Probe =
        R.ScriptPath(TEXT("gas_authored_runtime_")+ FGuid::NewGuid().ToString(EGuidFormats::Digits) + TEXT(".das"));
    if (!FFileHelper::SaveStringToFile(Source, *Probe, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
        return 2;
    ON_SCOPE_EXIT
    {
        IFileManager::Get().Delete(*Probe, false, true);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASAuthoredProbeHost>();
    if (!Host)
        return 1;
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(Host);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    const bool Added = Host->Abilities->AddAuthoredAttributes(AttrClass);
    const auto* Defaults = CastChecked<UDasAuthoredAttributeSet>(AttrClass->GetDefaultObject());
    UE_LOG(LogTemp, Display,
           TEXT("GAS_REGISTER added=%d cdo=%s rules=%d owner=%s"), Added, *Defaults->DefinitionName.ToString(),
                Defaults->Rules.Num(), *Host->GetName());
    for (const auto* Spawned : Host->Abilities->GetSpawnedAttributes())
        if (const auto* Set = Cast<UDasAuthoredAttributeSet>(Spawned))
            UE_LOG(LogTemp, Display,
                   TEXT("GAS_SET class=%s name=%s rules=%d health=%d"), *Set->GetClass()->GetName(),
                        *Set->DefinitionName.ToString(), Set->Rules.Num(), Set->Attribute(TEXT("Health")).IsValid());
    if (!Check(Added && Host->Abilities->HasAuthoredAttribute(TEXT("CombatAttributes"), TEXT("Health")),
                                                              TEXT("native_attribute_registration_and_lookup")))
        return 1;
    FEditorScriptExecutionGuard Guard;
    auto Advance = [&](float Seconds)
    {
        // A real world/TimerManager tick, not direct execution of an effect.
        const int32 Frames = FMath::RoundToInt(Seconds / 0.01f);
        for (int32 Frame = 0; Frame < Frames; ++Frame)
        {
            ++GFrameCounter;
            World->Tick(LEVELTICK_All, 0.01f);
        }
    };
    for (int32 Phase = 0; Phase < 16; ++Phase)
    {
        if (Phase == 1 || Phase == 5 || Phase == 12)
            CollectGarbage(RF_NoFlags);
        if (Phase == 5)
            Advance(0.2f);
        if (Phase == 6)
            Advance(0.12f);
        if (Phase == 7)
            Advance(0.3f);
        if (Phase == 8 || Phase == 9)
            Advance(0.31f);
        if (Phase == 11 || Phase == 15)
            Advance(0.45f);
        const auto Result = R.RunPersistent(Probe, Phase, Host);
        if (!Result.bSucceeded || Result.Value != 42)
        {
            UE_LOG(LogTemp, Error, TEXT("GAS phase=%d %s"), Phase, *Result.Error);
            return 1;
        }
        if ((Phase == 10 || Phase == 15) &&
            !Check(Host->Abilities->HasMatchingGameplayTag(
                FGameplayTag::RequestGameplayTag(TEXT("Ability.Cooldown.Restore"))) == (Phase == 10),
                Phase == 10 ? TEXT("native_cooldown_tag_granted") : TEXT("native_cooldown_tag_expired")))
            return 1;
    }
    if (!Check(true, TEXT("native_effects_abilities_cost_cooldown_lifecycle_snapshots")))
        return 1;
    R.ReleaseObject(Host);
    if (!Check(R.SubscriptionCount(Host) == 0 && !Host->Abilities->AttributeChanged.IsBound() &&
                   !Host->Abilities->AbilityChanged.IsBound(),
               TEXT("released_owner_unsubscribed")))
        return 1;
    FDasAuthoringModule ActorModel;
    if (!Check(R.ReadAuthoringScript(R.ScriptPath(TEXT("dsl_authored_gas_actor.das")), ActorModel, Error),
                                     TEXT("gameplay_actor_class_and_component_compile")))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_AUTHORED_RESULT=PASS checks=%d assets=%s"), Checks, *Folder);
    return 0;
}
