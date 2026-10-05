#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/UnrealType.h"
#include "DasLifecycleTestObserver.generated.h"

UCLASS(Transient)
class UDasLifecycleTestObserver : public UObject
{
    GENERATED_BODY()
public:
    int32 Calls = 0;
    int32 Health = 0;
    EEndPlayReason::Type Reason = EEndPlayReason::Quit;
    UFUNCTION()
    void OnEnded(AActor* Actor, EEndPlayReason::Type InReason)
    {
        ++Calls;
        Reason = InReason;
        if (auto* Property = FindFProperty<FIntProperty>(Actor->GetClass(), TEXT("health")))
            Health = Property->GetPropertyValue_InContainer(Actor);
    }
};

// A foreign UE listener can destroy an Actor synchronously during its script call.
UCLASS(Transient)
class UDasDelegateDestroyer : public UObject
{
    GENERATED_BODY()
public:
    TWeakObjectPtr<AActor> Target;
    int32 Calls = 0;
    UFUNCTION()
    void OnSignal(int32 Value)
    {
        ++Calls;
        if (AActor* Actor = Target.Get()) Actor->Destroy();
    }
};
