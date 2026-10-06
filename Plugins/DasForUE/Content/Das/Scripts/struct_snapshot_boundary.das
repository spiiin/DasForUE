options gen2
require ue_foundation
var anchor = ue_struct_value("/Script/DasRuntime.DasCallPayload")
[export]
def main(input:int):int {
    let value = ue_struct_with_int(anchor,"Count",input)
    return ue_struct_get_int(value,"Count")
}
