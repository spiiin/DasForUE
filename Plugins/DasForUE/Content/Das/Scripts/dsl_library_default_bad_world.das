options gen2
require ue_dsl
[ue_library]
class BadDefaults {
[ue_function(BlueprintPure,WorldContext="context")] def static Bad(context:UObjectHandle=ue_null_object()):int {return 0}
}
