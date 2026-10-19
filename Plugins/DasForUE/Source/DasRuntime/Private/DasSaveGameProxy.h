#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DasSaveGameProxy.generated.h"
class USaveGame;
UCLASS(Transient)
class UDasSaveGameProxy : public UObject
{
    GENERATED_BODY()
public:
    uint64 SubscriptionId = 0;
    UPROPERTY()
    TObjectPtr<USaveGame> SaveObject;
};
