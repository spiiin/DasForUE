options gen2
require ue_foundation
require ue_time
require ue_math
require ue_engine

// AOT check of the DasNativeBindsManifest commandlet (DasNativeBinds.h). open_only calls native binds with an AOT
// ABI (scalars, strings, engine values, time values) and with_objects object methods and helpers: both are
// compiled for AOT and call them through the guarded bridge. with_closed calls an extern of the isolated physics
// VM (interpreted only): it stays in the interpreter.
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

// Reference parameters are compiled too: a transform setter and output parameters.
[export]
def with_refs(input:int):int {
    var t = ue_transform_identity()
    t.set_location(ue_vector(1.0lf, 2.0lf, 3.0lf))
    var axis = zero_vector()
    var angle = 0.0lf
    make_from_euler(zero_vector()).to_axis_and_angle(axis, angle)
    var direction = zero_vector()
    var length = 0.0lf
    forward_vector().to_direction_and_length(direction, length)
    return int(length + angle) + input
}

// ue_foundation's math externs and operators are compiled.
[export]
def with_math(input:int):int {
    let v = ue_vector(1.0lf, 2.0lf, 2.0lf) + ue_vector(0.0lf, 0.0lf, 0.0lf)
    return int(ue_length(v * 2.0lf)) + input
}

// A class argument is compiled (a null handle here: class literals come from ue_foundation externs, which are not).
[export]
def with_classes(input:int):int {
    var no_class : UEClassHandle
    var none : USceneComponent
    return none.valid() && none.get_child_component_by_class(no_class).valid() ? 1 : input
}

// An FName parameter and == of a class type are compiled.
[export]
def with_names(input:int):int {
    let a = as_AActor(ue_self_object())
    return a.valid() && a == a && a.actor_has_tag(ue_name_value("tag")) ? 1 : input
}

// An array result is compiled too (array<UActorComponent>).
[export]
def with_arrays(input:int):int {
    var none : AActor
    return none.valid() ? length(none.get_all_components()) : input
}

// A function that reaches a struct snapshot collection through a callee (called inside an expression, which
// daScript's AOT proof would reject): the generator keeps it interpreted instead of failing.
def make_point():UEStructValue {
    return ue_struct_default("/Script/CoreUObject.IntPoint", "(X=1,Y=2)")
}
[export]
def with_carrier(input:int):int {
    if (input < 0) {
        return ue_struct_get_int(make_point(), "X")
    }
    return input
}

[export]
def with_closed(input:int):int {
    if (input < 0) {
        return int(ue_physics_input(0))
    }
    return input
}
