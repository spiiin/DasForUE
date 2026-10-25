options gen2
options ue_native_routes = true
require ue_native_gameplay
require ue_scene_components
require smoke_routes
require math

// Gameplay calls of UFUNCTIONs that ue_engine binds reflect (DasNativeBinds.h, Reflects), run by
// DasNativeRoutesProbe.cpp with the probe actor as self. native_routes_probe.das calls the binds,
// native_routes_reflected.das is the same file with `ue_native_routes = false` (reflection); keep them equal line
// for line, the probe compares their errors. Each entry makes one gameplay method call and returns 0 when its
// result is right, or a value the probe compares with C++; the probe counts the calls the binds answered.
def near(a, b : UEVector) : bool {
    return ue_nearly_equal(a, b, 0.001lf)
}
def self_actor() : ActorRef {
    return ue_ref(type<ActorRef>, ue_self_object())
}

[export]
def actor_location(input:int):int {
    let a = self_actor()
    return a.K2_GetActorLocation() == ue_vector(100.0lf, 200.0lf, 300.0lf) ? 0 : 1
}
[export]
def actor_rotation(input:int):int {
    let a = self_actor()
    return abs(a.K2_GetActorRotation().yaw - 90.0lf) < 0.001lf ? 0 : 1
}
[export]
def actor_forward(input:int):int {
    let a = self_actor()
    return near(a.GetActorForwardVector(), ue_vector(0.0lf, 1.0lf, 0.0lf)) ? 0 : 1
}
[export]
def actor_transform(input:int):int {
    let a = self_actor()
    return near(ue_transform_translation(a.GetTransform()), ue_vector(100.0lf, 200.0lf, 300.0lf)) ? 0 : 1
}
[export]
def set_actor_scale(input:int):int {
    let a = self_actor()
    a.SetActorScale3D(ue_vector(2.0lf, 2.0lf, 2.0lf))
    return 0
}
[export]
def actor_has_tag(input:int):int {
    let a = self_actor()
    return a.ActorHasTag(ue_name_value("probe_tag")) ? 0 : 1
}
[export]
def actor_life_span(input:int):int {
    let a = self_actor()
    return a.GetLifeSpan() == 0.0 ? 0 : 1
}
[export]
def set_actor_hidden(input:int):int {
    let a = self_actor()
    a.SetActorHiddenInGame(true)
    return 0
}
[export]
def component_location(input:int):int {
    let a = self_actor()
    let c = a.RootComponent
    return c.K2_GetComponentLocation() == ue_vector(100.0lf, 200.0lf, 300.0lf) ? 0 : 1
}
// Pawn: the Actor bind answers a UFUNCTION of an ancestor class.
[export]
def pawn_location(input:int):int {
    let p = ue_ref(type<NativePawnRef>, smoke_routes_pawn())
    return p.K2_GetActorLocation() == ue_vector(0.0lf, 0.0lf, 500.0lf) ? 0 : 1
}
[export]
def pawn_locally_controlled(input:int):int {
    let p = ue_ref(type<NativePawnRef>, smoke_routes_pawn())
    return p.IsLocallyControlled() ? 1 : 0
}
// Kept on reflection by policy: unsafe in construction scripts, requires authority.
[export]
def component_is_active(input:int):int {
    let a = self_actor()
    let c = a.RootComponent
    return c.IsActive() ? 1 : 0
}
[export]
def set_replicates(input:int):int {
    let a = self_actor()
    a.SetReplicates(false)
    return 0
}
// The receiver checks of the reflected call: a destroyed actor is refused with the same error.
[export]
def destroyed_receiver(input:int):int {
    let v = ue_ref(type<ActorRef>, smoke_routes_victim())
    smoke_routes_destroy_victim()
    return v.K2_GetActorLocation() == ue_vector(0.0lf, 0.0lf, 0.0lf) ? 0 : 1
}
// Object parameters and results: native reference types cross the route as their handles.
def other_actor() : ActorRef {
    return ue_ref(type<ActorRef>, smoke_routes_other())
}
[export]
def owner_round_trip(input:int):int {
    let a = self_actor()
    a.SetOwner(other_actor())
    return ue_same_object(a.GetOwner(), other_actor()) ? 0 : 1
}
[export]
def owner_cleared(input:int):int {
    let a = self_actor()
    var none : ActorRef
    a.SetOwner(none)
    return ue_is_valid(a.GetOwner()) ? 1 : 0
}
[export]
def root_component(input:int):int {
    let a = self_actor()
    return ue_same_object(a.K2_GetRootComponent(), a.RootComponent) ? 0 : 1
}
[export]
def attach_parent(input:int):int {
    let c = ue_ref(type<SceneComponentRef>, smoke_routes_child())
    return ue_same_object(c.GetAttachParent(), self_actor().RootComponent) ? 0 : 1
}
[export]
def component_owner(input:int):int {
    let a = self_actor()
    let c = a.RootComponent
    return ue_same_object(c.GetOwner(), a) ? 0 : 1
}
[export]
def pawn_controller(input:int):int {
    let p = ue_ref(type<NativePawnRef>, smoke_routes_pawn())
    return ue_is_valid(p.GetController()) ? 1 : 0
}
// Output parameters (the bind's T&): the variables get the bounds and the eyes' view point.
[export]
def actor_bounds(input:int):int {
    let a = self_actor()
    var origin = ue_vector(7.0lf, 7.0lf, 7.0lf)
    var extent = ue_vector(7.0lf, 7.0lf, 7.0lf)
    a.GetActorBounds(false, origin, extent)
    return int(origin.x + origin.y + origin.z + extent.x + extent.y + extent.z)
}
[export]
def actor_eyes(input:int):int {
    let a = self_actor()
    var location = ue_vector(7.0lf, 7.0lf, 7.0lf)
    var rotation = ue_rotator(7.0lf, 7.0lf, 7.0lf)
    a.GetActorEyesViewPoint(location, rotation)
    return near(location, ue_vector(100.0lf, 200.0lf, 300.0lf)) && abs(rotation.yaw - 90.0lf) < 0.001lf ? 0 : 1
}
// Enums (the bind takes and returns their int32 values): a native enum argument and result.
[export]
def component_mobility(input:int):int {
    let c = ue_ref(type<SceneComponentRef>, smoke_routes_child())
    c.SetMobility(EComponentMobility.Stationary)
    return 0
}
[export]
def local_role(input:int):int {
    return int(self_actor().GetLocalRole())
}
// An expired object argument: both paths refuse it with the same message.
[export]
def destroyed_argument(input:int):int {
    let a = self_actor()
    let v = ue_ref(type<ActorRef>, smoke_routes_victim())
    smoke_routes_destroy_victim()
    a.SetOwner(v)
    return 0
}
[export]
def bench(count:int):int {
    let a = self_actor()
    var n = 0
    for (i in range(0, count)) {
        if (a.K2_GetActorLocation().x > -1.0e30lf) { n += 1 }
    }
    return n
}
// An object result: the owner is null, each call returns an invalid reference.
[export]
def bench_owner(count:int):int {
    let a = self_actor()
    var n = 0
    for (i in range(0, count)) {
        if (!ue_is_valid(a.GetOwner())) { n += 1 }
    }
    return n
}
