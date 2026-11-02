options gen2
module owned_object_refs public
require ue_dsl public
require ue_object_types public
[ue_object_ref(script="owned_object_action.das",type="OwnedAction")]
struct OwnedActionRef { handle:UObjectHandle }
typedef OwnedActionClass = $UEClass<OwnedActionRef>
