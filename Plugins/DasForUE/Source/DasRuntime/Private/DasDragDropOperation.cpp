#include "DasDragDropOperation.h"
#include "DasScriptUserWidget.h"
#include "Engine/World.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "Slate/UMGDragDropOp.h"
#include "Containers/Ticker.h"
void UDasDragDropOperation::Initialize(UDasScriptUserWidget* Source, UObject* Data, UWidget* Visual,
                                       const FString& Label, int32 InPivot, const FVector2D& InOffset,
                                       uint32 InUserIndex, uint32 InPointerIndex)
{
    SourceWidget = Source;
    UserIndex = InUserIndex;
    PointerIndex = InPointerIndex;
    Payload = Data;
    DefaultDragVisual = Visual;
    Tag = Label;
    Pivot = EDragPivot(InPivot);
    Offset = InOffset;
    Live = true;
}
UWorld* UDasDragDropOperation::GetWorld() const
{
    return GetTypedOuter<UWorld>();
}
bool UDasDragDropOperation::IsLive() const
{
    auto* Source = SourceWidget.Get();
    return Live && Source && Source->GetCachedWidget().IsValid() && Source->GetWorld() &&
           !Source->GetWorld()->bIsTearingDown;
}
void UDasDragDropOperation::Revoke()
{
    if (!Live)
        return;
    Live = false;
    SourceWidget.Reset();
    Payload = nullptr;
    DefaultDragVisual = nullptr;
    Tag.Empty();
    OnDrop.Clear();
    OnDragCancelled.Clear();
    OnDragged.Clear();
    // Cancel only this native drag, after VM unwinds. Never cancel another application's operation.
    if (!IsInGameThread() || !FSlateApplication::IsInitialized())
        return;
    auto User = FSlateApplication::Get().GetUser(int32(UserIndex));
    if (!User)
        return;
    auto Current = User->GetDragDropContent();
    if (!Current.IsValid() || !Current->IsOfType<FUMGDragDropOp>() ||
        StaticCastSharedPtr<FUMGDragDropOp>(Current)->GetOperation() != this)
        return;
    const TWeakObjectPtr<UDasDragDropOperation> Weak(this);
    const uint32 UserId = UserIndex;
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda(
        [Weak, UserId](float)
        {
            if (!FSlateApplication::IsInitialized())
                return false;
            auto SlateUser = FSlateApplication::Get().GetUser(int32(UserId));
            if (!SlateUser)
                return false;
            auto Drag = SlateUser->GetDragDropContent();
            if (auto* Op = Weak.Get(); Op && Drag.IsValid() && Drag->IsOfType<FUMGDragDropOp>() &&
                                       StaticCastSharedPtr<FUMGDragDropOp>(Drag)->GetOperation() == Op)
                SlateUser->CancelDragDrop();
            return false;
        }));
}
void UDasDragDropOperation::Drop_Implementation(const FPointerEvent& Event)
{
    if (IsLive())
        Super::Drop_Implementation(Event);
    Revoke();
}
void UDasDragDropOperation::DragCancelled_Implementation(const FPointerEvent& Event)
{
    if (IsLive())
        Super::DragCancelled_Implementation(Event);
    Revoke();
}
void UDasDragDropOperation::Dragged_Implementation(const FPointerEvent& Event)
{
    if (IsLive())
        Super::Dragged_Implementation(Event);
    else
        Revoke();
}
bool FDasDragLifetime::Track(UDasDragDropOperation* Op)
{
    Operations.RemoveAll([](const auto& W) { return !W.IsValid() || !W->IsLive(); });
    if (Operations.Contains(Op))
        return true;
    if (Operations.Num() >= 64)
        return false;
    Operations.Add(Op);
    return true;
}
void FDasDragLifetime::Revoke()
{
    for (auto& W : Operations)
        if (auto* Op = W.Get())
            Op->Revoke();
    Operations.Empty();
}
