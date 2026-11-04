options gen2
module ue_behavior_tree public
require ue_ai public

[ue_native_base(path="/Script/AIModule.BTTask_BlueprintBase")]
class BTTask {}

[ue_native_base(path="/Script/AIModule.BTDecorator_BlueprintBase")]
class BTDecorator {}

[ue_native_base(path="/Script/AIModule.BTService_BlueprintBase")]
class BTService {}

[ue_native_base(path="/Script/AIModule.EnvQueryContext_BlueprintBase")]
class EQSContext {}

def ue_bt_finish(success:bool):void {
    let call=ue_begin_call(ue_self_object(),"FinishExecute")
    ue_arg(call,"bSuccess",success)
    ue_invoke_call(call)
    ue_release_call(call)
}

def ue_ai_blackboard(controller:AIControllerRef):UObjectHandle {
    return ue_get_object(controller.handle,"Blackboard")
}

def ue_blackboard_object(board:UObjectHandle; key:string):ActorRef {
    let call=ue_begin_call(board,"GetValueAsObject")
    ue_arg(call,"KeyName",ue_name_value(key))
    ue_invoke_call(call)
    let result=ue_result_object(call,"ReturnValue")
    ue_release_call(call)
    return ue_ref(type<ActorRef>,result)
}

def ue_blackboard_set_object(board:UObjectHandle; key:string; value:ActorRef):void {
    let call=ue_begin_call(board,"SetValueAsObject")
    ue_arg(call,"KeyName",ue_name_value(key))
    ue_arg(call,"ObjectValue",value)
    ue_invoke_call(call)
    ue_release_call(call)
}

def ue_blackboard_set_bool(board:UObjectHandle; key:string; value:bool):void {
    let call=ue_begin_call(board,"SetValueAsBool")
    ue_arg(call,"KeyName",ue_name_value(key))
    ue_arg(call,"BoolValue",value)
    ue_invoke_call(call)
    ue_release_call(call)
}

def ue_ai_run_behavior_tree(controller:AIControllerRef; tree:ObjectRef):bool {
    let call=ue_begin_call(controller.handle,"RunBehaviorTree")
    ue_arg(call,"BTAsset",tree)
    ue_invoke_call(call)
    let result=ue_result_bool(call,"ReturnValue")
    ue_release_call(call)
    return result
}

def ue_ai_brain(controller:AIControllerRef; method:string; reason:string):void {
    let brain=ue_get_object(controller.handle,"BrainComponent")
    if (!ue_object_valid(brain)) { return }
    let call=ue_begin_call(brain,method)
    ue_arg(call,"Reason",reason)
    ue_invoke_call(call)
    ue_release_call(call)
}

def ue_ai_brain_pause(controller:AIControllerRef; paused:bool; reason:string):bool {
    return __ue_ai_brain_pause(controller.handle,paused,reason)
}
