#include "DasExecutionRolePrivate.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "PBDRigidsSolver.h"
namespace
{
thread_local EDasExecutionRole Role = EDasExecutionRole::Gameplay;
}
EDasExecutionRole GetDasExecutionRole()
{
    return Role;
}
FDasPhysicsExecutionScope::FDasPhysicsExecutionScope() : Previous(Role)
{
    Role = EDasExecutionRole::PhysicsPreSim;
}
FDasPhysicsExecutionScope::~FDasPhysicsExecutionScope()
{
    Role = Previous;
}
bool IsDasFrozenPhysicsWorld(const UObject* Object)
{
    if (!IsInGameThread())
        return false;
    auto Frozen = [](const UWorld* World)
    {
        const auto* Scene = World ? World->GetPhysicsScene() : nullptr;
        const auto* Solver = Scene ? Scene->GetSolver() : nullptr;
        return Solver && Solver->IsGameThreadFrozen();
    };
    if (IsValid(Object) && Frozen(Object->GetWorld()))
        return true;
    // An authored/static callback may select another owner or have no self.
    // The execution phase belongs to the GT stack, not to that selected object.
    if (GEngine)
        for (const auto& Context : GEngine->GetWorldContexts())
            if (Frozen(Context.World()))
                return true;
    return false;
}