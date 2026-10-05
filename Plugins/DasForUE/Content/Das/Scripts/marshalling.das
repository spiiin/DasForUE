options gen2
require ue_foundation
[export]
def main(input:int):int {
    let call = ue_begin_call(ue_self_object(), "TransformPayload")
    ue_arg_float(call, "Payload.Metrics.Ratio", 1.5)
    ue_arg_int(call, "Payload.Count", 7)
    ue_arg_string(call, "Payload.Label", "Привет")
    ue_arg_bool(call, "Payload.bEnabled", false)
    ue_arg_double(call, "Payload.Weight", double(1.25))
    ue_arg_int(call, "Delta", input)
    ue_arg_bool(call, "bUpper", false)
    ue_arg_string(call, "Suffix", " 世界")
    ue_invoke_call(call)
    let resultText = ue_result_string(call, "ReturnValue.Label")
    verify(ue_result_bool(call, "ReturnValue.bEnabled"))
    verify(ue_result_double(call, "ReturnValue.Weight") == double(2.5))
    verify(ue_result_float(call, "ReturnValue.Metrics.Ratio") == 4.5)
    let result = ue_result_int(call, "ReturnValue.Count")
    ue_release_call(call)
    verify(resultText == "Привет 世界")
    return result
}
