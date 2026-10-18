options gen2
module ue_widget_events public
require ue_input_events public
require ue_geometry public

// Focus snapshots are immutable owned values; cause is an Unreal EFocusCause name.
def ue_focus_user(event:UEFocusEvent):uint { return __ue_focus_user(event) }
def ue_focus_cause(event:UEFocusEvent):string { return __ue_focus_cause(event) }

// Replies can only be created/used inside a synchronous DasScriptUserWidget input handler.
def ue_handled():UEEventReply { return __ue_widget_handled() }
def ue_unhandled():UEEventReply { return __ue_widget_unhandled() }
def ue_reply_capture_mouse(reply:UEEventReply; widget:UObjectHandle):UEEventReply { return __ue_widget_capture(reply,widget) }
def ue_reply_release_mouse(reply:UEEventReply):UEEventReply { return __ue_widget_release(reply) }
def ue_reply_focus(reply:UEEventReply; widget:UObjectHandle; all_users:bool=false):UEEventReply { return __ue_widget_focus(reply,widget,all_users) }
def ue_reply_clear_focus(reply:UEEventReply; all_users:bool=false):UEEventReply { return __ue_widget_clear_focus(reply,all_users) }
def ue_reply_lock_mouse(reply:UEEventReply; widget:UObjectHandle):UEEventReply { return __ue_widget_lock(reply,widget) }
def ue_reply_unlock_mouse(reply:UEEventReply):UEEventReply { return __ue_widget_unlock(reply) }
def ue_reply_mouse_position(reply:UEEventReply; x:int; y:int):UEEventReply { return __ue_widget_position(reply,x,y) }
def ue_reply_detect_drag(reply:UEEventReply; widget:UObjectHandle; key:string):UEEventReply { return __ue_widget_drag(reply,widget,key) }
def ue_reply_end_drag(reply:UEEventReply):UEEventReply { return __ue_widget_end_drag(reply) }
// Bind an owned DasScriptUserWidget created by ue_widget_create, before displaying it.
def ue_widget_bind_input(widget:UObjectHandle; script:string; event:string; handler:string):void { __ue_widget_bind_input(widget,script,event,handler) }
