options gen2
module dsl_class_callback_interface public
require ue_object_types public
[ue_interface(BlueprintType)]
class ClassContract {
    [ue_function(BlueprintCallable)]
    def EchoClass(value:ActorClass):ActorClass {}
    [ue_function(BlueprintCallable)]
    def ChangeClass(var value:ActorClass&; @ue_out var previous:ActorClass&):ActorClass {}
}
