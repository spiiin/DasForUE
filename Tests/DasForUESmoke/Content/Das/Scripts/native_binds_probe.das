options gen2
require smoke_native

// Native binds from the smoke game module (Tests/DasForUESmoke/.../DasNativeBindsProbe.cpp). main returns 0 when
// every value survives the round trip, otherwise the number of the first failed check.
[export]
def main(input:int):int {
    if (!smoke_echo_bool(true) || smoke_echo_bool(false)) { return 1 }
    if (smoke_echo_int(-7) != -7 || smoke_echo_int(2147483647) != 2147483647) { return 2 }
    if (smoke_echo_float(1.5f) != 1.5f) { return 3 }
    if (smoke_echo_double(0.1lf) != 0.1lf) { return 4 }
    if (smoke_echo_byte(uint8(250)) != uint8(250)) { return 5 }
    if (smoke_echo_string("Привет, мир") != "Привет, мир" || smoke_echo_string("") != "") { return 6 }
    if (smoke_name_string(smoke_echo_name(ue_name_value("Hero_7"))) != "Hero_7") { return 7 }
    if (smoke_text_string(smoke_echo_text(ue_text_invariant("Текст"))) != "Текст") { return 8 }
    if (smoke_echo_vector(ue_vector(1.0lf, -2.0lf, 3.5lf)) != ue_vector(1.0lf, -2.0lf, 3.5lf)) { return 9 }
    if (smoke_echo_vector2d(ue_vector2d(4.0lf, 5.0lf)) != ue_vector2d(4.0lf, 5.0lf)) { return 10 }
    if (smoke_echo_rotator(ue_rotator(10.0lf, 20.0lf, 30.0lf)) != ue_rotator(10.0lf, 20.0lf, 30.0lf)) { return 11 }
    if (!smoke_quat_is(smoke_echo_quat(ue_quat(0.0lf, 0.0lf, 0.6lf, 0.8lf)), 0.0lf, 0.0lf, 0.6lf, 0.8lf)) { return 12 }
    let location = ue_vector(1.0lf, 2.0lf, 3.0lf)
    if (!smoke_transform_is(smoke_echo_transform(smoke_make_transform(location, 2.0lf)), location, 2.0lf)) { return 13 }
    let linear = ue_linear_color(0.25f, 0.5f, 0.75f, 1.0f)
    if (smoke_echo_linear_color(linear) != linear) { return 14 }
    if (smoke_echo_color(ue_color(1, 2, 3, 4)) != ue_color(1, 2, 3, 4)) { return 15 }
    let text = smoke_describe(3, 1.5f, "ok", ue_vector(1.0lf, 2.0lf, 3.0lf), ue_name_value("Tag"))
    if (text != "3|1.50|ok|1.0,2.0,3.0|Tag") { return 16 }
    let before = smoke_void_calls()
    smoke_count_call()
    smoke_count_call()
    if (smoke_void_calls() != before + 2) { return 17 }
    if (smoke_square(12) != 144) { return 18 }
    return 0
}

// The call cost benchmark (interop-performance): count native calls inside the script.
[export]
def bench(count:int):int {
    var sum = 0
    for (i in range(0, count)) { sum += smoke_add(i, 1) }
    return sum
}
