options gen2
require ue_dsl
[ue_class]
class Bad : Actor {
 [ue_function(BlueprintCallable)]
 def Invalid(value:int=ue_int_value(1)):int {return 1}
}
