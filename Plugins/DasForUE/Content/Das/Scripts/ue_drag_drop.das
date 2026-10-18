options gen2
module ue_drag_drop public
require ue_widget_events public
// Returned UObjectHandle is weak. UE owns the operation while a native drag is active.
// Create only during OnDragDetected; inspect data only during that operation's callbacks.
def ue_drag_create(payload:UObjectHandle; visual:UObjectHandle; tag:string=""; pivot:int=0; offset:UEVector2D=ue_vector2d(0.0lf,0.0lf)):UObjectHandle {
    return __ue_drag_create(payload,visual,tag,pivot,offset)
}
def ue_drag_payload(operation:UObjectHandle):UObjectHandle { return __ue_drag_payload(operation) }
def ue_drag_visual(operation:UObjectHandle):UObjectHandle { return __ue_drag_visual(operation) }
def ue_drag_tag(operation:UObjectHandle):string { return __ue_drag_tag(operation) }
def ue_drag_is_valid(operation:UObjectHandle):bool { return __ue_drag_valid(operation) }
def ue_widget_bind_drag(widget:UObjectHandle; script:string; event:string; handler:string):void { __ue_widget_bind_drag(widget,script,event,handler) }
