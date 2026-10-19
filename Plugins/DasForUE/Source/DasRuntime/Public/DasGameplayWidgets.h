#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DasGameplayWidgets.generated.h"
class UButton;

// Reusable native template for the example; callers may create Widget Blueprints instead.
UCLASS()
class DASRUNTIME_API UDasGameplayHUD : public UUserWidget
{
    GENERATED_BODY()
protected:
    virtual TSharedRef<SWidget> RebuildWidget() override;
};

UCLASS(Transient)
class DASRUNTIME_API UDasWidgetCallback : public UObject
{
    GENERATED_BODY()
public:
    TWeakObjectPtr<UObject> Owner;
    TWeakObjectPtr<UUserWidget> Widget;
    TWeakObjectPtr<UButton> Button;
    FName Handler;
    bool Active = true;
    void Deactivate();
    UFUNCTION()
    void Receive();
};
