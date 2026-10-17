options gen2
require ue_native_gameplay
def needs_character(value:CharacterRef) {}
def replace_pawn(var value:NativePawnRef) {}
[export]
def main():bool {
    var character=ue_character_self()
    replace_pawn(character)
    return true
}
