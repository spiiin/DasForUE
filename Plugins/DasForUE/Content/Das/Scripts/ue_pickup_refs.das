options gen2
module ue_pickup_refs public
require ue_dsl public
require ue_object_types public

// Declaration-only import avoids a dependency from the Character back to pickups.
[ue_object_ref(script="dsl_playable_character.das",type="PlayableCharacter")]
struct PlayableCharacterRef { handle:UObjectHandle }
