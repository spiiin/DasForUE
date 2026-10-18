options gen2
options gc = true
require ue_widget_events
require ue_api_umg_input

var saved:UEKeyEvent
var old_reply:UEEventReply
var seen:int=0
var saved_geometry:array<UEGeometry>

def original_geometry(g:UEGeometry):bool {
    let size=ue_ui_geometry_GetLocalSize(g)
    let absolute=ue_ui_geometry_LocalToAbsolute(g,ue_vector2d(4.0lf,6.0lf))
    let local=ue_ui_geometry_AbsoluteToLocal(g,absolute)
    return size.x==100.0lf && size.y==50.0lf && absolute.x==18.0lf && absolute.y==32.0lf && local.x==4.0lf && local.y==6.0lf
}

[export]
def key_down(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
    if(!original_geometry(geometry)){panic("key geometry mismatch")}
    saved := event
    push(saved_geometry,geometry)
    saved_geometry <- clone(saved_geometry)
    seen+=1
    let reply=ue_reply_focus(ue_handled(),ue_self_object())
    old_reply := reply
    return reply
}
[export]
def read_saved(input:int):int {
    ue_struct_collect()
    return seen==1 && ue_input_key_GetKey(saved).f_keyname.value=="SpaceBar" && length(saved_geometry)==1 && original_geometry(saved_geometry[0]) ? 1 : -1
}
[export]
def inspect_geometry(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
    let size=ue_ui_geometry_GetLocalSize(geometry)
    if(size.x!=7.0lf || size.y!=9.0lf || !original_geometry(saved_geometry[0])){panic("callback geometry revisions are aliased")}
    return ue_handled()
}
[export]
def key_up(geometry:UEGeometry; event:UEKeyEvent):UEEventReply { return ue_unhandled() }
[export]
def character(geometry:UEGeometry; event:UECharacterEvent):UEEventReply { return ue_handled() }
[export]
def analog(geometry:UEGeometry; event:UEAnalogInputEvent):UEEventReply {
    return ue_input_key_GetAnalogValue(event)==0.5 ? ue_handled() : ue_unhandled()
}
[export]
def mouse_down(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    let local=ue_ui_geometry_AbsoluteToLocal(geometry,ue_input_key_PointerEvent_GetScreenSpacePosition(event))
    if(local.x!=1.0lf || local.y!=0.0lf){panic("pointer local position mismatch")}
    let target=ue_self_object()
    let reply=ue_reply_capture_mouse(ue_handled(),target)
    let locked=ue_reply_lock_mouse(reply,target)
    let drag=ue_reply_detect_drag(locked,target,"LeftMouseButton")
    return ue_reply_mouse_position(drag,40,50)
}
[export]
def mouse_up(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
    let reply=ue_reply_release_mouse(ue_handled())
    return ue_reply_end_drag(ue_reply_clear_focus(ue_reply_unlock_mouse(reply)))
}
[export]
def pointer(geometry:UEGeometry; event:UEPointerEvent):UEEventReply { return ue_handled() }
[export]
def stale(geometry:UEGeometry; event:UEKeyEvent):UEEventReply { return old_reply }
[export]
def invalid_default(geometry:UEGeometry; event:UEKeyEvent):UEEventReply { var reply:UEEventReply;return reply }
[export]
def failing(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
    let reply=ue_reply_capture_mouse(ue_handled(),ue_self_object())
    panic("deliberate widget failure")
    return reply
}
[export]
def wrong_signature(geometry:UEGeometry; event:UEKeyEvent):int { return 1 }
[export]
def old_signature(event:UEKeyEvent):UEEventReply { return ue_handled() }
[export]
def wrong_geometry(geometry:UEKeyEvent; event:UEKeyEvent):UEEventReply { return ue_handled() }
[export]
def outside(input:int):int { let reply=ue_handled();return 0 }
