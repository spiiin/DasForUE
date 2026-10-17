options gen2
require ue_native_gameplay
[export]
def main():bool {
    let text_renderer=TextRenderComponentRef_from(ue_self_object())
    text_renderer.K2_SetText("flattened string")
    return true
}
