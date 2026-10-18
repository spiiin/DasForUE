options gen2
options gc = true
require ue_api_umg_input
require ue_drag_drop
var payload:UObjectHandle
var visual:UObjectHandle
var saved:UObjectHandle
var old_event:UEPointerEvent
var saved_geometry:UEGeometry
var saved_pointer:UEPointerEvent
var entered:int=0
var left:int=0
var cancelled:int=0
var dropped:int=0
[export]
def setup(p:UObjectHandle;v:UObjectHandle):int {payload:=p;visual:=v;return 1}
def null_object():UObjectHandle {var empty:UObjectHandle;return empty}
[export]
def mouse_down(g:UEGeometry;e:UEPointerEvent):UEEventReply {
 old_event:=e
 return ue_ui_DetectDragIfPressed(e,C_Widget_f2421369_from(ue_self_object()),ue_input_key_PointerEvent_GetEffectingButton(e))
}
[export]
def stale_event(g:UEGeometry;e:UEPointerEvent):UEEventReply {
 return ue_ui_DetectDragIfPressed(old_event,C_Widget_f2421369_from(ue_self_object()),ue_input_key_PointerEvent_GetEffectingButton(e))
}
[export]
def mismatch_button(g:UEGeometry;e:UEPointerEvent):UEEventReply {
 return ue_ui_DetectDragIfPressed(e,C_Widget_f2421369_from(ue_self_object()),ue_input_key_PointerEvent_GetEffectingButton(old_event))
}
[export]
def detected(g:UEGeometry;e:UEPointerEvent):UObjectHandle {
 saved_geometry:=g;saved_pointer:=e
 saved:=ue_drag_create(payload,visual,"inventory",5,ue_vector2d(0.25lf,-0.25lf))
 let f=ue_begin_call(ue_self_object(),"CollectDragGarbageForTest");ue_invoke_call(f);ue_release_call(f)
 verify(ue_drag_is_valid(saved),"created operation lost during GC")
 verify(ue_drag_tag(saved)=="inventory","created tag")
 return saved
}
[export]
def enter(g:UEGeometry;e:UEPointerEvent;op:UObjectHandle):void {
 verify(ue_drag_is_valid(op) && ue_drag_tag(op)=="inventory","enter operation")
 saved:=op;entered+=1
}
[export]
def over(g:UEGeometry;e:UEPointerEvent;op:UObjectHandle):bool {
 verify(ue_drag_is_valid(op),"over live")
 verify(ue_drag_tag(op)=="inventory","over tag")
 let p=ue_drag_payload(op)
 verify(ue_object_valid(p),"over payload")
 return true
}
[export]
def leave(e:UEPointerEvent;op:UObjectHandle):void {verify(ue_drag_is_valid(op),"leave live");left+=1}
[export]
def drop(g:UEGeometry;e:UEPointerEvent;op:UObjectHandle):bool {verify(ue_drag_tag(op)=="inventory","drop tag");dropped+=1;return true}
[export]
def cancel(e:UEPointerEvent;op:UObjectHandle):void {verify(ue_drag_is_valid(op),"cancel live");cancelled+=1}
[export]
def counts(input:int):int {return entered*1000+left*100+cancelled*10+dropped}
[export]
def read_saved(input:int):int {return ue_drag_is_valid(saved) ? 1 : 0}
[export]
def read_snapshot(input:int):int {ue_struct_collect();return ue_ui_geometry_GetLocalSize(saved_geometry).x==100.0lf && ue_input_key_PointerEvent_GetScreenSpacePosition(saved_pointer).x==30.0lf ? 1 : -1}
[export]
def outside_create(input:int):int {let op=ue_drag_create(payload,visual);return 1}
[export]
def outside_data(input:int):int {let tag=ue_drag_tag(saved);return 1}
[export]
def old_return(g:UEGeometry;e:UEPointerEvent):UObjectHandle {return saved}
[export]
def null_return(g:UEGeometry;e:UEPointerEvent):UObjectHandle {return null_object()}
[export]
def panic_create(g:UEGeometry;e:UEPointerEvent):UObjectHandle {saved:=ue_drag_create(payload,visual);panic("deliberate drag failure");return saved}
[export]
def duplicate_create(g:UEGeometry;e:UEPointerEvent):UObjectHandle {saved:=ue_drag_create(payload,visual);return ue_drag_create(payload,visual)}
[export]
def bad_pivot(g:UEGeometry;e:UEPointerEvent):UObjectHandle {return ue_drag_create(payload,visual,"",99)}
[export]
def bad_offset(g:UEGeometry;e:UEPointerEvent):UObjectHandle {return ue_drag_create(payload,visual,"",0,ue_vector2d(2.0lf,0.0lf))}
[export]
def wrong_signature(g:UEGeometry;e:UEPointerEvent):int {return 1}
[export]
def destroy_create(g:UEGeometry;e:UEPointerEvent):UObjectHandle {
 saved:=ue_drag_create(payload,visual)
 let f=ue_begin_call(ue_self_object(),"DestroyDuringDragForTest");ue_invoke_call(f);ue_release_call(f)
 return saved
}
[export]
def panic_drop(g:UEGeometry;e:UEPointerEvent;op:UObjectHandle):bool {panic("deliberate drop failure");return true}
[export]
def foreign_data(g:UEGeometry;e:UEPointerEvent;op:UObjectHandle):bool {let text=ue_drag_tag(saved);return true}
[export]
def destroy_over(g:UEGeometry;e:UEPointerEvent;op:UObjectHandle):bool {
 let f=ue_begin_call(ue_self_object(),"DestroyDuringDragForTest");ue_invoke_call(f);ue_release_call(f);return true
}
