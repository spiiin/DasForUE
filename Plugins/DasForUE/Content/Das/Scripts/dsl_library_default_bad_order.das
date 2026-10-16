options gen2
require ue_dsl
[ue_library]
class BadDefaults {
[ue_function(BlueprintPure)] def static Bad(first:int=1; second:int):int { return first+second }
}
