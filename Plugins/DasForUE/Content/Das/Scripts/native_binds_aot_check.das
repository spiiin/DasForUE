options gen2
require ue_foundation
require ue_time
require ue_math
require ue_engine

// AOT check of the DasNativeBindsManifest commandlet (DasNativeBinds.h). open_only calls native binds with an AOT
// ABI (scalars, strings, engine values, time values): it is compiled for AOT and calls them through the guarded
// bridge. with_closed calls an object method, which has no ABI yet: it stays in the interpreter.
[export]
def open_only(input:int):int {
    let d = make_date_time(2024, 2, 29)
    // Only native binds here: ue_foundation's own vector operators have no AOT ABI and would keep this function
    // in the interpreter.
    let v = forward_vector().rotate_angle_axis(90.0lf, up_vector())
    let text = d.to_iso8601()
    let scalars = days_in_year(2023) + int(f_interp_to(0.0lf, 10.0lf, 0.1lf, 5.0lf))
    let values = d.get_day() + int(v.size_2d()) + length(text)
    return scalars + values + input
}

[export]
def with_closed(input:int):int {
    return int(as_Actor(ue_self_object()).get_actor_location().x) + input
}
