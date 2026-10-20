#pragma once
#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "DasGASCues.generated.h"
// Native routing is present in Shipping too; no debug-manager delegate is used.
UCLASS()
class DASRUNTIME_API UDasGameplayCueNotify : public UGameplayCueNotify_Static
{
    GENERATED_BODY()
public:
    UDasGameplayCueNotify(const FObjectInitializer& ObjectInitializer);
    virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type Event,
                                   const FGameplayCueParameters& Parameters) override;
};
