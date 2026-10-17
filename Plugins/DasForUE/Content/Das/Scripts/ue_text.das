options gen2
module ue_text public
require ue_dsl public

// A real text renderer; FText values remain opaque UETextValue snapshots.
[ue_native_object(path="/Script/Engine.TextRenderComponent")]
struct TextRenderComponentRef { handle:UObjectHandle }
def TextRenderComponentRef_from(value:UObjectHandle):TextRenderComponentRef {
    return TextRenderComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.TextRenderComponent"))
}
