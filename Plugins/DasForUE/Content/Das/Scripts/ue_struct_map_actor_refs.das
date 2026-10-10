options gen2
module ue_struct_map_actor_refs public
require ue_dsl public

[ue_object_ref(type="StructMapActor", catalog="ue_example_source_catalog.json")]
struct StructMapActorRef { handle:UObjectHandle }

// A second nominal view for the same authored class must not share converters.
[ue_object_ref(type="StructMapActor", catalog="ue_example_source_catalog.json")]
struct AlternativeMapActorRef { handle:UObjectHandle }
