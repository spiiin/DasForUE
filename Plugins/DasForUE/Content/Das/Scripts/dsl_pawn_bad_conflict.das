options gen2
require ue_dsl
[ue_class(parent="Actor")]
class BadParent : Character {
 [ue_function(BlueprintPure)]
 def Value():int { return 1 }
}
