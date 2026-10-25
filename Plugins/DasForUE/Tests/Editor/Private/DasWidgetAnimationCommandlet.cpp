#include "DasWidgetAnimationCommandlet.h"
#include "DasRuntime.h"
#include "DasActor.h"
#include "DasScriptUserWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Animation/WidgetAnimationState.h"
#include "MovieScene.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
UDasWidgetAnimationCommandlet::UDasWidgetAnimationCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasWidgetAnimationCommandlet::Main(const FString& Params)
{
    FString Script;
    if (!FParse::Value(*Params, TEXT("Script="), Script))
        return 2;
    const bool OwnSlate = !FSlateApplication::IsInitialized();
    if (OwnSlate)
    {
        FSlateApplication::Create(MakeShareable(FNullApplication::CreateNullApplication()));
        FSlateApplication::Get().InitializeRenderer(FModuleManager::LoadModuleChecked<ISlateNullRendererModule>(
            TEXT("SlateNullRenderer")).CreateSlateNullRenderer());
    }
    ON_SCOPE_EXIT
    {
        if (OwnSlate)
            FSlateApplication::Shutdown();
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_WIDGET_ANIMATION_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    auto* Actor = World->SpawnActor<ADasActor>();
    TStrongObjectPtr<UDasWidgetAnimationFixture> Widget(NewObject<UDasWidgetAnimationFixture>(World.Get()));
    Widget->Initialize();
    TSharedPtr<SWidget> Slate = Widget->TakeWidget();
    TStrongObjectPtr<UWidgetAnimation> Animation(NewObject<UWidgetAnimation>(Widget.Get()));
    Animation->MovieScene = NewObject<UMovieScene>(Animation.Get());
    Animation->MovieScene->SetTickResolutionDirectly(FFrameRate(30, 1));
    Animation->MovieScene->SetDisplayRate(FFrameRate(30, 1));
    Animation->MovieScene->SetPlaybackRange(0, 3);
    Animation->SetSequenceFlags(EMovieSceneSequenceFlags::BlockingEvaluation);
    auto Run = [&](const TCHAR* Entry)
    {
        auto R = Runtime.RunNamedPersistent(Script, Entry, 0, Actor);
        if (!R.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("Animation issue %s: %s / %s"), Entry, *R.Error, *R.ReloadError);
        return R;
    };
    auto Setup = [&]()
    {
        TArray<FDasScalarValue> Args;
        for (UObject* O : {static_cast<UObject*>(Widget.Get()), static_cast<UObject*>(Animation.Get())})
        {
            FDasScalarValue V;
            V.Kind = 8;
            V.ObjectClass = TEXT("/Script/CoreUObject.Object");
            V.ObjectValue = O;
            Args.Add(V);
        }
        return Runtime.RunNamedFramePersistent(Script, TEXT("setup"), Args, EDasMethodSignature::Int, Actor);
    };
    Check(Setup().bSucceeded, TEXT("setup_weak_widget_and_asset_references"));
    auto Initial = Run(TEXT("play_all"));
    Check(Initial.bSucceeded && Initial.Value == 4, TEXT("four_generated_play_functions_return_live_handles"));
    for (auto Pair : {TPair<const TCHAR*, const TCHAR*>{TEXT("bad_null_asset"), TEXT("live animation asset")},
                                                        {TEXT("bad_speed"), TEXT("positive playback speed")}})
    {
        auto R = Run(Pair.Key);
        Check(!R.bSucceeded && R.Error.Contains(Pair.Value), TEXT("invalid_playback_rejected_before_native_dispatch"));
    }
    auto State = Widget->GetAnimationState(Animation.Get());
    Check(State && State->GetOnWidgetAnimationFinished().IsBound(), TEXT("native_completion_hook_registered"));
    CollectGarbage(RF_NoFlags);
    auto Live = Run(TEXT("read_live"));
    Check(Live.bSucceeded && Live.Value == 4, TEXT("typed_globals_arrays_clone_and_GC_retain_weak_capabilities"));
    Widget->TickActionsAndAnimation(0.25f);
    Widget->FlushAnimations();
    Widget->TickActionsAndAnimation(0.25f);
    Widget->FlushAnimations();
    Check(State && State->GetPlaybackStatus() == EMovieScenePlayerStatus::Stopped,
          TEXT("native_animation_completes_by_time_advancement"));
    Check(State && State->GetAnimationHandle().IsValid(),
          TEXT("native_weak_handle_can_remain_valid_while_finished_state_is_pinned"));
    auto Finished = Run(TEXT("read_finished"));
    Check(Finished.bSucceeded && Finished.Value == 4, TEXT("completed_animation_capabilities_are_invalid"));
    auto Replay = Run(TEXT("replay"));
    Check(Replay.bSucceeded && Replay.Value == 1,
          TEXT("replay_returns_new_live_capability_without_reviving_old_handles"));
    auto Sealed = Run(TEXT("sealed_result"));
    Check(Sealed.bSucceeded && Sealed.Value == 1,
          TEXT("same_entry_deferred_result_cannot_revive_after_reconstruction"));
    auto Resume = Run(TEXT("replay"));
    Check(Resume.bSucceeded && Resume.Value == 1, TEXT("new_animation_lease_after_reconstruction"));
    auto Late = Run(TEXT("make_late_frame"));
    Check(Late.bSucceeded && Late.Value == 1, TEXT("low_level_result_is_captured_before_DAS_reads_it"));
    auto Current = Widget->GetAnimationState(Animation.Get());
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    auto Dead = Run(TEXT("read_destroyed"));
    Check(Dead.bSucceeded && Dead.Value == 1, TEXT("NativeDestruct_revokes_handles_in_another_VM"));
    Slate = Widget->TakeWidget();
    auto OldResult = Run(TEXT("read_late_frame"));
    Check(!OldResult.bSucceeded &&
          OldResult.Error.Contains(TEXT("call handle is released")),
                                   TEXT("cross_entry_deferred_result_frame_is_already_released"));
    auto Fresh = Run(TEXT("replay"));
    Check(Fresh.bSucceeded && Fresh.Value == 1, TEXT("new_playback_after_reconstruction_is_valid"));
    Current = Widget->GetAnimationState(Animation.Get());
    Check(Current && Current->GetOnWidgetAnimationFinished().IsBound(), TEXT("reload_fixture_has_live_native_hooks"));
    FString Original;
    FFileHelper::LoadFileToString(Original, *Script);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Script);
    };
    FFileHelper::SaveStringToFile(Original + TEXT("\n// WidgetAnimation reload revision\n"), *Script);
    auto Reload = Run(TEXT("read_after_reload"));
    Check(Reload.bSucceeded && Reload.ReloadError.IsEmpty() && Reload.Revision > Fresh.Revision && Reload.Value == 1,
          TEXT("reload_resets_old_context_handles"));
    Check(Current && !Current->GetOnWidgetAnimationFinished().IsBound(),
          TEXT("reload_unsubscribes_old_native_hooks_without_stopping_UE_animation"));
    Check(Current && Current->GetPlaybackStatus() == EMovieScenePlayerStatus::Playing,
          TEXT("reload_does_not_own_or_stop_external_animation_state"));
    Check(Setup().bSucceeded, TEXT("new_context_accepts_native_widget_again"));
    auto Again = Run(TEXT("play_all"));
    Check(Again.bSucceeded && Again.Value == 4, TEXT("four_producers_work_after_reload"));
    auto HookState = Widget->GetAnimationState(Animation.Get());
    auto Drop = Run(TEXT("drop_handles"));
    Check(Drop.bSucceeded && Drop.Value == 1, TEXT("unreachable_handles_are_collected"));
    Check(HookState && !HookState->GetOnWidgetAnimationFinished().IsBound(),
          TEXT("collection_unsubscribes_native_hooks"));
    auto Keep = Run(TEXT("play_all"));
    Check(Keep.bSucceeded && Keep.Value == 4, TEXT("weak_ownership_fixture_ready"));
    Widget->StopAllAnimations();
    Widget->FlushAnimations();
    State.Reset();
    Current.Reset();
    HookState.Reset();
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    TWeakObjectPtr<UObject> WeakWidget = Widget.Get(), WeakAnimation = Animation.Get();
    Animation.Reset();
    Widget.Reset();
    CollectGarbage(RF_NoFlags);
    Check(!WeakWidget.IsValid() && !WeakAnimation.IsValid(), TEXT("DAS_handles_do_not_pin_widget_or_animation_asset"));
    auto Collected = Run(TEXT("read_finished"));
    Check(Collected.bSucceeded && Collected.Value == 4, TEXT("handles_remain_safely_invalid_after_native_GC"));
    Runtime.ReleaseObject(Actor);
    Check(Runtime.StructSnapshotCount(Actor) == 0, TEXT("owner_release_clears_weak_capability_table"));
    Actor->Destroy();
    UE_LOG(LogTemp, Display,
           TEXT("DAS_WIDGET_ANIMATION_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
