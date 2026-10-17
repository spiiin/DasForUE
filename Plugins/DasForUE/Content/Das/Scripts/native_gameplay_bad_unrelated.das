options gen2
require ue_native_gameplay
def needs_character(value:CharacterRef) {}
def replace_pawn(var value:NativePawnRef) {}
[export]
def main():bool {
    needs_character(PlayerControllerRef_from(ue_self_object()))
    return true
}
