options gen2
require ue_native_gameplay
[export]
def main():bool {
    let movement=ue_character_self().CharacterMovement
    movement.Velocity.x=1.0lf
    return true
}
