options gen2
module ue_random public
require ue_foundation public

let UE_RANDOM_MAX_RANGE = int64(0x7fffffff)

// A seed is plain script data; each call advances the exact native FRandomStream.
def ue_random_int(var seed : int&; minimum : int; maximum : int) : int {
    verify(maximum >= minimum && int64(maximum)-int64(minimum)+1 <= UE_RANDOM_MAX_RANGE, "Invalid random integer range")
    let call = ue_begin_static_call("/Script/DasRuntime.DasRandomLibrary", "RandomIntegerInRange", "")
    ue_arg_int(call, "Seed", seed)
    ue_arg_int(call, "Minimum", minimum)
    ue_arg_int(call, "Maximum", maximum)
    ue_invoke_call(call)
    let value = ue_result_int(call, "ReturnValue")
    seed = ue_result_int(call, "Seed")
    ue_release_call(call)
    return value
}


def ue_random_float(var seed : int&; minimum : float; maximum : float) : float {
    verify(minimum <= maximum, "Invalid random float range")
    let call = ue_begin_static_call("/Script/DasRuntime.DasRandomLibrary", "RandomFloatInRange", "")
    ue_arg_int(call, "Seed", seed)
    ue_arg_float(call, "Minimum", minimum)
    ue_arg_float(call, "Maximum", maximum)
    ue_invoke_call(call)
    let value = ue_result_float(call, "ReturnValue")
    seed = ue_result_int(call, "Seed")
    ue_release_call(call)
    return value
}
