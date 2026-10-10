options gen2
module ue_component_types public
require ue_dsl public
// Declaration reference only: the Actor importer creates the Component Blueprint.
[ue_object_ref(script="dsl_component_behavior.das", type="ScoreBehavior")]
struct ScoreComponent { handle : UObjectHandle }
