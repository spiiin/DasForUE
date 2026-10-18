#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/Widget.h"
#include "InputAction.h"
#include "DasInputUIProbe.generated.h"
UCLASS()
class ADasInputUIProbe:public AActor {
 GENERATED_BODY()
public:
 UPROPERTY(BlueprintReadOnly) TObjectPtr<UInputAction> Action;
 UPROPERTY(BlueprintReadOnly) TObjectPtr<UWidget> Widget;
 int32 NavigationCalls=0;
 UFUNCTION() UWidget* ChooseNavigation(EUINavigation Direction){++NavigationCalls;return Widget;}
 UFUNCTION() void WrongNavigation(){}
};
