options gen2
require ue_foundation
[export]
def main(input:int):int {
let c = ue_begin_call(ue_self_object(), "MutateValues")
ue_arg_int(c, "Count", 7)
ue_arg_string(c, "Text", "Привет")
ue_invoke_call(c)
verify(ue_result_int(c, "Count") == 10)
verify(ue_result_int(c, "ReturnValue") == 20)
verify(ue_result_bool(c, "Accepted"))
let description = ue_result_string(c, "Description")
verify(ue_result_string(c, "Text") == description)
ue_release_call(c)
verify(description == "Привет 世界")
return 1
}
