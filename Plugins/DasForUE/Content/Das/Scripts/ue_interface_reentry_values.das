options gen2
module ue_interface_reentry_values public
require ue_text public
require ue_text_byte_interface_bindings public
def test_interface_values(target:UObjectHandle):bool {
    let receiver=TextByteContract_from(target)
    let original=ue_text("InterfaceReentry","Source","Original")
    if(receiver.EchoByte(uint8(255))!=uint8(255) || receiver.EchoByte(uint8(0))!=uint8(0)) { return false }
    if(ue_text_key(receiver.EchoText(original))!="Source") { return false }
    var number:uint8=0
    var title=original
    var previous:uint8=100
    var oldTitle=ue_text_invariant("old")
    let result=receiver.Change(number,title,previous,oldTitle)
    if(number!=uint8(255) || previous!=uint8(0) || ue_text_key(title)!="Changed" || ue_text_key(oldTitle)!="Source" || ue_text_key(result)!="Source") { return false }
    var bytes:array<uint8> = [uint8(0)]
    var texts:array<FText>
    receiver.ChangeArrays(bytes,texts)
    return length(bytes)==2 && bytes[0]==uint8(0) && bytes[1]==uint8(255) && length(texts)==2 && ue_text_key(texts[0])=="Array" && ue_text_is_invariant(texts[1])
}
[export]
def probe_values(target:UObjectHandle):bool { return test_interface_values(target) }
