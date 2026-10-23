options gen2
require math
require ue_foundation
require ue_engine
require ue_generated

// ue_engine and the generated bindings both name a type Actor: a script that requires both qualifies it
// (ue_engine::Actor) and converts between the two references through UObjectHandle. Self is an actor.
[export]
def main(input:int):int {
    let native = as_Actor(ue_self_object())
    let generated = C_Actor_adf65d5b_from(native.handle())
    var typed : ue_engine::Actor = as_Actor(C_Actor_adf65d5b_raw(generated))
    if (!typed.valid() || typed != native) { return 1 }
    let location = actor_K2_GetActorLocation(generated)
    if (abs(location.f_x - native.get_actor_location().x) > 0.001lf) { return 2 }
    return 0
}
