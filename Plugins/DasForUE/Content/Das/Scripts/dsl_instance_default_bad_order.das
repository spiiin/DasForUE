options gen2
require ue_dsl
[ue_class]
class Bad : Actor {
 [ue_function(BlueprintCallable)]
 def Invalid(value:int=1; required:int):int {return 1}
}
