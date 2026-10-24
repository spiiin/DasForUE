#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NativeBindsExampleObjects.generated.h"

// The game class of the native binds example (NativeBindsExample.cpp, Docs/Guide/Native/NativeBinds.md): plain C++
// methods, no UFUNCTIONs needed for a native bind.
UCLASS()
class AExampleTarget : public AActor
{
    GENERATED_BODY()
public:
    int32 Health = 100;

    int32 GetHealth() const { return Health; }
    bool IsAlive() const { return Health > 0; }
    void ApplyDamage(int32 Amount) { Health = FMath::Max(0, Health - Amount); }
};
