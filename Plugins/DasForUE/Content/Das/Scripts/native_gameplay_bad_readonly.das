options gen2
require ue_native_gameplay
[export]
def main():bool { let character=ue_character_self(); character.CharacterMovement=default<CharacterMovementRef>; return true }
