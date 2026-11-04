options gen2
module ue_float_curves public
require ue_object_types public

typedef CurveFloatRef = UENativeObjectRef("/Script/Engine.CurveFloat")

def ue_curve_float_value(curve : CurveFloatRef; time : float) : float {
    let call = ue_begin_call(curve.handle, "GetFloatValue")
    ue_arg_float(call, "InTime", time)
    ue_invoke_call(call)
    let value = ue_result_float(call, "ReturnValue")
    ue_release_call(call)
    return value
}
