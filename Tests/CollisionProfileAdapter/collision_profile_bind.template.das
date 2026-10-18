options gen2
require @COLLISION@
[export]
def main(input:int):int { return 42 }
[export]
def begin_play() { }
[export]
def bind_events() {
    let call=ue_begin_static_call("/Script/Engine.KismetSystemLibrary","SetCollisionProfileNameProperty","")
    ue_release_call(call)
}
