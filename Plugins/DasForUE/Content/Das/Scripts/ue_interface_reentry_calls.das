options gen2
module ue_interface_reentry_calls public
require ue_text public
def interface_bounce(target:UObjectHandle; remaining:int):int {
    let frame=ue_begin_interface_call(target,"/Game/DasInterfaceReentry/Types/ReentryContract.ReentryContract_C","Bounce")
    ue_arg_int(frame,"Input",remaining)
    ue_invoke_call(frame)
    let result=ue_result_int(frame,"ReturnValue")
    ue_release_call(frame)
    return result
}
def interface_fault(target:UObjectHandle):void {
    let frame=ue_begin_interface_call(target,"/Game/DasInterfaceReentry/Types/ReentryContract.ReentryContract_C","Fault")
    __ue_method_arg_text(frame,"title",ue_text_invariant("Original"))
    ue_invoke_call(frame)
    ue_release_call(frame)
}
[export]
def probe_bounce(remaining:int):int { return interface_bounce(ue_self_object(),remaining) }
[export]
def fault():void { interface_fault(ue_self_object()) }
[export]
def recover_fault():bool {
    let frame=ue_begin_interface_call(ue_self_object(),"/Game/DasInterfaceReentry/Types/ReentryContract.ReentryContract_C","Fault")
    __ue_method_arg_text(frame,"title",ue_text_invariant("Original"))
    var caught=false
    try { ue_invoke_call(frame) } recover { caught=true }
    var blocked=false
    try { let value=ue_result_byte(frame,"number") } recover { blocked=true }
    ue_release_call(frame)
    return caught && blocked
}

[export]
def legacy_bounce(remaining:int):int {
    return ue_interface_call_int(ue_self_object(),"/Game/DasInterfaceReentry/Types/ReentryContract.ReentryContract_C","Bounce",remaining)
}
[export]
def legacy_label():bool {
    return ue_interface_call_string(ue_self_object(),"/Game/DasInterfaceReentry/Types/ReentryContract.ReentryContract_C","Label")=="Reentrant label"
}
