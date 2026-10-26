#pragma once
#include "GameFramework/GameModeBase.h"
#include "CleanGameplayMode.generated.h"
class ADasActor;
UCLASS()
class ACleanGameplayMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ACleanGameplayMode();
    virtual void BeginPlay() override;
    virtual void Tick(float Delta) override;

private:
    UPROPERTY()
    TObjectPtr<ADasActor> Collectible;
    bool Verify = false;
    int32 Collections = 0;
    double Started = 0;
    bool ScreenshotRequested = false;
    void Finish(bool Good);
};
