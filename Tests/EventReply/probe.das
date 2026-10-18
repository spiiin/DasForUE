options gen2
options gc = true
require ue_widget_events
require ue_api_umg_input
var saved:UEKeyEvent
var old_reply:UEEventReply
var executed_frame:UCallHandle
var old_frame:UCallHandle
var seen:int=0
var foreign_widget:UObjectHandle

[export]
def key_down(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
 saved := event
 seen+=1
 let focused=ue_ui_SetUserFocus(ue_ui_Handled(),C_Widget_f2421369_from(ue_self_object()),false)
 old_reply := focused.out_reply
 old_frame := ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","Handled","")
 executed_frame := ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","Handled","")
 ue_invoke_call(executed_frame)
 return focused.out_returnvalue
}
[export]
def read_saved(input:int):int {
 ue_struct_collect()
 return seen==1 && ue_input_key_GetKey(saved).f_keyname.value=="SpaceBar" ? 1 : -1
}
[export]
def key_up(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return ue_ui_Unhandled()}
[export]
def character(geometry:UEGeometry; event:UECharacterEvent):UEEventReply {return ue_ui_Handled()}
[export]
def analog(geometry:UEGeometry; event:UEAnalogInputEvent):UEEventReply {return ue_input_key_GetAnalogValue(event)==0.5 ? ue_ui_Handled() : ue_ui_Unhandled()}
[export]
def mouse_down(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
 let target=C_Widget_f2421369_from(ue_self_object())
 let captured=ue_ui_CaptureMouse(ue_ui_Handled(),target)
 let locked=ue_ui_LockMouse(captured.out_reply,target)
 let drag=ue_ui_DetectDrag(locked.out_returnvalue,target,ue_input_key_PointerEvent_GetEffectingButton(event))
 return ue_ui_SetMousePosition(drag.out_reply,ue_vector2d(40.9lf,50.8lf)).out_returnvalue
}
[export]
def mouse_up(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {
 let released=ue_ui_ReleaseMouseCapture(ue_ui_Handled())
 let unlocked=ue_ui_UnlockMouse(released.out_returnvalue)
 let unfocused=ue_ui_ClearUserFocus(unlocked.out_reply,false)
 return ue_ui_EndDragDrop(unfocused.out_returnvalue).out_reply
}
[export]
def pointer(geometry:UEGeometry; event:UEPointerEvent):UEEventReply {return ue_ui_Handled()}
[export]
def stale(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return old_reply}
[export]
def invalid_default(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {var reply:UEEventReply;return reply}
[export]
def failing(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
 let reply=ue_ui_CaptureMouse(ue_ui_Handled(),C_Widget_f2421369_from(ue_self_object()))
 panic("deliberate widget failure")
 return reply.out_reply
}
[export]
def wrong_signature(geometry:UEGeometry; event:UEKeyEvent):int {return 1}
[export]
def outside(input:int):int {let reply=ue_ui_Handled();return 0}
[export]
def stale_argument(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return ue_ui_ReleaseMouseCapture(old_reply).out_returnvalue}
[export]
def stale_frame(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {ue_invoke_call(old_frame);return ue_ui_Handled()}
[export]
def stale_result(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return __ue_result_event_reply(executed_frame,"returnvalue")}
[export]
def result_before_invoke(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
 let c=ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","Handled","")
 return __ue_result_event_reply(c,"returnvalue")
}
[export]
def unassigned(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
 let c=ue_begin_static_call("/Script/UMG.WidgetBlueprintLibrary","ReleaseMouseCapture","")
 ue_invoke_call(c)
 return __ue_result_event_reply(c,"returnvalue")
}
[export]
def bad_position(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return ue_ui_SetMousePosition(ue_ui_Handled(),ue_vector2d(1.0e30lf,0.0lf)).out_reply}
[export]
def bad_drag_key(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
 return ue_ui_DetectDrag(ue_ui_Handled(),C_Widget_f2421369_from(ue_self_object()),ue_input_key_GetKey(event)).out_reply
}
[export]
def unhandled_command(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return ue_ui_ReleaseMouseCapture(ue_ui_Unhandled()).out_reply}
[export]
def set_foreign(value:UObjectHandle):int {foreign_widget := value;return 1}
[export]
def foreign_target(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {return ue_ui_CaptureMouse(ue_ui_Handled(),C_Widget_f2421369_from(foreign_widget)).out_reply}
[export]
def null_target(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {var w:C_Widget_f2421369;return ue_ui_CaptureMouse(ue_ui_Handled(),w).out_reply}
[export]
def scope_limit(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {for(i in range(130)){let r=ue_ui_Handled()};return ue_ui_Handled()}
[export]
def old_result_outside(input:int):int {let r=__ue_result_event_reply(executed_frame,"returnvalue");return 0}

[export]
def immutable_reply(geometry:UEGeometry; event:UEKeyEvent):UEEventReply {
 let original=ue_ui_Handled()
 let ignored=ue_ui_CaptureMouse(original,C_Widget_f2421369_from(ue_self_object()))
 return ue_ui_ReleaseMouseCapture(original).out_reply
}
