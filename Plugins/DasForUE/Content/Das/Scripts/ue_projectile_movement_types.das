options gen2
module ue_projectile_movement_types public
require ue_object_types public

// Canonical receiver declarations cannot depend on the generated API.
[ue_native_object(path="/Script/Engine.ProjectileMovementComponent")]
struct ProjectileMovementComponentRef { handle:UObjectHandle }

def ProjectileMovementComponentRef_from(value:UObjectHandle):ProjectileMovementComponentRef {
    return ProjectileMovementComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.ProjectileMovementComponent"))
}
