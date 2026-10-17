options gen2
module ue_health_refs public
require ue_dsl public
[ue_object_ref(script="dsl_health_component.das",type="HealthComponent")]
struct HealthComponentRef { handle:UObjectHandle }
