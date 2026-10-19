#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasEffectProxy.generated.h"
class UAudioComponent;
class UNiagaraComponent;
UCLASS(Transient)
class DASRUNTIME_API UDasEffectProxy : public UObject
{
    GENERATED_BODY()
public:
    uint64 SubscriptionId = 0;
    bool Active = true, Starting = true, AudioDone = true, NiagaraDone = true, Queued = false;
    TWeakObjectPtr<UObject> Owner;
    UPROPERTY()
    TObjectPtr<UAudioComponent> Audio;
    UPROPERTY()
    TObjectPtr<UNiagaraComponent> Niagara;
    bool IsLive() const;
    void Deactivate();
    void CompleteIfReady();
    virtual void BeginDestroy() override;
    UFUNCTION()
    void AudioFinished();
    UFUNCTION()
    void NiagaraFinished(UNiagaraComponent* Component);
};
