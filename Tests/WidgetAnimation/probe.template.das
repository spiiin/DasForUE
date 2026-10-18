options gen2
options gc = true
require ue_api_umg_input
require ue_widget_animation
var widget:UObjectHandle
var animation:UObjectHandle
var saved:array<UEWidgetAnimationHandle>
var fresh:UEWidgetAnimationHandle
var late:UCallHandle
[export]
def setup(w:UObjectHandle;a:UObjectHandle):int {widget:=w;animation:=a;return 1}
[export]
def play_all(input:int):int {
 let w=C_UserWidget_a3d12281_from(widget)
 let a=C_WidgetAnimation_462f92b4_from(animation)
 let one=ue_user_widget_PlayAnimation(w,a,0.0,1,E_EUMGSequencePlayMode_6cd7cbee.v_Forward,1.0,false)
 let two=ue_user_widget_PlayAnimationForward(w,a,1.0,false)
 let three=ue_user_widget_PlayAnimationReverse(w,a,1.0,false)
 let four=ue_user_widget_PlayAnimationTimeRange(w,a,0.0,0.08,1,E_EUMGSequencePlayMode_6cd7cbee.v_Forward,1.0,false)
 push(saved,one);push(saved,two);push(saved,three);push(saved,four)
 saved <- clone(saved)
 ue_struct_collect()
 for(h in saved){if(!ue_widget_animation_is_valid(h) || !ue_widget_animation_is_playing(h)){panic("animation producer did not return a live handle")}}
 return length(saved)
}
[export]
def read_live(input:int):int {ue_struct_collect();for(h in saved){if(!ue_widget_animation_is_valid(h)){return -1}};return length(saved)}
[export]
def read_finished(input:int):int {ue_struct_collect();for(h in saved){if(ue_widget_animation_is_valid(h) || ue_widget_animation_is_playing(h)){return -1}};return length(saved)}
[export]
def replay(input:int):int {
 fresh := ue_user_widget_PlayAnimationForward(C_UserWidget_a3d12281_from(widget),C_WidgetAnimation_462f92b4_from(animation),1.0,false)
 for(h in saved){if(ue_widget_animation_is_valid(h)){panic("old animation handle revived")}}
 return ue_widget_animation_is_valid(fresh) ? 1 : -1
}
[export]
def make_late_frame(input:int):int {
 late := ue_begin_call(widget,"PlayAnimationForward")
 ue_arg_object(late,"inanimation",animation);ue_arg_float(late,"playbackspeed",1.0);ue_arg_bool(late,"brestorestate",false)
 ue_invoke_call(late);ue_struct_collect();return 1
}
[export]
def read_destroyed(input:int):int {for(h in saved){if(ue_widget_animation_is_valid(h)){return -1}};return ue_widget_animation_is_valid(fresh) ? -1 : 1}
[export]
def read_late_frame(input:int):int {
 let h=__ue_result_widget_animation(late,"returnvalue")
 let invalid=!ue_widget_animation_is_valid(h)
 ue_release_call(late)
 return invalid ? 1 : -1
}
[export]
def read_after_reload(input:int):int {return length(saved)==0 && !ue_widget_animation_is_valid(fresh) && !ue_widget_animation_is_playing(fresh) ? 1 : -1}
[export]
def drop_handles(input:int):int {resize(saved,0);var empty:UEWidgetAnimationHandle;fresh:=empty;ue_struct_collect();return 1}
[export]
def bad_null_asset(input:int):int {var a:C_WidgetAnimation_462f92b4;let h=ue_user_widget_PlayAnimationForward(C_UserWidget_a3d12281_from(widget),a,1.0,false);return 0}
[export]
def bad_speed(input:int):int {let h=ue_user_widget_PlayAnimationForward(C_UserWidget_a3d12281_from(widget),C_WidgetAnimation_462f92b4_from(animation),-1.0,false);return 0}

[export]
def sealed_result(input:int):int {
 let call=ue_begin_call(widget,"PlayAnimationForward")
 ue_arg_object(call,"inanimation",animation);ue_arg_float(call,"playbackspeed",1.0);ue_arg_bool(call,"brestorestate",false);ue_invoke_call(call)
 let rebuild=ue_begin_call(widget,"ReconstructForAnimationTest");ue_invoke_call(rebuild);ue_release_call(rebuild)
 let h=__ue_result_widget_animation(call,"returnvalue");let good=!ue_widget_animation_is_valid(h);ue_release_call(call)
 return good ? 1 : -1
}
