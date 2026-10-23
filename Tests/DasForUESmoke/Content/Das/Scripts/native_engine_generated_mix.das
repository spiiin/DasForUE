options gen2
require math
require ue_foundation
require ue_engine
require ue_generated

// ue_engine and the generated bindings together. The generator emits no short alias for a name of a native type
// (Scripts/GenerateBindings.py, native_type_names), so `Actor` is ue_engine::Actor, unqualified, and the generated
// wrapper keeps its full name; the two convert through UObjectHandle. Self is an actor.
[export]
def main(input:int):int {
    let native = as_Actor(ue_self_object())
    let generated = C_Actor_adf65d5b_from(native.handle())
    var typed : Actor = as_Actor(C_Actor_adf65d5b_raw(generated))
    let qualified : ue_engine::Actor = typed
    if (!typed.valid() || typed != native || qualified != native) { return 1 }
    let location = actor_K2_GetActorLocation(generated)
    if (abs(location.f_x - native.get_actor_location().x) > 0.001lf) { return 2 }
    return 0
}
