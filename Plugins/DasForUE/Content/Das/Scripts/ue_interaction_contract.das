options gen2
module ue_interaction_contract public
require ue_object_types public
[ue_interface(BlueprintType,module_id="dasplayableinteraction")]
class Interactable {
    [ue_function(BlueprintCallable)]
    def Interact(interactor:ActorRef):void {}
}
