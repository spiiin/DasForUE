#pragma once
#include "CoreMinimal.h"
enum class EDasExecutionRole : uint8
{
    Gameplay,
    PhysicsPreSim
};
DASRUNTIME_API EDasExecutionRole GetDasExecutionRole();
// Uses solver state in every build, not PHYSICS_THREAD_CONTEXT debug flags.
DASRUNTIME_API bool IsDasFrozenPhysicsWorld(const UObject* Object);
