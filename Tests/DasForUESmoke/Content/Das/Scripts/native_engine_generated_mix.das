options gen2
require math
require ue_foundation
require ue_engine
require ue_generated

// ue_engine and the generated bindings together: the native type is AActor (named as in C++), the generated
// wrapper keeps its full name (no short alias for Actor, a DSL base class: Scripts/GenerateBindings.py); the two
// convert through UObjectHandle. Self is an actor.
[export]
def main(input:int):int {
    let native = as_AActor(ue_self_object())
    let generated = C_Actor_adf65d5b_from(native.handle())
    var typed : AActor = as_AActor(C_Actor_adf65d5b_raw(generated))
    let qualified : ue_engine::AActor = typed
    if (!typed.valid() || typed != native || qualified != native) { return 1 }
    let location = actor_K2_GetActorLocation(generated)
    if (abs(location.f_x - native.get_actor_location().x) > 0.001lf) { return 2 }
    return 0
}
