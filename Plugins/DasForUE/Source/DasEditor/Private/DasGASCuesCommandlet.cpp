#include "DasGASAuthoredCommandlet.h"
#include "DasGASCues.h"
#include "DasRuntime.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayCueSet.h"
#include "Editor.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
UDasGASCuesCommandlet::UDasGASCuesCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasGASCuesCommandlet::Main(const FString& Params)
{
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    auto* Host = World->SpawnActor<ADasGASAuthoredProbeHost>();
    TStrongObjectPtr<ADasGASAuthoredProbeHost> Keep(Host);
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
    FEditorScriptExecutionGuard Guard;
    auto* ASC = Host->Abilities.Get();
    const auto Impact = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Impact"));
    const auto Aura = FGameplayTag::RequestGameplayTag(TEXT("GameplayCue.Das.Sample.Aura"));
    int32 Checks = 0;
    auto Check = [&](bool OK, const TCHAR* Name)
    {
        ++Checks;
        UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUES_%s %s"), OK ? TEXT("PASS"): TEXT("FAIL"), Name);
        return OK;
    };
    auto Run = [&](int32 Phase)
    {
        const auto Result = R.RunPersistent(R.ScriptPath(TEXT("gas_cues.das")), Phase, Host);
        if (!Result.bSucceeded || Result.Value != 42)
        {
            UE_LOG(LogTemp, Error, TEXT("GAS_CUE_SCRIPT phase=%d %s"), Phase, *Result.Error);
            return false;
        }
        return true;
    };
    // An existing native notify route must not be replaced by the DAS bridge.
    auto* Set = UAbilitySystemGlobals::Get().GetGameplayCueManager()->GetRuntimeCueSet();
    if (!Set)
        return 1;
    const FSoftObjectPath NativePath(UGameplayCueNotify_Static::StaticClass());
    Set->AddCues({FGameplayCueReferencePair(Aura, NativePath)});
    const auto* Entry = Set->GameplayCueData.FindByPredicate([&](const auto& D) { return D.GameplayCueTag == Aura; });
    if (!Check(!ASC->WatchAuthoredCue(Aura.GetTagName()) && Entry && Entry->GameplayCueNotifyObj == NativePath,
               TEXT("existing_native_notify_route_preserved")))
        return 1;
    Set->RemoveCuesByTags(FGameplayTagContainer(Aura));
    if (!Check(!ASC->WatchAuthoredCue(TEXT("Data.Healing")), TEXT("non_cue_tag_refused")))
        return 1;
    if (!Check(Run(0) && R.SubscriptionCount(Host) == 1, TEXT("native_cues_and_queued_das_subscription")))
        return 1;
    CollectGarbage(RF_NoFlags);
    if (!Check(Run(1), TEXT("owned_snapshot_after_callback_and_gc")) ||
               !Check(Run(2), TEXT("native_removed_and_retained_snapshot")))
        return 1;
    FGameplayCueParameters Native;
    Native.RawMagnitude = 21;
    // This deliberately bypasses the DAS sending adapter: delivery still goes through the native notify.
    ASC->ExecuteGameplayCue(Impact, Native);
    if (!Check(Run(3), TEXT("native_caller_not_synthesized_wrapper_event")))
        return 1;
    FDasGASCueParameters Invalid;
    Invalid.NormalizedMagnitude = 2;
    if (!Check(!ASC->ExecuteAuthoredCue(Impact.GetTagName(), Invalid), TEXT("invalid_parameters_refused")))
        return 1;
    R.ReleaseObject(Host);
    if (!Check(R.SubscriptionCount(Host) == 0 && !ASC->GameplayCueReceived.IsBound(), TEXT("released_vm_unsubscribed")))
        return 1;
    ASC->ExecuteGameplayCue(Impact, Native);
    if (!Check(Run(4), TEXT("fresh_session_has_no_stale_callbacks")))
        return 1;
    R.ReleaseObject(Host);
    if (!ASC->AddAuthoredCue(Aura.GetTagName(), FDasGASCueParameters()))
        return 1;
    Host->Destroy();
    if (!Check(!ASC->IsGameplayCueActive(Aura), TEXT("endplay_cleans_manual_active_cues")))
        return 1;
    UE_LOG(LogTemp, Display, TEXT("DAS_GAS_CUES_RESULT=PASS checks=%d"), Checks);
    return 0;
}
