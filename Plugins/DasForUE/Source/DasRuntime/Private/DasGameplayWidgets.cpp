#include "DasGameplayWidgets.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Containers/Ticker.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "UObject/UnrealType.h"

TSharedRef<SWidget> UDasGameplayHUD::RebuildWidget()
{
    if (!WidgetTree)
        WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"), RF_Transient);
    if (!WidgetTree->RootWidget)
    {
        auto* Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("Root"));
        WidgetTree->RootWidget = Root;
        auto Text = [&](const TCHAR* Name, const TCHAR* Value)
        {
            auto* T = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
            T->SetText(FText::FromString(Value));
            auto Font = T->GetFont();
            Font.Size = 24;
            T->SetFont(Font);
            T->SetColorAndOpacity(FSlateColor(FLinearColor::White));
            Root->AddChildToVerticalBox(T)->SetPadding(FMargin(16, 8));
            return T;
        };
        Text(TEXT("Score"), TEXT("Score: 0"));
        Text(TEXT("Health"), TEXT("Health: 100 / 100"));
        Text(TEXT("Help"), TEXT("E: interact | H: damage | Tab: mouse/UI"));
        auto* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("Damage"));
        auto* Caption = WidgetTree->ConstructWidget<UTextBlock>();
        Caption->SetText(FText::FromString(TEXT("Receive 25 damage")));
        Button->AddChild(Caption);
        Root->AddChildToVerticalBox(Button)->SetPadding(FMargin(16, 8));
    }
    return Super::RebuildWidget();
}
void UDasWidgetCallback::Deactivate()
{
    Active = false;
    if (auto* B = Button.Get())
        B->OnClicked.RemoveDynamic(this, &UDasWidgetCallback::Receive);
}
void UDasWidgetCallback::Receive()
{
    // ProcessEvent runs after the current VM call, with fresh owner/function checks.
    const TWeakObjectPtr<UDasWidgetCallback> Weak(this);
    FTSTicker::GetCoreTicker().AddTicker(
        FTickerDelegate::CreateLambda(
            [Weak](float)
            {
                auto* P = Weak.Get();
                if (!P || !P->Active)
                    return false;
                auto* O = P->Owner.Get();
                auto* W = P->Widget.Get();
                auto* B = P->Button.Get();
                auto* World = IsValid(O) ? O->GetWorld() : nullptr;
                if (!World || !World->IsGameWorld() || World->bIsTearingDown || !IsValid(W) || !W->IsInViewport() ||
                    !IsValid(B) || (Cast<AActor>(O) && CastChecked<AActor>(O)->IsActorBeingDestroyed()))
                    return false;
                auto* F = O->FindFunction(P->Handler);
                if (F && F->HasAnyFunctionFlags(FUNC_BlueprintCallable) && F->NumParms == 0 && !F->GetReturnProperty())
                    O->ProcessEvent(F, nullptr);
                return false;
            }),
        0.f);
}
