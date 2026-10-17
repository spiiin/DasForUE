options gen2
require ue_text
[export]
def main():bool {
    let path="/Game/DasTextByteEvents/Types/TextByteContract.TextByteContract_C"
    let original=ue_text("TextByteEvents","Source","Original text")
    for (boundary in [0,255]) {
        let byteCall=ue_begin_interface_call(ue_self_object(),path,"EchoByte")
        ue_arg_byte(byteCall,"value",boundary)
        ue_invoke_call(byteCall)
        let same=ue_result_byte(byteCall,"ReturnValue")==boundary
        ue_release_call(byteCall)
        if(!same) { return false }
    }
    for (message in [original,ue_text_invariant("Invariant")]) {
        let textCall=ue_begin_interface_call(ue_self_object(),path,"EchoText")
        __ue_method_arg_text(textCall,"value",message)
        ue_invoke_call(textCall)
        let result=__ue_method_result_text(textCall,"ReturnValue")
        let same=ue_text_key(result)==ue_text_key(message) && ue_text_namespace(result)==ue_text_namespace(message) && ue_text_is_invariant(result)==ue_text_is_invariant(message) && ue_text_to_string(result)==ue_text_to_string(message)
        ue_release_call(textCall)
        if(!same) { return false }
    }
    let call=ue_begin_interface_call(ue_self_object(),path,"Change")
    ue_arg_byte(call,"number",0)
    __ue_method_arg_text(call,"title",original)
    ue_invoke_call(call)
    let valid=ue_result_byte(call,"number")==0 && ue_result_byte(call,"previous")==0 && ue_text_key(__ue_method_result_text(call,"title"))=="Source" && ue_text_key(__ue_method_result_text(call,"oldTitle"))=="Source" && ue_text_key(__ue_method_result_text(call,"ReturnValue"))=="Source"
    ue_release_call(call)
    return valid
}
