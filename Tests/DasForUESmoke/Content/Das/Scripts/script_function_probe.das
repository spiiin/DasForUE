options gen2
require ue_foundation

// Typed calls from C++ (DasScript.h, TDasFunction), run by DasScriptFunctionProbe.cpp with an actor as the owner.
// Each function transforms its arguments so that the probe can check both directions.
var calls = 0

[export]
def negate(value:bool):bool { return !value }
[export]
def add(a:int; b:float):int { return a + int(b) }
[export]
def scale(value:float):float { return value * 2.0 }
[export]
def half(value:double):double { return value / 2.0lf }
[export]
def next_byte(value:uint8):uint8 { return uint8(int(value) + 1) }
[export]
def greet(name:string):string { return "hi {name}" }
[export]
def suffixed(value:UENameValue):UENameValue { return ue_name_value("{ue_name_text(value)}_x") }
[export]
def echo_text(value:UETextValue):UETextValue { return value }
[export]
def doubled(value:UEVector):UEVector { return ue_vector(value.x * 2.0lf, value.y * 2.0lf, value.z * 2.0lf) }
[export]
def turned(value:UERotator):UERotator { return ue_rotator(value.pitch, value.yaw + 90.0lf, value.roll) }
[export]
def same_transform(value:UETransform):UETransform { return value }
[export]
def same_object(value:UObjectHandle):UObjectHandle { return value }
// Several parameters of different kinds.
[export]
def describe(count:int; ratio:float; title:string; where_:UEVector):string {
    return "{title}:{count}:{int(ratio * 10.0)}:{int(where_.x)}"
}
// The owner's session keeps globals between calls.
[export]
def count_call():void { calls += 1 }
[export]
def call_count():int { return calls }
// Not exported: C++ cannot call it.
def hidden(value:int):int { return value }
// A script error inside the call.
[export]
def fails(value:int):int {
    var empty : array<int>
    return empty[value]
}
// An error only for some arguments: the first call succeeds, so the next one takes the fast path.
[export]
def checked(value:int):int {
    if (value < 0) {
        panic("negative value")
    }
    return value
}
