options gen2
require ue_foundation
[export]
def main(input:int):int {
    let c = ue_begin_call(ue_self_object(), "OversizedMap")
    var failed = false
    try { ue_invoke_call(c) } recover { failed = true }
    verify(failed)
    var blocked = false
    try { ue_result_hash_size(c, "ReturnValue") } recover { blocked = true }
    verify(blocked)
    ue_release_call(c)
    let next = ue_begin_call(ue_self_object(), "MakeMap")
    ue_invoke_call(next)
    verify(ue_result_hash_size(next, "Values") == 1)
    ue_release_call(next)
    return 1
}
