options gen2
module METHOD_REFS_MODULE public
require ue_dsl public
[ue_object_ref(script="TARGET_SOURCE", type="ValueTarget")]
struct ValueTargetRef {handle:UObjectHandle}

[ue_object_ref(script="TARGET_SOURCE", type="ValueTarget")]
struct AlternativeTargetRef {handle:UObjectHandle}

[ue_object_ref(script="DRIVER_SOURCE", type="ValueDriver")]
struct ValueDriverRef {handle:UObjectHandle}
