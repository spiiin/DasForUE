options gen2
require ue_dsl
[ue_class(parent="/Script/CoreUObject.Object")]
class BadParent {
 [ue_function(BlueprintPure)]
 def Value():int { return 1 }
}
