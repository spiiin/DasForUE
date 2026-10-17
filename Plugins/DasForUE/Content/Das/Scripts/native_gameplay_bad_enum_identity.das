options gen2
require ue_native_gameplay
def needs_character(value:CharacterRef) {}
def replace_pawn(var value:NativePawnRef) {}
[export]
def main():bool {
    ue_character_self().CharacterMovement.DefaultLandMovementMode=ECameraProjectionMode.Perspective
    return true
}
