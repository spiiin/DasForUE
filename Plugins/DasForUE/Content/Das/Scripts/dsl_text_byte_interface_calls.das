options gen2
require ue_text
[export]
def main():bool {
    let path="/Game/DasTextByteEvents/Types/TextByteContract.TextByteContract_C"
    let original=ue_text("TextByteEvents","Source","Original text")
    let call=ue_begin_interface_call(ue_self_object(),path,"Change")
    ue_arg_byte(call,"number",0)
    __ue_method_arg_text(call,"title",original)
    ue_invoke_call(call)
    return ue_result_byte(call,"number")==255 && ue_result_byte(call,"previous")==0 && ue_text_key(__ue_method_result_text(call,"title"))=="Changed" && ue_text_key(__ue_method_result_text(call,"oldTitle"))=="Source" && ue_text_key(__ue_method_result_text(call,"ReturnValue"))=="Source"
}
