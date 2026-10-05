options gen2
require ue_foundation

[export]
def begin_play() {
    ue_state_set_int("begins", ue_state_get_int("begins", 0) + 1)
    ue_set_self_int("health", ue_get_self_int("health") + 1)
}

[export]
def tick(delta_time:float) {
    ue_state_set_int("ticks", ue_state_get_int("ticks", 0) + 1)
    ue_set_self_float("speed", ue_get_self_float("speed") + delta_time * 1.0)
}

[export]
def end_play(reason:int) {
    ue_set_self_int("health", ue_get_self_int("health") + 100 + reason)
}

// Optional main is a diagnostic entry point, not needed for lifecycle dispatch.
[export]
def main(input:int):int {
    if (input == 0) { return ue_state_get_int("begins", 0) }
    return ue_state_get_int("ticks", 0)
}
