#include "DasGASAuthoredCommandlet.h"
#include "DasGASAssetService.h"
#include "DasGASCues.h"
#include "DasRuntime.h"
#include <limits>
#include "Editor.h"
#include "Engine/Blueprint.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Components/SphereComponent.h"
#include "Misc/ScopeExit.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
UDasGASCueHooksCommandlet::UDasGASCueHooksCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASCueHooksCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FEditorScriptExecutionGuard Guard;
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUE_HOOKS_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
        return OK;
    };
    FString Error;
    TMap<FString, UBlueprint*> Assets;
    if (!FDasGASAssetService::Apply(TEXT("gas_cue_hooks.das"), TEXT("/Game/Tests/GASAuthored/CueHooks"), Assets, Error,
                                                                    true))
    {
        UE_LOG(LogTemp, Error, TEXT("%s"), *Error);
        return 1;
    }
    TArray<TStrongObjectPtr<UBlueprint>> Keep;
    for (auto& P : Assets)
        Keep.Emplace(P.Value);
    auto* Cue = Assets.FindChecked(TEXT("HookCue"))->GeneratedClass->GetDefaultObject<UDasGameplayCueNotify>();
    if (!Check(Cue->Hooks.Num() == 4 && Cue->ScriptContract.Len() == 32, TEXT("four_saved_das_hooks")))
        return 1;
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* A = World->SpawnActor<ADasGASAuthoredProbeHost>();
    auto* B = World->SpawnActor<ADasGASAuthoredProbeHost>();
    auto Root = [&](AActor* Actor)
    {
        auto* S = NewObject<USphereComponent>(Actor);
        Actor->SetRootComponent(S);
        Actor->AddInstanceComponent(S);
        S->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        S->RegisterComponent();
        return S;
    };
    auto* SA = Root(A);
    auto* SB = Root(B);
    ON_SCOPE_EXIT
    {
        R.ReleaseObject(A);
        R.ReleaseObject(B);
        if (World->HasBegunPlay())
            World->EndPlay(EEndPlayReason::Quit);
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    World->InitializeActorsForPlay(FURL());
    World->BeginPlay();
    World->GetWorldSettings()->NotifyBeginPlay();
    auto* AS = A->Abilities.Get();
    auto* BS = B->Abilities.Get();
    const auto Stats = Assets.FindChecked(TEXT("CueHookStats"))->GeneratedClass.Get();
    if (!AS->AddAuthoredAttributes(Stats) || !BS->AddAuthoredAttributes(Stats))
        return 1;
    auto Effect = AS->MakeAuthoredSpec(Assets.FindChecked(TEXT("HookStrike"))->GeneratedClass.Get(), 1);
    if (!Effect.Definition)
        return 1;
    const auto Tag = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Hooks"));
    auto Run = [&](int32 Phase)
    {
        const auto V = R.RunPersistent(R.ScriptPath(TEXT("gas_cue_hooks.das")), Phase, A);
        if (!V.bSucceeded)
            UE_LOG(LogTemp, Error, TEXT("CUE_HOOK_OBSERVER %s"), *V.Error);
        return V.bSucceeded && V.Value == 42;
    };
    if (!Run(0))
        return 1;
    FDasGASCueParameters P;
    P.RawMagnitude = 7;
    P.SourceObject = AS;
    P.SourceTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Data.Healing")));
    P.TargetTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample")));
    P.Context.Valid = true;
    P.Context.Instigator = A;
    P.Context.OriginalInstigator = A;
    P.Context.EffectCauser = A;
    P.Context.SourceObject = A;
    P.Context.Actors = {A, B};
    P.Context.HasOrigin = true;
    P.Context.Origin = FVector(5, 6, 7);
    P.Context.HasHit = true;
    P.Context.Hit = FHitResult(A, SA, FVector(11, 22, 33), FVector::UpVector);
    P.Context.Hit.TraceStart = FVector(1, 2, 3);
    P.Context.Hit.TraceEnd = FVector(11, 22, 33);
    if (!Check(AS->ExecuteAuthoredCue(Tag.GetTagName(), P) && A->GetActorScale3D().X == 2,
               TEXT("execute_and_deferred_nested_hook")))
        return 1;
    P.SourceObject = BS;
    if (!Check(BS->ExecuteAuthoredCue(Tag.GetTagName(), P) && B->GetActorScale3D().X == 2,
               TEXT("state_isolated_for_second_target")))
        return 1;
    P.Context.Hit.ImpactPoint = FVector(999);
    P.Context.Origin = FVector(999);
    P.SourceTags.Reset();
    P.TargetTags.Reset();
    CollectGarbage(RF_NoFlags);
    if (!Check(Run(1), TEXT("owned_extended_observer_snapshot_after_gc")))
        return 1;
    // Native GE target-data path contributes its hit and captured tags, not a manually synthesized callback.
    AS->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Data.Healing")));
    BS->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample")));
    const FHitResult Hit(B, SB, FVector(11, 22, 33), FVector::UpVector);
    const auto HitId = AS->RegisterAuthoredTargetHit(Hit);
    const auto Targets = AS->MakeAuthoredTargets({}, {HitId});
    const auto Applied = AS->ApplyAuthoredTargets(Effect, Targets);
    if (!Check(Applied.Num() == 1 && Applied[0].Applied && B->GetActorScale3D().X == 3,
               TEXT("real_effect_context_hit_and_captured_tags")))
        return 1;
    FDasGASCueParameters Active;
    Active.RawMagnitude = 9;
    if (!Check(AS->AddAuthoredCue(Tag.GetTagName(), Active) && BS->AddAuthoredCue(Tag.GetTagName(), Active) &&
                   A->GetActorScale3D().Y == 1 && B->GetActorScale3D().Y == 1,
               TEXT("onactive_and_whileactive_hooks")))
        return 1;
    CollectGarbage(RF_NoFlags);
    if (!Check(AS->RemoveAuthoredCue(Tag.GetTagName()) && BS->RemoveAuthoredCue(Tag.GetTagName()) &&
                   A->GetActorScale3D().Z == 1 && B->GetActorScale3D().Z == 1,
               TEXT("removed_hooks_and_retained_instance_snapshot")))
        return 1;
    FDasGASCueParameters Invalid;
    Invalid.Context.HasOrigin = true;
    Invalid.Context.Origin.X = std::numeric_limits<double>::quiet_NaN();
    if (!Check(!AS->ExecuteAuthoredCue(Tag.GetTagName(), Invalid), TEXT("nonfinite_context_refused")))
        return 1;
    R.ReleaseObject(A);
    R.ReleaseObject(B);
    const int32 Before = R.PersistentContextCount();
    A->Destroy();
    B->Destroy();
    if (!Check(Before == 2 && R.PersistentContextCount() == 0, TEXT("endplay_releases_both_cue_sessions")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUE_HOOKS_RESULT=PASS checks=%d"), Checks);
    return 0;
}
