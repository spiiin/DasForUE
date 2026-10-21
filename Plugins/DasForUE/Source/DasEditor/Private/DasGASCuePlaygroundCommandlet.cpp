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
#include "Components/PrimitiveComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "UObject/SavePackage.h"
#include "UObject/UnrealType.h"
#include "Misc/ScopeExit.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "HAL/PlatformProcess.h"

UDasGASCuePlaygroundCommandlet::UDasGASCuePlaygroundCommandlet()
{
    // Niagara needs a real scene/compute dispatcher, even in an offscreen commandlet.
    IsClient = true;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASCuePlaygroundCommandlet::Main(const FString& Params)
{
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FEditorScriptExecutionGuard Guard;
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUE_PLAYGROUND_%s %s"), OK ? TEXT("PASS") : TEXT("FAIL"), Name);
        return OK;
    };
    const FString Folder = TEXT("/Game/DasExamples/GASCues");
    auto* Burst = PrepareDasCueExampleSystem(Folder, TEXT("RadialBurst"), TEXT("NS_Impact"));
    auto* Fountain = PrepareDasCueExampleSystem(Folder, TEXT("FountainLightweight"), TEXT("NS_Aura"));
    auto* Sound = LoadObject<USoundBase>(nullptr, TEXT("/Engine/EngineSounds/1kSineTonePing.1kSineTonePing"));
    if (!Check(Burst && Fountain && Sound && Burst->IsReadyToRun() && Fountain->IsReadyToRun(),
               TEXT("real_sound_and_saved_niagara_assets_ready")))
        return 1;
    FString Error;
    TMap<FString, UBlueprint*> Assets;
    UBlueprint* BP = nullptr;
    if (!FDasGASAssetService::Apply(
            TEXT("gas_cue_playground.das"), Folder, Assets, Error, true) ||
            !FDasDSLService::Apply(TEXT("dsl_gas_cue_playground.das"),
                                        Folder + TEXT("/BP_CuePlayground"),
                                                      TEXT("dsl_gas_cue_playground.CuePlayground"), BP, Error))
    {
        UE_LOG(LogTemp, Error, TEXT("CUE_PLAYGROUND_IMPORT %s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (auto& Pair : Assets)
        Keep.Emplace(Pair.Value);
    Keep.Emplace(BP);
    auto* CDO = BP->GeneratedClass->GetDefaultObject();
    for (const auto& Pair : {TPair<const TCHAR*, const TCHAR*>(
             TEXT("Attributes"), TEXT("PlaygroundStats")),
             TPair<const TCHAR*, const TCHAR*>(TEXT("HitEffect"), TEXT("PlaygroundHit")),
                                               TPair<const TCHAR*, const TCHAR*>(TEXT("AuraEffect"),
                                                                                      TEXT("PlaygroundAura"))})
    {
        auto* Property = FindFProperty<FClassProperty>(CDO->GetClass(), Pair.Key);
        if (!Property)
            return 1;
        Property->SetObjectPropertyValue_InContainer(CDO, Assets.FindChecked(Pair.Value)->GeneratedClass);
    }
    BP->MarkPackageDirty();
    if (!Check(SaveDasCueExampleAsset(BP), TEXT("placeable_blueprint_saved_with_effect_classes")))
        return 1;
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    // UWorld::CreateFXSystem skips every commandlet, including AllowCommandletRendering.
    // Provision this test world's normal FX system explicitly; UWorld owns its normal teardown.
    World->FXSystem = FFXSystemInterface::Create(World->GetFeatureLevel(), World->Scene);
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<AActor>(BP->GeneratedClass, FVector(100, 200, 100), FRotator::ZeroRotator);
    if (!Host)
        return 1;
    ON_SCOPE_EXIT
    {
        Runtime.ReleaseObject(Host);
        if (World->HasBegunPlay())
            World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto* ASC = Host->FindComponentByClass<UDasAuthoredAbilitySystemComponent>();
    auto Call = [&](const TCHAR* Name)
    {
        auto* Function = Host->FindFunction(Name);
        if (!Function)
            return false;
        Host->ProcessEvent(Function, nullptr);
        return true;
    };
    auto Niagara = [&](UNiagaraSystem* Asset)
    {
        TArray<UNiagaraComponent*> Components;
        Host->GetComponents(Components);
        Components.RemoveAll([&](const auto* Component)
                             { return !IsValid(Component) || Component->GetAsset() != Asset; });
        return Components;
    };
    if (!Check(ASC && Call(TEXT("Hit")) &&
                           ASC->ReadAuthoredAttribute(TEXT("PlaygroundStats"), TEXT("Health")) == 99 &&
                                                           Niagara(Burst).Num() == 1,
                                                      TEXT("gameplay_hit_executes_das_sound_and_niagara")))
        return 1;
    TArray<UAudioComponent*> Audio;
    Host->GetComponents(Audio);
    if (!Check(Audio.Num() == 1 && Audio[0]->Sound == Sound &&
                   Audio[0]->GetComponentLocation().Equals(Host->GetActorLocation()),
               TEXT("actual_audio_component_at_target")))
        return 1;
    // A real native effect-context hit must place the burst at ImpactPoint, not at the target origin.
    const FVector Point(123, 234, 150);
    FHitResult Hit(Host, Cast<UPrimitiveComponent>(Host->GetRootComponent()), Point, FVector::UpVector);
    const int32 HitId = ASC->RegisterAuthoredTargetHit(Hit);
    const auto Applied =
        ASC->ApplyAuthoredTargets(ASC->MakeAuthoredSpec(Assets.FindChecked(TEXT("PlaygroundHit"))->GeneratedClass.Get(), 1),
                                                        ASC->MakeAuthoredTargets({}, {HitId}));
    ASC->ReleaseAuthoredTargetHit(HitId);
    if (!Check(Applied.Num() == 1 && Applied[0].Applied && Niagara(Burst).Num() == 1 &&
                   Niagara(Burst)[0]->GetComponentLocation().Equals(Point),
               TEXT("owned_hit_context_positions_and_replaces_impact")))
        return 1;
    const auto AuraTag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Playground.Aura"));
    if (!Check(Call(TEXT("StartAura")) && Call(TEXT("StartAura")) && ASC->IsGameplayCueActive(AuraTag) &&
                                               Niagara(Fountain).Num() == 1,
                                               TEXT("infinite_effect_onactive_whileactive_are_idempotent")))
        return 1;
    TWeakObjectPtr<UNiagaraComponent> AuraComponent = Niagara(Fountain)[0];
    // Activation can wait on native PSO readiness; pump a bounded few frames, not a PIE session.
    for (int32 Frame = 0; Frame < 120 && AuraComponent.IsValid() && AuraComponent->IsComplete(); ++Frame)
    {
        World->Tick(LEVELTICK_All, 1.f / 60.f);
        World->SendAllEndOfFrameUpdates();
        FPlatformProcess::Sleep(0.005f);
    }
    UE_LOG(LogTemp, Display, TEXT("CUE_AURA_STATE client=%d valid=%d registered=%d complete=%d active=%d worldready=%d fx=%d"),
        GIsClient, AuraComponent.IsValid(), AuraComponent.IsValid() && AuraComponent->IsRegistered(),
        !AuraComponent.IsValid() || AuraComponent->IsComplete(), AuraComponent.IsValid() && AuraComponent->IsActive(),
        AuraComponent.IsValid() && AuraComponent->IsWorldReadyToRun(), World->FXSystem != nullptr);
    CollectGarbage(RF_NoFlags);
    UE_LOG(LogTemp, Display, TEXT("CUE_AURA_POST_GC valid=%d registered=%d complete=%d"), AuraComponent.IsValid(),
        AuraComponent.IsValid() && AuraComponent->IsRegistered(), !AuraComponent.IsValid() || AuraComponent->IsComplete());
    if (!Check(AuraComponent.IsValid() && AuraComponent->IsRegistered() && !AuraComponent->IsComplete(),
               TEXT("aura_component_survives_callback_and_gc")))
        return 1;
    if (!Check(Call(TEXT("StopAura")) &&
                    Call(TEXT("StopAura")) && !ASC->IsGameplayCueActive(AuraTag) && Niagara(Fountain).IsEmpty() &&
                         (!AuraComponent.IsValid() || AuraComponent->IsBeingDestroyed()) && Niagara(Burst).Num() == 1,
                         TEXT("removed_destroys_only_aura_and_is_repeat_safe")))
        return 1;
    if (!Call(TEXT("StartAura")) || Niagara(Fountain).Num() != 1)
        return 1;
    TWeakObjectPtr<UNiagaraComponent> Restarted = Niagara(Fountain)[0];
    TWeakObjectPtr<UNiagaraComponent> Impact = Niagara(Burst)[0];
    Host->Destroy();
    if (!Check(Runtime.PersistentContextCount() == 0 && (!Restarted.IsValid() || Restarted->IsBeingDestroyed()) &&
                   (!Impact.IsValid() || Impact->IsBeingDestroyed()),
               TEXT("endplay_cleans_restart_and_impact_sessions")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUE_PLAYGROUND_RESULT=PASS checks=%d"), Checks);
    return 0;
}
