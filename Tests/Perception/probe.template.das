options gen2
require @PAWN_AI@
[export]
def bind_events() {
    ue_subscribe_object_struct(ue_self_object(),"OnTargetPerceptionUpdated","on_target","@FRAME@")
}
[export]
def on_target(payload:UEStructValue) {
    let actor=ue_struct_get_object(payload,"arg_0")
    verify(ue_object_valid(actor),"callback Actor is live")
    let stimulus=ue_struct_nested_get(payload,"arg_1",-1)
    let sensed=ue_struct_get_bool(stimulus,"bSuccessfullySensed")
    let tag=ue_name_text(ue_struct_get_name(stimulus,"Tag"))
    if(tag=="DasNoise") {
        if(sensed) {
            verify(ue_struct_get_float(stimulus,"Strength")==2.0,"hearing strength")
            let location=ue_struct_nested_get(stimulus,"StimulusLocation",-1)
            verify(ue_struct_get_double(location,"X")==2000.0lf,"hearing location snapshot")
            ue_state_set_int("hearing_on",ue_state_get_int("hearing_on",0)+1)
        }
    } else {
        verify(tag=="None","unexpected stimulus tag")
        let key=sensed ? "sight_on" : "sight_off"
        ue_state_set_int(key,ue_state_get_int(key,0)+1)
    }
}
[export]
def main(phase:int):int {
    let perception=C_AIPerceptionComponent_e0f15310_from(ue_self_object())
    let sight=__ue_class_loaded("/Script/AIModule.AISense_Sight","/Script/CoreUObject.Object","")
    let hearing=__ue_class_loaded("/Script/AIModule.AISense_Hearing","/Script/CoreUObject.Object","")
    if(phase==0 || phase==5) {
        verify(ue_state_get_int("sight_on",0)==0 && ue_state_get_int("hearing_on",0)==0,"fresh callback context")
    } elif(phase==1) {
        verify(ue_state_get_int("sight_on",0)>=1,"DAS sight callback")
        let targets <- ue_perception_GetCurrentlyPerceivedActors(perception,sight)
        verify(length(targets)==1,"generated sight query finds source")
    } elif(phase==2) {
        verify(ue_state_get_int("sight_off",0)>=1,"DAS sight lost callback")
        let targets <- ue_perception_GetCurrentlyPerceivedActors(perception,sight)
        verify(length(targets)==0,"out-of-range target is no longer seen")
        let known <- ue_perception_GetKnownPerceivedActors(perception,sight)
        verify(length(known)==1,"lost sight target is still remembered before MaxAge")
    } elif(phase==3) {
        verify(ue_state_get_int("hearing_on",0)==1,"DAS hearing callback once")
        let targets <- ue_perception_GetCurrentlyPerceivedActors(perception,hearing)
        verify(length(targets)==1,"generated hearing query finds source")
    } elif(phase==4) {
        verify(ue_state_get_int("hearing_on",0)==1,"no duplicate/late hearing callback")
        let targets <- ue_perception_GetCurrentlyPerceivedActors(perception,hearing)
        verify(length(targets)==0,"hearing stimulus expires")
    } else { return -1 }
    return 42
}
