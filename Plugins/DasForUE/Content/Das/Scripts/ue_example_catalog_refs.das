// Example of a generated catalog reference module.
options gen2
module ue_example_catalog_refs public
require ue_dsl public
require ue_struct_map_types public
require ue_catalog_contract public

[ue_object_ref(type="structmaps.StructMapActor", catalog="ue_example_source_catalog.json")]
struct StructMapActorRef { handle:UObjectHandle }

[ue_object_ref(type="structmaps.ReflectedMapActor", catalog="ue_example_source_catalog.json")]
struct ReflectedMapActorRef { handle:UObjectHandle }
