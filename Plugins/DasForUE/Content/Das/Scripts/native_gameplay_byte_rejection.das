options gen2
require ue_native_gameplay
[export]
def main():bool {
    let movement=ue_character_self().CharacterMovement
    let before=movement.MovementMode
    let custom=movement.CustomMovementMode
    var negative=false
    var overflow=false
    try { movement.SetMovementMode(EMovementMode.MOVE_Custom,-1) } recover { negative=true }
    try { movement.SetMovementMode(EMovementMode.MOVE_Custom,256) } recover { overflow=true }
    return negative && overflow && movement.MovementMode==before && movement.CustomMovementMode==custom
}
