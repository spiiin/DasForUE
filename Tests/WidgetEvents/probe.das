options gen2
options gc = true
require ue_widget_events
require ue_api_umg_input

var saved:UEKeyEvent
var old_reply:UEEventReply
var seen:int=0

[export]
def key_down(event:UEKeyEvent):UEEventReply {
    saved := event
    seen+=1
    let reply=ue_reply_focus(ue_handled(),ue_self_object())
    old_reply := reply
    return reply
}
[export]
def read_saved(input:int):int {
    ue_struct_collect()
    return seen==1 && ue_input_key_GetKey(saved).f_keyname.value=="SpaceBar" ? 1 : -1
}
[export]
def key_up(event:UEKeyEvent):UEEventReply { return ue_unhandled() }
[export]
def character(event:UECharacterEvent):UEEventReply { return ue_handled() }
[export]
def analog(event:UEAnalogInputEvent):UEEventReply {
    return ue_input_key_GetAnalogValue(event)==0.5 ? ue_handled() : ue_unhandled()
}
[export]
def mouse_down(event:UEPointerEvent):UEEventReply {
    let target=ue_self_object()
    let reply=ue_reply_capture_mouse(ue_handled(),target)
    let locked=ue_reply_lock_mouse(reply,target)
    let drag=ue_reply_detect_drag(locked,target,"LeftMouseButton")
    return ue_reply_mouse_position(drag,40,50)
}
[export]
def mouse_up(event:UEPointerEvent):UEEventReply {
    let reply=ue_reply_release_mouse(ue_handled())
    return ue_reply_end_drag(ue_reply_clear_focus(ue_reply_unlock_mouse(reply)))
}
[export]
def pointer(event:UEPointerEvent):UEEventReply { return ue_handled() }
[export]
def stale(event:UEKeyEvent):UEEventReply { return old_reply }
[export]
def invalid_default(event:UEKeyEvent):UEEventReply { var reply:UEEventReply;return reply }
[export]
def failing(event:UEKeyEvent):UEEventReply {
    let reply=ue_reply_capture_mouse(ue_handled(),ue_self_object())
    panic("deliberate widget failure")
    return reply
}
[export]
def wrong_signature(event:UEKeyEvent):int { return 1 }
[export]
def outside(input:int):int { let reply=ue_handled();return 0 }
