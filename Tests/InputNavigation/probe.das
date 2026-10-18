options gen2
options gc = true
require ue_api_umg_input
require ue_widget_events
var saved_key:UEKeyEvent
var saved_analog:UEAnalogInputEvent
var old_frame:UCallHandle
var last_result:int=0
[export]
def key(g:UEGeometry;e:UEKeyEvent):UEEventReply {
 saved_key:=e
 let a=ue_input_key_Key_GetNavigationActionFromKey(e)
 let d=ue_input_key_Key_GetNavigationDirectionFromKey(e)
 verify(a==ue_input_key_Key_GetNavigationActionFromKey(e) && d==ue_input_key_Key_GetNavigationDirectionFromKey(e),"key query cache")
 verify(E_EUINavigationAction_4a0ae499_to_symbol(a)=="Back" && E_EUINavigation_e4554eeb_to_symbol(d)=="Right","custom user config")
 old_frame:=ue_begin_static_call("/Script/Engine.KismetInputLibrary","Key_GetNavigationDirectionFromKey","")
 return ue_handled()
}
[export]
def stale(g:UEGeometry;e:UEKeyEvent):UEEventReply {let d=ue_input_key_Key_GetNavigationDirectionFromKey(saved_key);return ue_handled()}
[export]
def stale_frame(g:UEGeometry;e:UEKeyEvent):UEEventReply {ue_invoke_call(old_frame);return ue_handled()}
[export]
def outside(input:int):int {let d=ue_input_key_Key_GetNavigationDirectionFromKey(saved_key);return 1}
[export]
def read_saved(input:int):int {ue_struct_collect();return ue_input_key_GetKey(saved_key).f_keyname.value=="F8" ? 1 : -1}
def query_analog(e:UEAnalogInputEvent):void {
 saved_analog:=e
 let a=ue_input_key_Key_GetNavigationDirectionFromAnalog(e)
 let b=ue_input_key_Key_GetNavigationDirectionFromAnalog(e)
 verify(a==b,"analog result cache")
 last_result=E_EUINavigation_e4554eeb_to_symbol(a)=="Right" ? 1 : 0
}
[export]
def analog(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {query_analog(e);return ue_handled()}
[export]
def analog_unhandled(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {query_analog(e);return ue_unhandled()}
[export]
def analog_panic(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {
 query_analog(e)
 let reply=ue_reply_capture_mouse(ue_handled(),ue_self_object())
 panic("deliberate navigation failure")
 return reply
}
[export]
def stale_analog(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {let d=ue_input_key_Key_GetNavigationDirectionFromAnalog(saved_analog);return ue_handled()}
[export]
def last(input:int):int {return last_result}
[export]
def noop(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {return ue_unhandled()}
def destroy_widget():void {let f=ue_begin_call(ue_self_object(),"DestroyDuringNavigationForTest");ue_invoke_call(f);ue_release_call(f)}
[export]
def destroy_before(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {destroy_widget();query_analog(e);return ue_handled()}
[export]
def destroy_after(g:UEGeometry;e:UEAnalogInputEvent):UEEventReply {query_analog(e);destroy_widget();return ue_handled()}
[export]
def wrong_signature(g:UEGeometry;e:UEAnalogInputEvent):int {return 1}
