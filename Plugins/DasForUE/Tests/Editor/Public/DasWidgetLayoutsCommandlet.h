#pragma once
#include "Commandlets/Commandlet.h"
#include "GameFramework/PlayerController.h"
#include "DasWidgetLayoutsCommandlet.generated.h"
UCLASS()
class UDasWidgetLayoutsCommandlet : public UCommandlet
{
    GENERATED_BODY()
public:
    UDasWidgetLayoutsCommandlet();
    virtual int32 Main(const FString& Params) override;
};
class UUserWidget;
class SWidget;
UCLASS()
class ADasWidgetLayoutController : public APlayerController
{
    GENERATED_BODY()
public:
    TWeakObjectPtr<UUserWidget> HUD;
    TSharedPtr<SWidget> Slate;
    int32 Clicks = 0;
    UFUNCTION(BlueprintCallable)
    void ConstructHUD(UUserWidget* Widget);
    UFUNCTION(BlueprintCallable)
    void Clicked() { ++Clicks; }
};
