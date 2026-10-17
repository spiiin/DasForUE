options gen2
require ue_animation
[ue_class(module_id="montagereload",Blueprintable)]
class MontageReloadCallbacks : Actor {
    [ue_function(BlueprintCallable)]
    def Received(event:MontageEvent):void { ue_state_set_int("events",ue_state_get_int("events",0)+1) }
}
[export]
def main(input:int):int {
    if(input==0) {
        let mesh=SkeletalMeshComponentRef_from(ue_get_self_object("Mesh"))
        let montage=AnimMontageRef_from(ue_get_self_object("Montage"))
        verify(ue_montage_play("reload",mesh,montage,"ue_MontageReloadCallbacks_Received")>0.0f,"play reload probe")
        var rejected=0
        try { ue_montage_play("validation",mesh,montage,"ue_MontageReloadCallbacks_Received",0.0f) } recover { rejected+=1 }
        try { ue_montage_play("validation",mesh,montage,"missing_handler") } recover { rejected+=1 }
        try { ue_montage_play("validation",mesh,default<AnimMontageRef>,"ue_MontageReloadCallbacks_Received") } recover { rejected+=1 }
        try { ue_montage_play("validation",mesh,montage,"ue_MontageReloadCallbacks_Received",1.0f,0.0f,ue_name_value("MissingSection")) } recover { rejected+=1 }
        verify(rejected==4 && ue_montage_playing("reload"),"invalid arguments must preserve active playback")
        return 1
    }
    if(input==2) { return ue_montage_playing("reload") ? 1 : 0 }
    return ue_state_get_int("events",0) // REVISION_MARKER
}
