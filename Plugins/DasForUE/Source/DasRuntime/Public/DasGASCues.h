#pragma once
#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "DasGASCues.generated.h"
// Native routing is present in Shipping too; no debug-manager delegate is used.
UCLASS(BlueprintType, Blueprintable)
class DASRUNTIME_API UDasGameplayCueNotify : public UGameplayCueNotify_Static
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FString DefinitionId;
    UPROPERTY()
    FString DefinitionHash;
    UDasGameplayCueNotify(const FObjectInitializer& ObjectInitializer);
    virtual void HandleGameplayCue(AActor* Target, EGameplayCueEvent::Type Event,
                                   const FGameplayCueParameters& Parameters) override;
};
DASRUNTIME_API bool RegisterDasGASCueAssets(const TArray<TSubclassOf<UDasGameplayCueNotify>>& Classes);
