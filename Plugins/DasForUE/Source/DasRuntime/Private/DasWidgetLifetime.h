#pragma once
#include "DasGameplayWidgets.h"
#include "GameFramework/PlayerController.h"
#include "UObject/StrongObjectPtr.h"

struct FDasWidgetLifetime
{
    TArray<TStrongObjectPtr<UUserWidget>> Widgets;
    TArray<TStrongObjectPtr<UDasWidgetCallback>> Buttons;
    TWeakObjectPtr<APlayerController> InputPlayer;
    bool PreviousCursor = false;
    void ResetInput()
    {
        if (auto* P = InputPlayer.Get())
        {
            P->SetInputMode(FInputModeGameOnly());
            P->bShowMouseCursor = PreviousCursor;
        }
        InputPlayer.Reset();
    }
    void Remove(UUserWidget* W)
    {
        for (auto& P : Buttons)
            if (P->Widget.Get() == W)
                P->Deactivate();
        Buttons.RemoveAll([&](const auto& P) { return P->Widget.Get() == W; });
        if (InputPlayer.Get() == W->GetOwningPlayer())
            ResetInput();
        W->RemoveFromParent();
        Widgets.RemoveAll([&](const auto& P) { return P.Get() == W; });
    }
    ~FDasWidgetLifetime()
    {
        for (auto& P : Buttons)
            P->Deactivate();
        ResetInput();
        for (auto& W : Widgets)
            if (IsValid(W.Get()))
                W->RemoveFromParent();
    }
    bool Owns(UUserWidget* W) const
    {
        return Widgets.ContainsByPredicate([&](const auto& P) { return P.Get() == W; });
    }
};
