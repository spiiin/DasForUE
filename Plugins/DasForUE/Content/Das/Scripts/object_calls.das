options gen2
require ue_foundation

// Any loaded object reachable through Blueprint-visible strong object properties.
[export]
def main(input:int):int {
    let self = ue_self_object()
    let target = ue_get_object(self, "Target")
    verify(ue_object_valid(target))
    let typed = ue_cast_object(target, "/Script/DasRuntime.DasObjectFixture")
    ue_call_void_int(typed, "SetHealthValue", 100)
    return ue_call_int(typed, "AddHealth", input)
}
