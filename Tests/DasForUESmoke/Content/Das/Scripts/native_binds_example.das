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

// [doc:called-from-cpp]
// Called from C++ through TDasFunction: exported, with the C++ signature's script types.
[export]
def enemy_count(wave:int; difficulty:float):int {
    return int(float(4 + wave * 2) * difficulty)
}
// An object arrives as UObjectHandle; the script calls the target's own bind.
[export]
def on_target_hit(target:UObjectHandle; damage:int):void {
    let hit = as_AExampleTarget(target)
    if (hit.valid()) {
        hit.apply_damage(damage)
    }
}
// [/doc]
