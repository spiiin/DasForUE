options gen2
require ue_text
require ue_text_byte_types
[ue_object_ref(script="dsl_text_byte.das", type="TextByteActor")]
struct TextByteRef { handle:UObjectHandle }
[export]
def main():bool {
    let actor=ue_ref(type<TextByteRef>,ue_self_object())
    actor.Priority=uint8(0)
    if (actor.Priority!=uint8(0)) { return false }
    let byteValue=actor.EchoByte(uint8(255))
    if (byteValue!=uint8(255) || actor.DefaultByte()!=uint8(255)) { return false }
    let defaultText=actor.DefaultText()
    if (ue_text_key(defaultText)!="Default") { return false }
    actor.Title=ue_text_invariant("Invariant title")
    if (!ue_text_is_invariant(actor.Title)) { return false }
    let textValue=actor.EchoText(ue_text("DasTextByte","Roundtrip","Localized text"))
    if (ue_text_key(textValue)!="Roundtrip" || ue_text_namespace(textValue)!="DasTextByte") { return false }
    var priority=uint8(41)
    var title=ue_text_invariant("")
    let returned=actor.Adjust(priority,title)
    var data=actor.Config
    if (priority!=uint8(42) || ue_text_key(title)!="Output" || ue_text_key(returned)!="Output" || data.Priority!=uint8(42) || ue_text_key(data.Title)!="Output") { return false }
    data.Priority=uint8(255)
    data.Title=ue_text("DasTextByte","Replacement","Replacement title")
    actor.Config=data
    let replaced=actor.Config
    if (replaced.Priority!=uint8(255) || ue_text_key(replaced.Title)!="Replacement") { return false }
    let finalByte=actor.Reverse(title,priority)
    return finalByte==uint8(255) && priority==uint8(255) && ue_text_is_invariant(title) && ue_text_to_string(title)=="Replaced" && ue_text_key(actor.Title)=="Output"
}
