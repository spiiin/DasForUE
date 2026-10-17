options gen2
require ue_native_gameplay
[export]
def main():bool {
    ue_character_self().CharacterMovement.SetMovementMode(EMovementMode.MOVE_Custom,1.5f)
    return true
}
