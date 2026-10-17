options gen2
module ue_interaction public
require ue_object_types public
require ue_interaction_contract public
def ue_interact(target:ActorRef; interactor:ActorRef):bool {
    if(!ue_is_valid(target) || !ue_implements_interface(target.handle,"/Game/DasExamples/Shared/Interactable.Interactable_C")) { return false }
    let call=ue_begin_interface_call(target.handle,"/Game/DasExamples/Shared/Interactable.Interactable_C","Interact")
    ue_arg_object(call,"interactor",interactor.handle)
    ue_invoke_call(call)
    ue_release_call(call)
    return true
}
