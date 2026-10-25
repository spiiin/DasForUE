#include "DasInputNavigationCommandlet.h"
#include "DasRuntime.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateUser.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/App.h"
#include "UObject/GarbageCollection.h"
namespace
{
struct FCountingNavigation final : FNavigationConfig
{
    mutable int32 Actions = 0, Keys = 0;
    int32 Analogs = 0;
    virtual EUINavigationAction GetNavigationActionFromKey(const FKeyEvent& E) const override
    {
        ++Actions;
        return E.GetKey() == EKeys::F8 ? EUINavigationAction::Back : FNavigationConfig::GetNavigationActionFromKey(E);
    }
    virtual EUINavigation GetNavigationDirectionFromKey(const FKeyEvent& E) const override
    {
        ++Keys;
        return E.GetKey() == EKeys::F8 ? EUINavigation::Right : FNavigationConfig::GetNavigationDirectionFromKey(E);
    }
    virtual EUINavigation GetNavigationDirectionFromAnalog(const FAnalogInputEvent& E) override
    {
        ++Analogs;
        return FNavigationConfig::GetNavigationDirectionFromAnalog(E);
    }
};
}
UDasInputNavigationCommandlet::UDasInputNavigationCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasInputNavigationCommandlet::Main(const FString& Params)
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
    auto VirtualUser = FSlateApplication::Get().FindOrCreateVirtualUser(0);
    const uint32 UserIndex = VirtualUser->GetUserIndex();
    auto User = FSlateApplication::Get().GetUser(int32(UserIndex));
    auto PreviousConfig = User->GetUserNavigationConfig();
    auto Config = MakeShared<FCountingNavigation>();
    User->SetUserNavigationConfig(Config);
    ON_SCOPE_EXIT
    {
        User->SetUserNavigationConfig(PreviousConfig);
    };
    const double PreviousTime = FApp::GetCurrentTime();
    FApp::SetCurrentTime(PreviousTime + 100);
    ON_SCOPE_EXIT
    {
        FApp::SetCurrentTime(PreviousTime);
    };
    auto& Runtime = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_NAVIGATION_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    TStrongObjectPtr<UDasInputNavigationFixture> Widget(NewObject<UDasInputNavigationFixture>(World.Get()));
    Widget->Initialize();
    Widget->ScriptFile = Script;
    TSharedPtr<SWidget> Slate = Widget->TakeWidget();
    const FGeometry G = FGeometry::MakeRoot(FVector2D(100, 50), FSlateLayoutTransform());
    const FKeyEvent Key(EKeys::F8, FModifierKeysState(), UserIndex, false, 0, 0);
    const FAnalogInputEvent Axis(EKeys::Gamepad_LeftX, FModifierKeysState(), UserIndex, false, 0, 0, 0.99f),
        Neutral(EKeys::Gamepad_LeftX, FModifierKeysState(), UserIndex, false, 0, 0, 0.f);
    auto Run = [&](const TCHAR* Name)
    {
        return Runtime.RunNamedPersistent(Runtime.ScriptPath(Script), Name, 0, Widget.Get());
    };
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("key"));
    auto KeyReply = Slate->OnKeyDown(G, Key);
    if (!Widget->LastInputError.IsEmpty())
        UE_LOG(LogTemp, Display, TEXT("Navigation issue: %s"), *Widget->LastInputError);
    Check(KeyReply.IsEventHandled() && Widget->LastInputError.IsEmpty(),
          TEXT("generated_key_action_and_direction_use_current_native_config"));
    Check(Config->Actions == 1 && Config->Keys == 1, TEXT("key_queries_cached_once_per_function_callback"));
    CollectGarbage(RF_NoFlags);
    Check(Run(TEXT("read_saved")).Value == 1, TEXT("detached_key_data_remains_readable_after_callback_GC"));
    const int32 BeforeKey = Config->Keys;
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("stale"));
    Check(!Slate->OnKeyDown(G, Key).IsEventHandled() &&
          Widget->LastInputError.Contains(TEXT("current callback event")) && Config->Keys == BeforeKey,
                                          TEXT("old_key_snapshot_cannot_query_live_navigation"));
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("stale_frame"));
    Check(!Slate->OnKeyDown(G, Key).IsEventHandled() &&
          Widget->LastInputError.Contains(TEXT("released")), TEXT("navigation_frames_released_at_callback_exit"));
    auto Outside = Run(TEXT("outside"));
    Check(!Outside.bSucceeded && Outside.Error.Contains(TEXT("active synchronous")),
                                                        TEXT("navigation_rejected_outside_native_input_callback"));
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("key"));
    FKeyEvent Unknown(EKeys::F8, FModifierKeysState(), 777, false, 0, 0);
    Check(!Slate->OnKeyDown(G, Unknown).IsEventHandled() &&
          Widget->LastInputError.Contains(TEXT("initialized Slate user")) && Config->Keys == BeforeKey,
                                          TEXT("unknown_Slate_user_rejected_before_native_query"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("analog"));
    const int32 BeforeAnalog = Config->Analogs;
    Check(Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() && Widget->LastInputError.IsEmpty() &&
          Run(TEXT("last")).Value == 1, TEXT("analog_first_press_returns_direction"));
    Check(Config->Analogs == BeforeAnalog + 1, TEXT("repeated_DAS_queries_update_repeat_state_only_once"));
    Check(Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() &&
          Run(TEXT("last")).Value == 0, TEXT("next_native_event_observes_native_repeat_suppression"));
    FApp::SetCurrentTime(FApp::GetCurrentTime() + 0.3);
    Check(Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() &&
          Run(TEXT("last")).Value == 1, TEXT("native_repeat_resumes_after_threshold_without_sleep"));
    Slate->OnAnalogValueChanged(G, Neutral);
    Check(Run(TEXT("last")).Value == 0 && Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() &&
              Run(TEXT("last")).Value == 1, TEXT("neutral_axis_resets_native_repeat_state"));
    const int32 AfterAnalog = Config->Analogs;
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("stale_analog"));
    Check(!Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() &&
          Widget->LastInputError.Contains(TEXT("current callback event")) && Config->Analogs == AfterAnalog,
                                          TEXT("stale_analog_does_not_consume_native_repeat_state"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("noop"));
    Check(!Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() && Widget->LastInputError.IsEmpty() &&
              Config->Analogs == AfterAnalog,
          TEXT("ordinary_unhandled_analog_keeps_parent_fallback"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("analog_unhandled"));
    Check(Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() &&
          Widget->LastInputError.Contains(TEXT("consumed this input")) && Config->Analogs == AfterAnalog + 1,
                                          TEXT("consuming_query_cannot_replay_into_parent_navigation"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("analog_panic"));
    auto Panic = Slate->OnAnalogValueChanged(G, Axis);
    Check(
        Panic.IsEventHandled() && !Panic.GetMouseCaptor().IsValid() &&
        Widget->LastInputError.Contains(TEXT("deliberate navigation failure")),
                                        TEXT("panic_after_consuming_query_swallows_input_and_discards_reply_commands"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("wrong_signature"));
    const int32 BeforeWrong = Config->Analogs;
    Check(!Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() && Config->Analogs == BeforeWrong &&
          Widget->LastInputError.Contains(TEXT("Expected exported")),
                                          TEXT("wrong_handler_signature_does_not_consume_input"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("destroy_before"));
    Check(!Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() && Config->Analogs == BeforeWrong &&
          Widget->LastInputError.Contains(TEXT("expired")),
                                          TEXT("Destroy_before_query_revokes_capability_without_consumption"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Slate = Widget->TakeWidget();
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("key"));
    Check(Slate->OnKeyDown(G, Key).IsEventHandled(), TEXT("input_reconstructed_after_Destroy"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("destroy_after"));
    Check(Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() &&
          Widget->LastInputError.Contains(TEXT("expired")), TEXT("Destroy_after_query_does_not_replay_consumed_input"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Slate = Widget->TakeWidget();
    Check(Slate->OnKeyDown(G, Key).IsEventHandled(), TEXT("second_input_reconstruction"));
    FString Original;
    const FString Path = Runtime.ScriptPath(Script);
    FFileHelper::LoadFileToString(Original, *Path);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Path);
    };
    FFileHelper::SaveStringToFile(Original + TEXT("\n// navigation reload revision\n"), *Path);
    Check(Slate->OnKeyDown(G, Key).IsEventHandled() && Widget->LastInputError.IsEmpty(),
          TEXT("committed_reload_preserves_live_navigation_handlers"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("analog"));
    Check(Slate->OnAnalogValueChanged(G, Axis).IsEventHandled() && Widget->LastInputError.IsEmpty(),
          TEXT("analog_query_after_reload"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Check(Runtime.PersistentContextCount(World.Get()) == 0, TEXT("NativeDestruct_releases_navigation_VM"));
    UE_LOG(LogTemp, Display, TEXT("DAS_NAVIGATION_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
