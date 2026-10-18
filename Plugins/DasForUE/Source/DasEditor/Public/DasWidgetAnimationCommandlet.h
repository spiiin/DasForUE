#pragma once
#include "Commandlets/Commandlet.h"
#include "DasScriptUserWidget.h"
#include "DasWidgetAnimationCommandlet.generated.h"
UCLASS()
class UDasWidgetAnimationCommandlet:public UCommandlet {
 GENERATED_BODY()
public:
 UDasWidgetAnimationCommandlet();virtual int32 Main(const FString& Params)override;
};

// Test-only access to the real engine's protected animation tick and state.
UCLASS()
class UDasWidgetAnimationFixture:public UDasScriptUserWidget {
 GENERATED_BODY()
public:
 using UUserWidget::GetAnimationState;
 using UUserWidget::TickActionsAndAnimation;
 UFUNCTION(BlueprintCallable)
 void ReconstructForAnimationTest(){NativeDestruct();NativeConstruct();}
};
