options gen2
require ue_native_gameplay
[export]
def main():bool {
 var values:array<CharacterRef>
 ue_character_self().GetAllChildActors(values)
 return true
}
