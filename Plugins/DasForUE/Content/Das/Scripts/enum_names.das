options gen2
require ue_foundation
[export]
def main(input:int):int {
let c = ue_begin_call(ue_self_object(), "ExchangeMode")
ue_arg_enum(c, "Input", "/Script/DasRuntime.EDasCallMode", "Active")
ue_arg_enum(c, "Current", "/Script/DasRuntime.EDasCallMode", "Paused")
ue_invoke_call(c)
verify(ue_result_enum(c, "Previous", "/Script/DasRuntime.EDasCallMode") == "Paused")
verify(ue_result_enum(c, "Current", "/Script/DasRuntime.EDasCallMode") == "Active")
let mode = ue_result_enum(c, "ReturnValue", "/Script/DasRuntime.EDasCallMode")
ue_release_call(c)
verify(mode == "Active")
let n = ue_begin_call(ue_self_object(), "ExchangeName")
ue_arg_name(n, "Input", "Пример_17")
ue_arg_name(n, "Current", "")
ue_invoke_call(n)
verify(ue_result_name(n, "Previous") == "None")
verify(ue_result_name(n, "Current") == "Пример_17")
let key = ue_result_name(n, "ReturnValue")
ue_release_call(n)
verify(key == "Пример_17")
return 1
}
