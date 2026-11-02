options gen2
module ue_projectile_components public
require ue_native_gameplay public
[ue_native_object(path="/Script/Engine.ProjectileMovementComponent")]
struct ProjectileMovementComponentRef { handle:UObjectHandle }
[ue_native_object(path="/Script/Niagara.NiagaraComponent")]
struct NiagaraComponentRef { handle:UObjectHandle }
[ue_native_object(path="/Script/Engine.AudioComponent")]
struct AudioComponentRef { handle:UObjectHandle }

def ue_projectile_movement_launch(movement:ProjectileMovementComponentRef; updated:SceneComponentRef; localVelocity:UEVector; gravityScale:float):void {
    __ue_projectile_movement_launch(movement.handle,updated.handle,localVelocity,gravityScale)
}

// A detached event payload. Strong references are pinned by the callback queue.
[ue_native_struct(path="/Script/DasRuntime.DasCollisionHit")]
struct CollisionHit {
    BlockingHit:bool
    StartPenetrating:bool
    Time:float
    Distance:float
    PenetrationDepth:float
    Location:UEVector
    ImpactPoint:UEVector
    Normal:UEVector
    ImpactNormal:UEVector
    TraceStart:UEVector
    TraceEnd:UEVector
    FaceIndex:int
    Item:int
    MyItem:int
    ElementIndex:uint8
    BoneName:UENameValue
    MyBoneName:UENameValue
    Actor:ActorRef
    Component:ObjectRef
    PhysMaterial:ObjectRef
}
