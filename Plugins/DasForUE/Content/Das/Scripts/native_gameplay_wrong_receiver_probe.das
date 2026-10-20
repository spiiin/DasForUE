options gen2
// Negative probe: a raw generated call whose receiver path does not match self (path literal on purpose).
require ue_native_gameplay
[export]
def main():bool {
    let call=__ue_begin_native_gameplay_call(ue_self_object(),"/Script/Engine.CameraComponent","SetFieldOfView")
    ue_release_call(call)
    return true
}
