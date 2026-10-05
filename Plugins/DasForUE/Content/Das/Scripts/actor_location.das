options gen2
require ue_foundation
[export]
def main(input:int):int {
    let call = ue_begin_call(ue_self_object(), "K2_GetActorLocation")
    ue_invoke_call(call)
    let x = ue_result_double(call, "ReturnValue.X")
    ue_release_call(call)
    return int(x)
}
