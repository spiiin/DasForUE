options gen2
require ue_foundation
var @ue_state begins:int = 0
var @ue_state ticks:int = 0

[export]
def begin_play() {
    begins = (begins + 1)
    ue_set_self_int("health", ue_get_self_int("health") + 1)
}

[export]
def tick(delta_time:float) {
    ticks = (ticks + 1)
    ue_set_self_float("speed", ue_get_self_float("speed") + delta_time * 1.0)
}

[export]
def end_play(reason:int) {
    ue_set_self_int("health", ue_get_self_int("health") + 100 + reason)
}

// Optional main is a diagnostic entry point, not needed for lifecycle dispatch.
[export]
def main(input:int):int {
    if (input == 0) { return begins }
    return ticks
}
