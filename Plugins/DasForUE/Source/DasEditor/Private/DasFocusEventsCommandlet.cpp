#include "DasFocusEventsCommandlet.h"
#include "DasRuntime.h"
#include "DasInputEvents.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "UObject/StrongObjectPtr.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "UObject/GarbageCollection.h"
UDasFocusEventsCommandlet::UDasFocusEventsCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasFocusEventsCommandlet::Main(const FString& Params)
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
        UE_LOG(LogTemp, Display, TEXT("DAS_FOCUS_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    TStrongObjectPtr<UDasFocusEventsFixture> Widget(NewObject<UDasFocusEventsFixture>(World.Get()));
    Widget->Initialize();
    Widget->ScriptFile = Script;
    TSharedPtr<SWidget> Slate = Widget->TakeWidget();
    const FGeometry G = FGeometry::MakeRoot(FVector2D(100, 50), FSlateLayoutTransform());
    const FFocusEvent Received(EFocusCause::Navigation, 37), Lost(EFocusCause::Cleared, 37);
    auto Run = [&](const TCHAR* Name)
    {
        return Runtime.RunNamedPersistent(Runtime.ScriptPath(Script), Name, 0, Widget.Get());
    };
    auto Receive = [&](const TCHAR* Name)
    {
        Widget->BindInputHandler(TEXT("OnFocusReceived"), Name);
        return Slate->OnFocusReceived(G, Received);
    };
    auto Lose = [&](const TCHAR* Name)
    {
        Widget->BindInputHandler(TEXT("OnFocusLost"), Name);
        Slate->OnFocusLost(Lost);
    };
    Check(!Widget->BindInputHandler(TEXT("OnFocusChanging"), TEXT("lost")),
                                    TEXT("borrowed_widget_path_event_not_exposed"));
    Check(!Slate->OnFocusReceived(G, Received).IsEventHandled(), TEXT("unbound_receive_uses_native_parent"));
    Check(Receive(TEXT("received")).IsEventHandled() && Widget->LastInputError.IsEmpty(),
                  TEXT("real_SObjectWidget_received_returns_synchronous_reply"));
    if (!Widget->LastInputError.IsEmpty())
        UE_LOG(LogTemp, Display, TEXT("Focus issue: %s"), *Widget->LastInputError);
    Check(Run(TEXT("get_score")).Value == 11, TEXT("received_executes_in_persistent_VM"));
    Lose(TEXT("lost"));
    Check(Widget->LastInputError.IsEmpty() && Run(TEXT("get_losses")).Value == 1,
                                                  TEXT("real_SObjectWidget_lost_receives_user_and_cause"));
    CollectGarbage(RF_NoFlags);
    Check(Run(TEXT("read_saved")).Value == 1, TEXT("owned_focus_snapshot_survives_callback_and_GC"));
    auto Focus = Receive(TEXT("focus_command"));
    Check(Focus.IsEventHandled() && Focus.GetUserFocusRecepient() == Slate,
          TEXT("received_focus_command_targets_live_widget"));
    Check(Receive(TEXT("reenter")).IsEventHandled() && Widget->bReentryRejected &&
                  Run(TEXT("get_losses")).Value == 1, TEXT("reentrant_focus_loss_cannot_enter_active_VM"));
    Check(!Run(TEXT("null_event")).bSucceeded, TEXT("null_focus_handle_rejected"));
    Check(!Receive(TEXT("stale_reply")).IsEventHandled() &&
                   Widget->LastInputError.Contains(TEXT("expired")),
                                                   TEXT("reply_from_previous_focus_callback_revoked"));
    Lose(TEXT("lost_reply"));
    Check(Widget->LastInputError.Contains(TEXT("active synchronous")),
                                          TEXT("void_focus_loss_cannot_issue_reply_commands"));
    Lose(TEXT("lost_panic"));
    Check(Widget->LastInputError.Contains(TEXT("deliberate focus loss failure")),
                                          TEXT("focus_loss_exception_contained"));
    auto Panic = Receive(TEXT("panic_received"));
    Check(!Panic.IsEventHandled() && !Panic.GetUserFocusRecepient().IsValid() &&
          Widget->LastInputError.Contains(TEXT("deliberate focus receive failure")),
                                          TEXT("failed_receive_discards_focus_commands"));
    Check(!Receive(TEXT("wrong_received")).IsEventHandled() &&
                   Widget->LastInputError.Contains(TEXT("Expected exported")), TEXT("received_signature_checked"));
    Lose(TEXT("wrong_lost"));
    Check(Widget->LastInputError.Contains(TEXT("Expected exported")), TEXT("lost_signature_checked"));
    Check(Receive(TEXT("received")).IsEventHandled() && Widget->LastInputError.IsEmpty(),
                  TEXT("VM_recovers_after_errors"));
    Lose(TEXT("lost"));
    const FString Path = Runtime.ScriptPath(Script);
    FString Original;
    FFileHelper::LoadFileToString(Original, *Path);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Path);
    };
    FString Revision = Original.Replace(TEXT("score=11"), TEXT("score=22"));
    FFileHelper::SaveStringToFile(Revision, *Path);
    Check(Receive(TEXT("received")).IsEventHandled() && Run(TEXT("get_score")).Value == 22,
                                                            TEXT("hot_reload_replaces_focus_receive_handler"));
    Lose(TEXT("lost"));
    Check(Widget->LastInputError.IsEmpty() && Run(TEXT("get_losses")).Value == 1,
                                                  TEXT("hot_reload_retains_void_focus_contract"));
    FFileHelper::SaveStringToFile(
        Revision.Replace(TEXT("def lost(e:UEFocusEvent):void"), TEXT("def lost(e:UEFocusEvent):int")), *Path);
    Check(Receive(TEXT("received")).IsEventHandled() && Run(TEXT("get_score")).Value == 22 &&
                                                            !Run(TEXT("get_score")).ReloadError.IsEmpty(),
                                                                 TEXT("bad_reload_keeps_working_focus_VM"));
    FFileHelper::SaveStringToFile(
        Revision.Replace(TEXT("def lost(e:UEFocusEvent):void"), TEXT("def removed_lost(e:UEFocusEvent):void")), *Path);
    Check(Receive(TEXT("received")).IsEventHandled() &&
                  Run(TEXT("get_score"))
                          .ReloadError.Contains(TEXT("Previously used entry point signature changed: lost")),
                                                TEXT("compiling_reload_cannot_remove_required_focus_loss_handler"));
    FFileHelper::SaveStringToFile(Revision + TEXT("\n// recovered focus revision\n"), *Path);
    Check(Receive(TEXT("received")).IsEventHandled(), TEXT("focus_reload_recovers_after_rejection"));
    Widget->BindInputHandler(TEXT("OnFocusReceived"), NAME_None);
    Check(!Slate->OnFocusReceived(G, Received).IsEventHandled(), TEXT("explicit_unbind_removes_received_handler"));
    Check(!Receive(TEXT("destroy_received")).IsEventHandled() &&
                   Widget->LastInputError.Contains(TEXT("expired")), TEXT("Destroy_during_received_revokes_reply"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Check(Runtime.PersistentContextCount(World.Get()) == 0, TEXT("Destroy_releases_VM_and_scoped_handles"));
    Slate = Widget->TakeWidget();
    Check(Receive(TEXT("received")).IsEventHandled(), TEXT("fresh_VM_after_widget_reconstruction"));
    Lose(TEXT("destroy_lost"));
    Check(Widget->LastInputError.Contains(TEXT("expired")), TEXT("Destroy_during_lost_invalidates_notification"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    Check(Runtime.PersistentContextCount(World.Get()) == 0, TEXT("focus_loss_Destroy_leaves_no_VM"));
    Slate = Widget->TakeWidget();
    World->bIsTearingDown = true;
    Check(!Receive(TEXT("received")).IsEventHandled() && !Widget->LastInputError.IsEmpty(),
                   TEXT("world_teardown_rejects_focus_callback"));
    World->bIsTearingDown = false;
    for (uint8 Cause = 0; Cause <= uint8(EFocusCause::WindowActivate); ++Cause)
    {
        FFocusEvent E(EFocusCause(Cause), 37);
        FInstancedStruct Snapshot;
        FString Error;
        Check(CaptureDasInputEvent(FFocusEvent::StaticStruct(), &E, Snapshot, Error) &&
                  Snapshot.Get<FFocusEvent>().GetCause() == E.GetCause() && Snapshot.Get<FFocusEvent>().GetUser() == 37,
              TEXT("each_native_focus_cause_copied"));
    }
    FFocusEvent Invalid(EFocusCause(255), 37);
    FInstancedStruct InvalidSnapshot;
    FString Error;
    Check(!CaptureDasInputEvent(FFocusEvent::StaticStruct(), &Invalid, InvalidSnapshot, Error),
          TEXT("invalid_native_focus_cause_rejected"));
    Slate.Reset();
    Widget->ReleaseSlateResources(true);
    UE_LOG(LogTemp, Display, TEXT("DAS_FOCUS_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
