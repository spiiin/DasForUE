options gen2
require ue_foundation
require ue_generated

// The reflected counterpart of bench_actor_location in native_engine_probe.das: the generated binding of
// K2_GetActorLocation, called through reflection (interop-performance, script_reflected_actor_location).
[export]
def bench_actor_location(count:int):int {
    let a = C_Actor_adf65d5b_from(ue_self_object())
    var n = 0
    for (i in range(0, count)) {
        if (actor_K2_GetActorLocation(a).f_x > -1.0e30lf) { n += 1 }
    }
    return n
}
