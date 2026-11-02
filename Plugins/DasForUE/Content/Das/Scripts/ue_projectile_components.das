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

def ue_apply_point_damage(target:ActorRef; amount:float; direction:UEVector; hit:CollisionHit;
    source:ActorRef; instigator:PlayerControllerRef):float {
    let call=ue_begin_static_call("/Script/DasRuntime.DasCollisionLibrary", "ApplyPointDamage", "")
    ue_arg_object(call, "Target", target.handle)
    ue_arg_float(call, "Damage", amount)
    __ue_method_arg_vector(call, "Direction", direction)
    ue_arg(call, "Hit", hit)
    ue_arg_object(call, "Instigator", instigator.handle)
    ue_arg_object(call, "Source", source.handle)
    ue_invoke_call(call)
    let result=ue_result_float(call, "ReturnValue")
    ue_release_call(call)
    return result
}

def ue_report_noise(location:UEVector; loudness:float; instigator:NativePawnRef):void {
    let call=ue_begin_static_call("/Script/AIModule.AISense_Hearing", "ReportNoiseEvent", "WorldContextObject")
    __ue_method_arg_vector(call, "NoiseLocation", location)
    ue_arg_float(call, "Loudness", loudness)
    ue_arg_object(call, "Instigator", instigator.handle)
    ue_arg_float(call, "MaxRange", 0.0f)
    ue_arg_name(call, "Tag", "None")
    ue_invoke_call(call)
    ue_release_call(call)
}
