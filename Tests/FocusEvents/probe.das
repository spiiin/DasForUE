options gen2
options gc = true
require ue_widget_events
var saved:UEFocusEvent
var previous_reply:UEEventReply
var score:int=0
var losses:int=0
[export]
def received(g:UEGeometry;e:UEFocusEvent):UEEventReply {
 verify(ue_focus_user(e)==37u && ue_focus_cause(e)=="Navigation","focus snapshot data")
 saved:=e
 score=11
 previous_reply:=ue_handled()
 return previous_reply
}
[export]
def lost(e:UEFocusEvent):void {
 verify(ue_focus_user(e)==37u && ue_focus_cause(e)=="Cleared","lost snapshot data")
 losses+=1
 saved:=e
}
[export]
def read_saved(input:int):int {ue_struct_collect();return ue_focus_user(saved)==37u && ue_focus_cause(saved)=="Cleared" ? 1 : -1}
[export]
def get_score(input:int):int {return score}
[export]
def get_losses(input:int):int {return losses}
[export]
def focus_command(g:UEGeometry;e:UEFocusEvent):UEEventReply {return ue_reply_focus(ue_handled(),ue_self_object())}
[export]
def reenter(g:UEGeometry;e:UEFocusEvent):UEEventReply {
 let call=ue_begin_call(ue_self_object(),"ReenterFocusForTest")
 ue_invoke_call(call)
 ue_release_call(call)
 return ue_handled()
}
[export]
def stale_reply(g:UEGeometry;e:UEFocusEvent):UEEventReply {return previous_reply}
[export]
def lost_reply(e:UEFocusEvent):void {let r=ue_handled()}
[export]
def lost_panic(e:UEFocusEvent):void {panic("deliberate focus loss failure")}
[export]
def panic_received(g:UEGeometry;e:UEFocusEvent):UEEventReply {
 let r=ue_reply_focus(ue_handled(),ue_self_object())
 panic("deliberate focus receive failure")
 return r
}
[export]
def wrong_received(g:UEGeometry;e:UEFocusEvent):int {return 1}
[export]
def wrong_lost(e:UEFocusEvent):int {return 1}
[export]
def null_event(input:int):int {var empty:UEFocusEvent;return int(ue_focus_user(empty))}
def destroy_self():void {let call=ue_begin_call(ue_self_object(),"DestroyDuringFocusForTest");ue_invoke_call(call);ue_release_call(call)}
[export]
def destroy_received(g:UEGeometry;e:UEFocusEvent):UEEventReply {let r=ue_handled();destroy_self();return r}
[export]
def destroy_lost(e:UEFocusEvent):void {destroy_self()}
