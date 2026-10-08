options gen2
require ue_dsl
require ue_object_types
[ue_class(module_id="badtyped", Blueprintable)]
class BadTarget {
    [ue_function(BlueprintCallable)]
    def Wrong(value:ObjectRef):ActorRef { return value }
}
