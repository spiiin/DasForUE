options gen2
require ue_collision_profiles
require ue_enhanced_input
[export]
def main(input:int):int {
 let profile:CollisionProfile=CollisionProfile.OverlapOnlyPawn
 let name=ue_collision_profile_name(profile)
 verify(ue_name_text(name)=="OverlapOnlyPawn","typed profile conversion")
 let call=ue_begin_call(ue_self_object(),"SetCollisionProfileName")
 ue_arg_name(call,"InCollisionProfileName",ue_name_text(name))
 ue_arg_bool(call,"bUpdateOverlaps",false)
 ue_invoke_call(call)
 ue_release_call(call)
 let read=ue_begin_call(ue_self_object(),"GetCollisionProfileName")
 ue_invoke_call(read)
 let actual=ue_result_name(read,"ReturnValue")
 ue_release_call(read)
 verify(actual==ue_name_text(name),"native FName profile roundtrip")
 return 42
}
[export]
def custom():UENameValue {return ue_collision_profile_name(CollisionProfile.DasProfileTest)}
[export]
def forged():UENameValue {return __ue_collision_profile_name(int64(0))}
[export]
def legacy(action:InputActionRef):bool {return ue_enhanced_unbind(action,"Triggered")}
[export]
def typed(action:InputActionRef):bool {return ue_enhanced_unbind(action,TriggerEvent.Triggered)}
