options gen2
module starter_refs public
require ue_dsl public
[ue_object_ref(script="starter_component.das",type="StarterLogic")]
struct StarterLogicRef { handle:UObjectHandle }
[ue_object_ref(script="starter_character.das",type="StarterCharacter")]
struct StarterCharacterRef { handle:UObjectHandle }
