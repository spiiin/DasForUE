options gen2
require ue_native_gameplay
def needs_character(value:CharacterRef) {}
def replace_pawn(var value:NativePawnRef) {}
[export]
def main():bool {
    let pawn=NativePawnRef_from(ue_self_object())
    needs_character(pawn)
    return true
}
