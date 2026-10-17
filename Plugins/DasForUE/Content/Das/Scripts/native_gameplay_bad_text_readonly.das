options gen2
require ue_native_gameplay
[export]
def main():bool {
    let text_renderer=TextRenderComponentRef_from(ue_self_object())
    text_renderer.Text=ue_text_invariant("bypass setter")
    return true
}
