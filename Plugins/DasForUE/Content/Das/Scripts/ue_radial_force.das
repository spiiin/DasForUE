options gen2
module ue_radial_force public
require ue_scene_components public

typedef RadialForceComponentRef = UENativeObjectRef("/Script/Engine.RadialForceComponent")

typedef CollisionLibrary = UENativeObjectRef("/Script/DasRuntime.DasCollisionLibrary")

// Native RadialForce exposes object-query enums; callers use collision channels.
def ue_radial_force_remove_channel(force : RadialForceComponentRef; channel : auto) : bool {
    let call = ue_begin_static_call("/Script/DasRuntime.DasCollisionLibrary", "RemoveRadialForceCollisionChannel", "")
    ue_arg(call, "Force", force)
    __ue_method_arg_enum(call, "Channel", ue_enum_from_number("/Script/Engine.ECollisionChannel", int(channel)))
    ue_invoke_call(call)
    let removed = ue_result_bool(call, "ReturnValue")
    ue_release_call(call)
    return removed
}
