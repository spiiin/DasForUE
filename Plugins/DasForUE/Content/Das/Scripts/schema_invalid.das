options gen2
require ue_schema_probe
[ue_probe]
struct UnsupportedActor {
    displayName : string
}
[export]
def main(input : int) : int {
    return input
}
