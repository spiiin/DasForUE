options gen2
require ue_foundation
[export]
def main(input : int) : int {
    let speed = ue_get_self_float("speed")
    assert(speed > 0.0)
    ue_set_self_float("speed", speed + 1.0)
    let health = ue_get_self_int("health") + input
    ue_set_self_int("health", health)
    return ue_get_self_int("health")
}
