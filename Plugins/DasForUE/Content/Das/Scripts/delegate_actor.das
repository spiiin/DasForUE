options gen2
require ue_foundation

// Called for every candidate version; declarations are installed only after validation.
[export]
def bind_events() {
    ue_subscribe_int("peer", "OnDasSignal", "on_signal")
}

[export]
def on_signal(value:int) {
    ue_state_set_int("total", ue_state_get_int("total", 0) + value)
}

[export]
def main(input:int):int {
    return ue_state_get_int("total", 0)
}
