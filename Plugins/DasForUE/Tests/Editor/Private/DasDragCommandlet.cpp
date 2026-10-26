#include "DasDragCommandlet.h"
#include "DasDragDropOperation.h"
#include "DasRuntime.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Slate/UMGDragDropOp.h"
#include "Slate/SlateBrushAsset.h"
#include "Slate/SObjectWidget.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Layout/WidgetPath.h"
#include "Layout/ArrangedChildren.h"
#include "Interfaces/ISlateNullRendererModule.h"
#include "Null/NullApplication.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GarbageCollection.h"
#include "Misc/ScopeExit.h"
#include "Misc/FileHelper.h"
#include "Containers/Ticker.h"
void UDasDragFixture::CollectDragGarbageForTest()
{
    CollectGarbage(RF_NoFlags);
}
UDasDragCommandlet::UDasDragCommandlet()
{
    IsClient = false;
    IsServer = false;
    IsEditor = true;
    LogToConsole = true;
}
int32 UDasDragCommandlet::Main(const FString& Params)
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
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    int32 Checks = 0, Failures = 0;
    auto Check = [&](bool Good, const TCHAR* Name)
    {
        ++Checks;
        if (!Good)
            ++Failures;
        UE_LOG(LogTemp, Display, TEXT("DAS_DRAG_%s %s"), Good ? TEXT("PASS"): TEXT("FAIL"), Name);
    };
    TStrongObjectPtr<UWorld> World(UWorld::CreateWorld(EWorldType::Game, false));
    GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World.Get());
    ON_SCOPE_EXIT
    {
        GEngine->DestroyWorldContext(World.Get());
        World->DestroyWorld(false);
    };
    TStrongObjectPtr<UDasDragFixture> Source(NewObject<UDasDragFixture>(World.Get())),
        Target(NewObject<UDasDragFixture>(World.Get()));
    TStrongObjectPtr<UDasScriptUserWidget> Visual(NewObject<UDasScriptUserWidget>(World.Get()));
    for (auto* W : {static_cast<UDasScriptUserWidget*>(Source.Get()), static_cast<UDasScriptUserWidget*>(Target.Get()),
                    Visual.Get()})
        W->Initialize();
    Source->ScriptFile = Script;
    Target->ScriptFile = Script;
    TSharedPtr<SWidget> SourceSlate = Source->TakeWidget(), TargetSlate = Target->TakeWidget(),
                        VisualSlate = Visual->TakeWidget();
    TStrongObjectPtr<UObject> Payload(NewObject<USlateBrushAsset>(World.Get()));
    TWeakObjectPtr<UObject> WeakPayload = Payload.Get();
    const FGeometry G = FGeometry::MakeRoot(FVector2D(100, 50), FSlateLayoutTransform());
    const FPointerEvent Mouse(0, FVector2D(30, 20), FVector2D(20, 10), TSet<FKey>{EKeys::LeftMouseButton},
                              EKeys::LeftMouseButton, 0, FModifierKeysState());
    const FPointerEvent Right(0, FVector2D(30, 20), FVector2D(20, 10), TSet<FKey>{EKeys::RightMouseButton},
                              EKeys::RightMouseButton, 0, FModifierKeysState());
    auto Run = [&](UDasDragFixture* W, const TCHAR* Entry)
    {
        auto V = R.RunNamedPersistent(R.ScriptPath(Script), Entry, 0, W);
        if (!V.bSucceeded)
            UE_LOG(LogTemp, Display, TEXT("Drag issue %s: %s"), Entry, *V.Error);
        return V;
    };
    auto Setup = [&](UDasDragFixture* W)
    {
        TArray<FDasScalarValue> A;
        for (auto* O : {Payload.Get(), static_cast<UObject*>(Visual.Get())})
        {
            FDasScalarValue V;
            V.Kind = 8;
            V.ObjectClass = TEXT("/Script/CoreUObject.Object");
            V.ObjectValue = O;
            A.Add(V);
        }
        return R.RunNamedFramePersistent(R.ScriptPath(Script),
                                         TEXT("setup"), A, EDasMethodSignature::Int, W).bSucceeded;
    };
    Source->BindInputHandler(TEXT("OnMouseButtonDown"), TEXT("mouse_down"));
    Target->BindInputHandler(TEXT("OnMouseButtonDown"), TEXT("mouse_down"));
    Check(!Source->BindDragHandler(TEXT("InventedDrag"), TEXT("detected")), TEXT("unknown_drag_event_rejected"));
    Check(!SourceSlate->OnDragDetected(G, Mouse).IsEventHandled(), TEXT("missing_handler_native_fallback"));
    const auto Detect = SourceSlate->OnMouseButtonDown(G, Mouse);
    Check(Detect.IsEventHandled() && Detect.GetDetectDragRequest() == SourceSlate &&
              Detect.GetDetectDragRequestButton() == EKeys::LeftMouseButton && Source->LastInputError.IsEmpty(),
          TEXT("generated_DetectDragIfPressed_requests_native_detection"));
    TargetSlate->OnMouseButtonDown(G, Mouse);
    Check(Setup(Source.Get()) && Setup(Target.Get()), TEXT("payload_and_visual_setup"));
    SourceSlate->OnMouseButtonDown(G, Right);
    Source->BindInputHandler(TEXT("OnTouchStarted"), TEXT("mismatch_button"));
    const FPointerEvent Touch(uint32(0), uint32(1), FVector2D(30, 20), FVector2D(20, 10), 1.f, true, false, false,
                              FModifierKeysState());
    auto TouchReply = SourceSlate->OnTouchStarted(G, Touch);
    Check(TouchReply.IsEventHandled() && TouchReply.GetDetectDragRequestButton() == EKeys::RightMouseButton &&
              Source->LastInputError.IsEmpty(),
          TEXT("native_touch_detection_preserves_mismatching_effecting_button_semantics"));
    SourceSlate->OnMouseButtonDown(G, Mouse);
    Source->BindInputHandler(TEXT("OnMouseButtonDown"), TEXT("mismatch_button"));
    auto Mismatch = SourceSlate->OnMouseButtonDown(G, Right);
    Check(!Mismatch.IsEventHandled() && Source->LastInputError.IsEmpty(),
          TEXT("native_nonmatching_button_is_unhandled"));
    Source->BindInputHandler(TEXT("OnMouseButtonDown"), TEXT("stale_event"));
    auto Stale = SourceSlate->OnMouseButtonDown(G, Mouse);
    Check(!Stale.IsEventHandled() &&
          Source->LastInputError.Contains(TEXT("current callback pointer")),
                                          TEXT("old_pointer_snapshot_cannot_request_live_drag"));
    Source->BindInputHandler(TEXT("OnMouseButtonDown"), TEXT("mouse_down"));
    Source->BindDragHandler(TEXT("OnDragDetected"), TEXT("detected"));
    Source->BindDragHandler(TEXT("OnDragCancelled"), TEXT("cancel"));
    for (auto Pair : {TPair<FName, FName>(TEXT("OnDragEnter"), TEXT("enter")),
                                          {TEXT("OnDragOver"), TEXT("over")},
                                           {TEXT("OnDragLeave"), TEXT("leave")}, {TEXT("OnDrop"), TEXT("drop")}})
        Target->BindDragHandler(Pair.Key, Pair.Value);
    auto Start = [&]()
    {
        auto Reply = SourceSlate->OnDragDetected(G, Mouse);
        if (!Source->LastDragError.IsEmpty())
            UE_LOG(LogTemp, Display, TEXT("Detected issue: %s"), *Source->LastDragError);
        auto Base = Reply.GetDragDropContent();
        return Base.IsValid() && Base->IsOfType<FUMGDragDropOp>() ? StaticCastSharedPtr<FUMGDragDropOp>(Base)
                                                                  : TSharedPtr<FUMGDragDropOp>();
    };
    auto Activate = [&](const TSharedRef<FDragDropOperation>& Content)
    {
        auto Window = SNew(SWindow);
        TArray<FWidgetAndPointer> Nodes;
        Nodes.Emplace(FArrangedWidget(Window, G));
        Nodes.Emplace(FArrangedWidget(SourceSlate.ToSharedRef(), G));
        FWidgetPath Path{MakeArrayView(Nodes)};
        FSlateApplication::Get().ProcessReply(Path, FReply::Handled().BeginDragDrop(Content), &Path, &Mouse);
    };
    auto Op = Start();
    Check(Op.IsValid(), TEXT("SObjectWidget_creates_native_FUMGDragDropOp"));
    if (!Op)
    {
        UE_LOG(LogTemp, Display, TEXT("DAS_DRAG_RESULT=FAIL checks=%d"), Checks);
        return 1;
    }
    auto* Data = Cast<UDasDragDropOperation>(Op->GetOperation());
    Check(Data && Data->IsLive() && Data->Payload == Payload.Get() && Data->DefaultDragVisual == Visual.Get() &&
          Data->Tag == TEXT("inventory") && Data->Pivot == EDragPivot::CenterCenter,
                            TEXT("native_operation_has_payload_visual_pivot_and_tag"));
    TStrongObjectPtr<UDragDropOperation> Foreign(NewObject<UDragDropOperation>(World.Get()));
    auto ForeignNative = FUMGDragDropOp::New(Foreign.Get(), 0, FVector2D(30, 20), FVector2D(0, 0), 1,
                                             StaticCastSharedPtr<SObjectWidget>(SourceSlate));
    FDragDropEvent ForeignEvent(Mouse, ForeignNative);
    Check(!TargetSlate->OnDragOver(G, ForeignEvent).IsEventHandled() &&
          Target->LastDragError.Contains(TEXT("foreign")),
                                         TEXT("foreign_operation_not_accepted_as_managed_capability"));
    const FPointerEvent OtherPointer(1, FVector2D(30, 20), FVector2D(20, 10), TSet<FKey>{EKeys::LeftMouseButton},
                                     EKeys::LeftMouseButton, 0, FModifierKeysState());
    const FDragDropEvent WrongPointer(OtherPointer, Op);
    Check(!TargetSlate->OnDragOver(G, WrongPointer).IsEventHandled() &&
          Target->LastDragError.Contains(TEXT("foreign")), TEXT("operation_is_bound_to_original_pointer"));
    const FDragDropEvent E(Mouse, Op);
    TargetSlate->OnDragEnter(G, E);
    Check(Target->LastDragError.IsEmpty(), TEXT("drag_enter_callback"));
    Check(TargetSlate->OnDragOver(G, E).IsEventHandled() && Target->LastDragError.IsEmpty(),
          TEXT("drag_over_returns_bool_synchronously"));
    TargetSlate->OnDragLeave(E);
    Check(Run(Target.Get(), TEXT("counts")).Value == 1100, TEXT("drag_enter_leave_counts"));
    CollectGarbage(RF_NoFlags);
    Check(Data->IsLive() && Run(Source.Get(), TEXT("read_snapshot")).Value == 1,
                                TEXT("owned_geometry_pointer_snapshots_and_operation_survive_GC"));
    auto OutsideCreate = Run(Source.Get(), TEXT("outside_create"));
    Check(!OutsideCreate.bSucceeded && OutsideCreate.Error.Contains(TEXT("active synchronous drag")),
                                                                    TEXT("factory_outside_drag_detected_rejected"));
    auto OutsideData = Run(Source.Get(), TEXT("outside_data"));
    Check(!OutsideData.bSucceeded && OutsideData.Error.Contains(TEXT("active synchronous drag")),
                                                                TEXT("data_access_outside_drag_callback_rejected"));
    const bool Dropped = TargetSlate->OnDrop(G, E).IsEventHandled();
    Check(Dropped && Run(Target.Get(), TEXT("counts")).Value == 1101, TEXT("drop_callback_delivers_payload"));
    Op->OnDrop(Dropped, Mouse);
    Check(!Data->IsLive() && !Data->Payload && !Data->DefaultDragVisual && Data->Tag.IsEmpty() &&
          Run(Source.Get(), TEXT("read_saved")).Value == 0, TEXT("native_drop_revokes_and_clears_operation"));
    Op = Start();
    if (!Op)
        return 1;
    Data = CastChecked<UDasDragDropOperation>(Op->GetOperation());
    Op->OnDrop(false, Mouse);
    Check(!Data->IsLive() && Run(Source.Get(), TEXT("counts")).Value == 10,
                                 TEXT("native_cancel_notifies_source_and_revokes"));
    for (auto Case : TArray<TPair<FName, FString>>{{TEXT("old_return"), TEXT("newly created")},
                                                    {TEXT("panic_create"), TEXT("deliberate drag failure")},
                                                     {TEXT("duplicate_create"), TEXT("exactly one")},
                                                      {TEXT("bad_pivot"), TEXT("invalid drag")},
                                                       {TEXT("bad_offset"), TEXT("invalid drag")},
                                                        {TEXT("wrong_signature"), TEXT("Expected exported")}})
    {
        Source->BindDragHandler(TEXT("OnDragDetected"), Case.Key);
        auto Bad = Start();
        Check(!Bad && Source->LastDragError.Contains(Case.Value),
              TEXT("invalid_detected_handler_falls_back_without_operation"));
    }
    Check(Run(Source.Get(), TEXT("read_saved")).Value == 0, TEXT("failed_factory_does_not_leave_active_operation"));
    Source->BindDragHandler(TEXT("OnDragDetected"), TEXT("null_return"));
    Check(!Start() && Source->LastDragError.IsEmpty(), TEXT("null_detected_result_is_valid_noop"));
    Source->BindDragHandler(TEXT("OnDragDetected"), TEXT("detected"));
    Op = Start();
    if (!Op)
        return 1;
    Data = CastChecked<UDasDragDropOperation>(Op->GetOperation());
    Target->BindDragHandler(TEXT("OnDrop"), TEXT("panic_drop"));
    FDragDropEvent FailingDrop(Mouse, Op);
    Check(!TargetSlate->OnDrop(G, FailingDrop).IsEventHandled() &&
          Target->LastDragError.Contains(TEXT("deliberate drop failure")),
                                         TEXT("failed_drop_does_not_publish_handled"));
    Op->OnDrop(false, Mouse);
    Check(!Data->IsLive(), TEXT("unhandled_drop_is_cancelled_by_native_operation"));
    Target->BindDragHandler(TEXT("OnDrop"), TEXT("drop"));
    Op = Start();
    if (!Op)
        return 1;
    Data = CastChecked<UDasDragDropOperation>(Op->GetOperation());
    FDragDropEvent DestroyTarget(Mouse, Op);
    Target->BindDragHandler(TEXT("OnDragOver"), TEXT("destroy_over"));
    Check(!TargetSlate->OnDragOver(G, DestroyTarget).IsEventHandled() && !Data->IsLive() &&
          Target->LastDragError.Contains(TEXT("expired")), TEXT("target_destroy_revokes_operation_and_discards_bool"));
    TargetSlate.Reset();
    Target->ReleaseSlateResources(true);
    TargetSlate = Target->TakeWidget();
    Target->BindDragHandler(TEXT("OnDragOver"), TEXT("over"));
    TargetSlate->OnMouseButtonDown(G, Mouse);
    Check(Setup(Target.Get()), TEXT("target_reconstruction_setup"));
    Op = Start();
    if (!Op)
        return 1;
    Data = CastChecked<UDasDragDropOperation>(Op->GetOperation());
    Activate(Op.ToSharedRef());
    Check(FSlateApplication::Get().IsDragDropping(), TEXT("operation_activated_in_native_Slate_application"));
    SourceSlate.Reset();
    Source->ReleaseSlateResources(true);
    FTSTicker::GetCoreTicker().Tick(0);
    Check(!FSlateApplication::Get().IsDragDropping(), TEXT("revoked_native_Slate_drag_is_cancelled_on_next_ticker"));
    Check(!Data->IsLive() && !Data->Payload, TEXT("source_NativeDestruct_revokes_live_drag"));
    SourceSlate = Source->TakeWidget();
    SourceSlate->OnMouseButtonDown(G, Mouse);
    Check(Setup(Source.Get()), TEXT("source_reconstruction_setup"));
    Source->BindDragHandler(TEXT("OnDragDetected"), TEXT("destroy_create"));
    Check(!Start() && Source->LastDragError.Contains(TEXT("expired")),
                                                     TEXT("source_destroy_during_factory_discards_operation"));
    SourceSlate.Reset();
    Source->ReleaseSlateResources(true);
    SourceSlate = Source->TakeWidget();
    SourceSlate->OnMouseButtonDown(G, Mouse);
    Check(Setup(Source.Get()), TEXT("source_second_reconstruction_setup"));
    Source->BindDragHandler(TEXT("OnDragDetected"), TEXT("detected"));
    Op = Start();
    if (!Op)
        return 1;
    Data = CastChecked<UDasDragDropOperation>(Op->GetOperation());
    FString Original;
    const FString Path = R.ScriptPath(Script);
    FFileHelper::LoadFileToString(Original, *Path);
    ON_SCOPE_EXIT
    {
        FFileHelper::SaveStringToFile(Original, *Path);
    };
    FString Invalid = Original.Replace(TEXT("def detected("), TEXT("def detected_removed("));
    FFileHelper::SaveStringToFile(Invalid, *Path);
    auto Rejected = Run(Source.Get(), TEXT("counts"));
    Check(Rejected.bSucceeded && !Rejected.ReloadError.IsEmpty() && Data->IsLive(),
          TEXT("rejected_reload_preserves_working_operation"));
    FFileHelper::SaveStringToFile(Original + TEXT("\n// drag reload revision\n"), *Path);
    auto Reload = Run(Source.Get(), TEXT("counts"));
    Check(Reload.bSucceeded && Reload.Revision > 1 && !Data->IsLive() && !Data->Payload,
          TEXT("committed_hot_reload_revokes_previous_VM_operations"));
    Check(Setup(Source.Get()), TEXT("reload_setup"));
    Op = Start();
    Check(Op.IsValid() && Source->LastDragError.IsEmpty(), TEXT("drag_handlers_work_after_reload"));
    if (!Op)
        return 1;
    Data = CastChecked<UDasDragDropOperation>(Op->GetOperation());
    FDragDropEvent TargetReload(Mouse, Op);
    Check(TargetSlate->OnDragOver(G, TargetReload).IsEventHandled(), TEXT("unrelated_new_drag_survives_target_reload"));
    Activate(Op.ToSharedRef());
    FFileHelper::SaveStringToFile(Original + TEXT("\n// drag target reload revision\n"), *Path);
    Check(!TargetSlate->OnDragOver(G, TargetReload).IsEventHandled() && !Data->IsLive() &&
          Target->LastDragError.Contains(TEXT("expired")), TEXT("target_reload_revokes_old_participating_operation"));
    FSlateApplication::Get().CancelDragDrop();
    Activate(ForeignNative);
    FTSTicker::GetCoreTicker().Tick(0);
    Check(FSlateApplication::Get().GetDragDroppingContent() == ForeignNative,
          TEXT("stale_deferred_cancel_does_not_cancel_unrelated_drag"));
    FSlateApplication::Get().CancelDragDrop();
    Payload.Reset();
    Op.Reset();
    CollectGarbage(RF_NoFlags);
    Check(!WeakPayload.IsValid(), TEXT("finished_or_revoked_operations_do_not_pin_payload"));
    SourceSlate.Reset();
    TargetSlate.Reset();
    VisualSlate.Reset();
    Source->ReleaseSlateResources(true);
    Target->ReleaseSlateResources(true);
    Visual->ReleaseSlateResources(true);
    FTSTicker::GetCoreTicker().Tick(0);
    Check(R.PersistentContextCount(World.Get()) == 0, TEXT("NativeDestruct_releases_drag_VMs"));
    UE_LOG(LogTemp, Display, TEXT("DAS_DRAG_RESULT=%s checks=%d"), Failures ? TEXT("FAIL"): TEXT("PASS"), Checks);
    return Failures ? 1 : 0;
}
