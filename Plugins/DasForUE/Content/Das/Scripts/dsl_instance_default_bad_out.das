options gen2
require ue_dsl
[ue_class]
class Bad : Actor {
 [ue_function(BlueprintCallable)]
 def Invalid(var @ue_out value:int&=1):int {return 1}
}
