options gen2
require ue_foundation
require ue_api_math
require ue_generated

// Call-cost benchmark (DasInteropProbe.inl). Each entry runs `count` calls of one kind inside the script, so the
// C++ side pays one dispatch per sample. Self is a UDasObjectFixture with health == 7.
[export]
def scalar(value:int):int { return value+1 }
[export]
def nothing() {}

[export]
def script_loop(count:int):int {
    var sum=0
    for (i in range(0,count)) { sum+=i & 7 }
    return sum
}
def leaf(value:int):int { return value+1 }
[export]
def script_to_script_call(count:int):int {
    var sum=0
    for (i in range(0,count)) { sum+=leaf(i) }
    return sum
}
[export]
def script_to_extern(count:int):int {
    var sum=0
    for (i in range(0,count)) { sum+=ue_add(i,1) }
    return sum
}
[export]
def script_call_int_reflected(count:int):int {
    let target=ue_self_object()
    var sum=0
    for (i in range(0,count)) { sum+=ue_call_int(target,"AddHealth",0) }
    return sum
}
[export]
def script_generated_object_call(count:int):int {
    let target=C_DasObjectFixture_5f019a81_from(ue_self_object())
    var sum=0
    for (i in range(0,count)) { sum+=fixture_AddHealth(target,0) }
    return sum
}
[export]
def script_generated_static_int(count:int):int {
    var sum=0
    for (i in range(0,count)) { sum+=ue_math_Add_IntInt(i,1) }
    return sum
}
[export]
def script_generated_static_vector(count:int):int {
    let v=ue_vector(3.0lf,4.0lf,0.0lf)
    var sum=0
    for (i in range(0,count)) { sum+=int(ue_math_VSize(v)) }
    return sum
}
// Reads health and writes it back unchanged: one get and one set per call.
[export]
def script_self_field_read_write(count:int):int {
    var sum=0
    for (i in range(0,count)) {
        let value=ue_get_self_int("health")
        ue_set_self_int("health",value)
        sum+=value
    }
    return sum
}
[export]
def script_object_field_read(count:int):int {
    let target=ue_self_object()
    var sum=0
    for (i in range(0,count)) { sum+=ue_object_get_int(target,"health") }
    return sum
}
