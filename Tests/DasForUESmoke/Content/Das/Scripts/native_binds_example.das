options gen2
require ue_engine
require example_game
require math

// The script of the native binds example (Tests/DasForUESmoke/Source/DasForUESmoke/NativeBindsExample.cpp,
// Docs/Guide/Native/NativeBinds.md), run with an AExampleTarget at (300, 400, 0) as self. main returns 0 or the
// number of the first failed check.
[export]
def main(input:int):int {
    // [doc:script]
    let target = as_AExampleTarget(ue_self_object()) // null when self is not an AExampleTarget
    if (!target.valid()) { return 1 }
    target.apply_damage(damage_for(10, 1.5))         // 15
    target.heal(5)
    if (target.get_health() != 90 || !target.is_alive()) { return 2 }
    // Methods of the ancestor types: ue_engine's AActor and the one example_game adds to it.
    let location = target.get_actor_location()
    if (abs(target.distance_to_origin() - 500.0lf) > 0.001lf || location.x != 300.0lf) { return 3 }
    // A typed reference converts to its ancestors and back.
    let actor : AActor = as_AActor(target)
    if (as_AExampleTarget(actor) != target) { return 4 }
    // [/doc]
    return 0
}

[export]
def bad_heal(input:int):int {
    as_AExampleTarget(ue_self_object()).heal(-1)
    return 0
}

[export]
def null_target(input:int):int {
    var target : AExampleTarget
    return target.get_health()
}
