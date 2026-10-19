#include "DasWidgetEventsCommandlet.h"
#include "DasScriptUserWidget.h"
#include "DasRuntime.h"
#include "Input/Events.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "Layout/WidgetPath.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"

UDasWidgetEventsCommandlet::UDasWidgetEventsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasWidgetEventsCommandlet::Main(const FString& Params)
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
    auto Check = [&](bool Ok, const TCHAR* Label)
    {
        ++Checks;
        if (!Ok)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_WIDGET_EVENTS_%s %s"), Ok ? TEXT("PASS"): TEXT("FAIL"), Label);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    TStrongObjectPtr<UDasScriptUserWidget> Widget(NewObject<UDasScriptUserWidget>(World.Get()));
    Widget->ScriptFile = Script;
    Widget->SetIsFocusable(true);
    Widget->Initialize();
    TSharedPtr<SWidget> Slate = Widget->TakeWidget();
    const bool GeometryProbe = FParse::Param(*Params, TEXT("Geometry"));
    FGeometry Geometry = FGeometry::MakeRoot(
        FVector2D(100, 50), GeometryProbe ? FSlateLayoutTransform(2.f, FVector2D(10, 20)) : FSlateLayoutTransform());
    const FKeyEvent Key(EKeys::SpaceBar, FModifierKeysState(), 0, false, 32, 32);
    Check(!Slate->OnKeyDown(Geometry, Key).IsEventHandled(), TEXT("missing_handler_uses_native_parent"));
    Check(!Widget->BindInputHandler(TEXT("InventedEvent"), TEXT("key_down")), TEXT("unknown_event_binding_rejected"));
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("key_down"));
    const auto KeyReply = Slate->OnKeyDown(Geometry, Key);
    if (!Widget->LastInputError.IsEmpty())
        UE_LOG(LogTemp, Display, TEXT("Widget issue: %s"), *Widget->LastInputError);
    Check(KeyReply.IsEventHandled() && KeyReply.ShouldSetUserFocus() && KeyReply.GetUserFocusRecepient() == Slate,
          TEXT("key_reply_returns_synchronously_to_slate"));
    if (GeometryProbe)
        Geometry = FGeometry::MakeRoot(FVector2D(7, 9), FSlateLayoutTransform(3.f, FVector2D(200, 300)));
    CollectGarbage(RF_NoFlags);
    const auto Read = Runtime.RunNamedPersistent(Runtime.ScriptPath(Script), TEXT("read_saved"), 0, Widget.Get());
    Check(Read.bSucceeded && Read.Value == 1, TEXT("typed_event_snapshot_survives_callback_and_gc"));
    if (GeometryProbe)
    {
        Check(Read.bSucceeded && Read.Value == 1,
              TEXT("geometry_snapshot_clone_survives_native_change_callback_and_gc"));
        Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("inspect_geometry"));
        Check(Slate->OnKeyDown(Geometry, Key).IsEventHandled() && Widget->LastInputError.IsEmpty(),
              TEXT("new_callback_geometry_is_independent_of_saved_revision"));
        for (const FName Handler : {FName(TEXT("old_signature")), FName(TEXT("wrong_geometry"))})
        {
            Widget->BindInputHandler(TEXT("OnKeyDown"), Handler);
            Check(!Slate->OnKeyDown(Geometry, Key).IsEventHandled() &&
                  Widget->LastInputError.Contains(TEXT("Expected exported")),
                                                  TEXT("geometry_callback_requires_exact_two_parameter_signature"));
        }
        Geometry = FGeometry::MakeRoot(FVector2D(100, 50), FSlateLayoutTransform(2.f, FVector2D(10, 20)));
    }
    Widget->BindInputHandler(TEXT("OnKeyUp"), TEXT("key_up"));
    Check(!Slate->OnKeyUp(Geometry, Key).IsEventHandled() && Widget->LastInputError.IsEmpty(),
          TEXT("explicit_unhandled_reply"));
    Widget->BindInputHandler(TEXT("OnKeyChar"), TEXT("character"));
    Check(Slate->OnKeyChar(Geometry, FCharacterEvent('A', FModifierKeysState(), 0, false)).IsEventHandled(),
          TEXT("typed_character_callback"));
    Widget->BindInputHandler(TEXT("OnAnalogValueChanged"), TEXT("analog"));
    Check(Slate
              ->OnAnalogValueChanged(
                  Geometry, FAnalogInputEvent(EKeys::Gamepad_LeftX, FModifierKeysState(), 0, false, 0, 0, 0.5f))
              .IsEventHandled(),
          TEXT("typed_analog_callback"));
    const TSet<FKey> Buttons = {EKeys::LeftMouseButton};
    const FPointerEvent Pointer(0, FVector2D(12, 20), FVector2D(10, 19), Buttons, EKeys::LeftMouseButton, 0,
                                FModifierKeysState());
    Widget->BindInputHandler(TEXT("OnMouseButtonDown"), TEXT("mouse_down"));
    const auto Mouse = Slate->OnMouseButtonDown(Geometry, Pointer);
    if (!Widget->LastInputError.IsEmpty())
        UE_LOG(LogTemp, Display, TEXT("Mouse issue: %s"), *Widget->LastInputError);
    Check(Mouse.IsEventHandled() && Mouse.GetMouseCaptor() == Slate && Mouse.GetMouseLockWidget() == Slate &&
              Mouse.GetDetectDragRequest() == Slate && Mouse.GetDetectDragRequestButton() == EKeys::LeftMouseButton &&
              Mouse.GetRequestedMousePos().IsSet() && Mouse.GetRequestedMousePos().GetValue() == FIntPoint(40, 50),
          TEXT("pointer_reply_capture_lock_drag_position"));
    Widget->BindInputHandler(TEXT("OnMouseButtonUp"), TEXT("mouse_up"));
    const auto Release = Slate->OnMouseButtonUp(Geometry, Pointer);
    Check(Release.IsEventHandled() && Release.ShouldReleaseMouse() && Release.ShouldReleaseMouseLock() &&
              Release.ShouldReleaseUserFocus() && Release.ShouldEndDragDrop(),
          TEXT("reply_release_unlock_focus_end_drag"));
    Widget->BindInputHandler(TEXT("OnTouchStarted"), TEXT("pointer"));
    Check(Slate->OnTouchStarted(Geometry, Pointer).IsEventHandled(), TEXT("touch_uses_pointer_snapshot"));
    for (const auto& Case : TArray<TPair<FName, FString>>{{TEXT("stale"), TEXT("expired")},
                                                           {TEXT("invalid_default"), TEXT("null")},
                                                            {TEXT("failing"), TEXT("deliberate widget failure")},
                                                             {TEXT("wrong_signature"), TEXT("Expected exported")}})
    {
        Widget->BindInputHandler(TEXT("OnKeyDown"), Case.Key);
        const auto Rejected = Slate->OnKeyDown(Geometry, Key);
        Check(!Rejected.IsEventHandled() && !Rejected.GetMouseCaptor().IsValid() &&
                  Widget->LastInputError.Contains(Case.Value),
              TEXT("invalid_reply_falls_back_without_partial_commands"));
    }
    const auto Outside = Runtime.RunNamedPersistent(Runtime.ScriptPath(Script), TEXT("outside"), 0, Widget.Get());
    Check(!Outside.bSucceeded && Outside.Error.Contains(TEXT("active synchronous widget callback")),
                                                        TEXT("reply_creation_outside_callback_rejected"));
    if (FParse::Param(*Params, TEXT("GeneratedReply")))
    {
        for (const auto& Case :
             TArray<TPair<FName, FString>>{{TEXT("stale_argument"), TEXT("expired")},
                                            {TEXT("stale_frame"), TEXT("call handle is released")},
                                             {TEXT("stale_result"), TEXT("call handle is released")},
                                              {TEXT("result_before_invoke"), TEXT("results require an executed call")},
                                               {TEXT("unassigned"), TEXT("missing input argument")},
                                                {TEXT("bad_position"), TEXT("finite int32")},
                                                 {TEXT("bad_drag_key"), TEXT("mouse or touch key")},
                                                  {TEXT("unhandled_command"), TEXT("handled reply")},
                                                   {TEXT("null_target"), TEXT("object handle is null")},
                                                    {TEXT("scope_limit"), TEXT("command limit")}})
        {
            Widget->BindInputHandler(TEXT("OnKeyDown"), Case.Key);
            const auto Reply = Slate->OnKeyDown(Geometry, Key);
            UE_LOG(LogTemp, Display,
                   TEXT("Expected generated rejection %s: %s"), *Case.Key.ToString(), *Widget->LastInputError);
            Check(!Reply.IsEventHandled() && !Reply.GetMouseCaptor().IsValid() &&
                      Widget->LastInputError.Contains(Case.Value),
                  TEXT("generated_reply_rejected_for_expected_reason"));
        }
        Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("immutable_reply"));
        const auto Immutable = Slate->OnKeyDown(Geometry, Key);
        Check(Immutable.IsEventHandled() && !Immutable.GetMouseCaptor().IsValid() && Immutable.ShouldReleaseMouse(),
              TEXT("generated_inout_does_not_mutate_input_token"));
        const auto StaleOutside =
            Runtime.RunNamedPersistent(Runtime.ScriptPath(Script), TEXT("old_result_outside"), 0, Widget.Get());
        Check(!StaleOutside.bSucceeded &&
              StaleOutside.Error.Contains(TEXT("call handle is released")),
                                          TEXT("reply_frames_revoked_when_callback_returns"));
        TStrongObjectPtr<UWorld> OtherWorld(UWorld::CreateWorld(EWorldType::Game, false));
        TStrongObjectPtr<UDasScriptUserWidget> Foreign(NewObject<UDasScriptUserWidget>(OtherWorld.Get()));
        Foreign->Initialize();
        TSharedPtr<SWidget> ForeignSlate = Foreign->TakeWidget();
        FDasScalarValue Arg;
        Arg.Kind = 8;
        Arg.ObjectClass = TEXT("/Script/CoreUObject.Object");
        Arg.ObjectValue = Foreign.Get();
        auto Set = Runtime.RunNamedFramePersistent(Runtime.ScriptPath(Script),
                                                   TEXT("set_foreign"), {Arg}, EDasMethodSignature::Int, Widget.Get());
        Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("foreign_target"));
        auto Rejected = Slate->OnKeyDown(Geometry, Key);
        Check(Set.bSucceeded && !Rejected.IsEventHandled() &&
              Widget->LastInputError.Contains(TEXT("callback world")),
                                              TEXT("generated_target_from_other_world_rejected"));
        ForeignSlate.Reset();
        Foreign->ReleaseSlateResources(true);
        OtherWorld->DestroyWorld(false);
        FString Original;
        const FString Path = Runtime.ScriptPath(Script);
        FFileHelper::LoadFileToString(Original, *Path);
        ON_SCOPE_EXIT
        {
            FFileHelper::SaveStringToFile(Original, *Path);
        };
        FFileHelper::SaveStringToFile(Original + TEXT("\n// EventReply reload revision\n"), *Path);
        Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("stale"));
        const auto ReloadReply = Slate->OnKeyDown(Geometry, Key);
        Check(!ReloadReply.IsEventHandled() &&
              Widget->LastInputError.Contains(TEXT("null")), TEXT("reload_does_not_restore_reply_from_old_callback"));
        Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("key_down"));
        Check(Slate->OnKeyDown(Geometry, Key).IsEventHandled() && Widget->LastInputError.IsEmpty(),
              TEXT("generated_wrappers_work_after_reload"));
        Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("stale_argument"));
        const auto AfterReload = Slate->OnKeyDown(Geometry, Key);
        Check(!AfterReload.IsEventHandled() &&
              Widget->LastInputError.Contains(TEXT("expired")),
                                              TEXT("reply_created_after_reload_expires_at_next_callback"));
    }
    Widget->BindInputHandler(TEXT("OnKeyDown"), TEXT("key_down"));
    Check(Slate->OnKeyDown(Geometry, Key).IsEventHandled() && Widget->LastInputError.IsEmpty(),
          TEXT("callback_recovers_after_failures"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Check(Runtime.PersistentContextCount(World.Get()) == 0 && Runtime.StructSnapshotCount(Widget.Get()) == 0,
          TEXT("native_destruct_releases_widget_context"));
    UE_LOG(LogTemp, Display,
           TEXT("DAS_WIDGET_EVENTS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
