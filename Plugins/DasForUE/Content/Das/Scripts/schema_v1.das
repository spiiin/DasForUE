options gen2
require ue_schema_probe
[ue_probe]
struct ProbeActor {
    speed : float
}
[export]
def main(input : int) : int {
    return input + emit_ProbeActor()
}
