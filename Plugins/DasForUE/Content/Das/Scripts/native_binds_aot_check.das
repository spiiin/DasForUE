options gen2
require ue_foundation
require ue_time
require ue_math
require ue_engine

// AOT check of the DasNativeBindsManifest commandlet (DasNativeBinds.h). open_only calls native binds with an AOT
// ABI (scalars, strings, engine values, time values) and with_objects object methods and helpers: both are
// compiled for AOT and call them through the guarded bridge. with_closed calls a bind with an FName parameter,
// which has no ABI yet: it stays in the interpreter.
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

// From self to ue_engine methods: ue_self_object and the binds' helpers are all compiled.
[export]
def with_objects(input:int):int {
    let a = as_AActor(ue_self_object())
    return a.valid() ? int(a.get_actor_location().size_2d()) + input : input
}

// Reference parameters are compiled too (values made by ue_math: ue_foundation's math externs are not).
[export]
def with_refs(input:int):int {
    var axis = zero_vector()
    var angle = 0.0lf
    make_from_euler(zero_vector()).to_axis_and_angle(axis, angle)
    var direction = zero_vector()
    var length = 0.0lf
    forward_vector().to_direction_and_length(direction, length)
    return int(length + angle) + input
}

[export]
def with_closed(input:int):int {
    return as_AActor(ue_self_object()).actor_has_tag(ue_name_value("tag")) ? 1 : input
}
