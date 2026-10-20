#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/HUD.h"
#include "DasGASPlaygroundGame.generated.h"

// Cosmetic presentation for the example. Ability and input behavior live in DAS.
UCLASS()
class DASRUNTIME_API ADasGASPlaygroundHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
};
UCLASS()
class DASRUNTIME_API ADasGASPlaygroundGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ADasGASPlaygroundGameMode();
};
