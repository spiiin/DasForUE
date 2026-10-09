options gen2
require ue_dsl
require ue_object_types
[ue_interface(module_id="badnative", BlueprintType)]
class Contract {
[ue_function(BlueprintCallable, return_class=Object)]
 def Echo(value:ActorRef):ActorRef {}
}
