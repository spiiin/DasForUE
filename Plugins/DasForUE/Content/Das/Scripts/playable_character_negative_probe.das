options gen2
require ue_character_control

// Test-only fixture: keep intentional errors out of the playable example.
[export]
def main():bool {
    let actor=ue_ref(type<ActorRef>,ue_self_object())
    let movement=ue_component_get(type<CharacterMovementRef>,actor,"CharMoveComp")
    try { movement.MaxWalkSpeed=-1.0f } recover { return true }
    return false
}
