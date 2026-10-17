options gen2
require ue_native_gameplay
def needs_character(value:CharacterRef) {}
def replace_pawn(var value:NativePawnRef) {}
[export]
def main():bool {
    let camera=CameraComponentRef_from(ue_self_object())
    camera.SetProjectionMode(EMovementMode.MOVE_Walking)
    return true
}
