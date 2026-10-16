options gen2
require ue_dsl
[ue_library]
class BadDefaults {
[ue_function(BlueprintPure)] def static Bad(value:int=1+2):int { return value }
}
