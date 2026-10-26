#include "DasTypeAdapters.h"
#include "DasWidgetAnimationTransport.h"
#include "Animation/WidgetAnimationState.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
bool DasIsWidgetAnimationType(const UScriptStruct* T)
{
    return DasHasNativeRole(T, EDasNativeTypeRole::WidgetAnimation);
}
bool DasWidgetAnimationFunctionAllowed(const UFunction* F)
{
    if (!F || !F->HasAnyFunctionFlags(FUNC_Native) || F->HasAnyFunctionFlags(FUNC_Static) ||
        F->GetOuterUClass() != UUserWidget::StaticClass())
        return false;
    static const TCHAR* Allowed[] = {TEXT("PlayAnimation"), TEXT("PlayAnimationForward"), TEXT("PlayAnimationReverse"), TEXT("PlayAnimationTimeRange")};
    for (const TCHAR* Name : Allowed)
        if (F->GetName() == Name)
            return true;
    return false;
}
FDasWidgetAnimationLease::~FDasWidgetAnimationLease()
{
    if (auto* W = Widget.Get())
        W->OnNativeDestruct.Remove(Destruct);
    if (auto State = Handle.PinAnimationState())
        State->GetOnWidgetAnimationFinished().Remove(Finished);
}
bool FDasWidgetAnimationLease::IsLive() const
{
    auto* W = Widget.Get();
    if (Revoked || !IsValid(W) || W->IsTemplate() || !W->GetWorld() || !W->GetWorld()->IsGameWorld() ||
        W->GetWorld()->bIsTearingDown || !W->GetCachedWidget().IsValid())
        return false;
    auto State = Handle.PinAnimationState();
    return State.IsValid() && State->IsValid() && !State->IsStopping() && !State->IsPendingDelete() &&
           State->GetUserWidget() == W;
}
TSharedPtr<FDasWidgetAnimationLease> BeginDasWidgetAnimationLease(UUserWidget* W)
{
    auto Lease = MakeShared<FDasWidgetAnimationLease>();
    Lease->Widget = W;
    TWeakPtr<FDasWidgetAnimationLease> Weak = Lease;
    Lease->Destruct = W->OnNativeDestruct.AddLambda(
        [Weak](UUserWidget*)
        {
            if (auto P = Weak.Pin())
                P->Revoked = true;
        });
    return Lease;
}
bool CompleteDasWidgetAnimationLease(const TSharedPtr<FDasWidgetAnimationLease>& Lease, const FWidgetAnimationHandle& H)
{
    if (!Lease)
        return false;
    Lease->Handle = H;
    if (!Lease->IsLive())
        return false;
    auto State = H.PinAnimationState();
    TWeakPtr<FDasWidgetAnimationLease> Weak = Lease;
    Lease->Finished = State->GetOnWidgetAnimationFinished().AddLambda(
        [Weak](FWidgetAnimationState&)
        {
            if (auto P = Weak.Pin())
                P->Revoked = true;
        });
    return true;
}
