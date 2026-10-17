options gen2
require ue_native_gameplay
[export]
def main():bool {
 ue_character_self().CharacterMovement.SetAvoidanceGroupMask(LightingChannels())
 return true
}
