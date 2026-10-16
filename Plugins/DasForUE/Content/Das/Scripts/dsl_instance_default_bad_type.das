options gen2
require ue_dsl
[ue_class]
class Bad : Actor {
 [ue_function(BlueprintCallable)]
 def Invalid(value:UEVector=ue_vector(1.,2.,3.)):int {return 1}
}
