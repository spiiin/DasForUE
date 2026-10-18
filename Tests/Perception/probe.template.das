options gen2
require @PAWN_AI@
require ue_perception

[ue_class]
class PerceptionProbe : ue_dsl::Actor {
    @ue_component
    Perception:PerceptionComponentRef

    [ue_event(source="Perception",name="OnTargetPerceptionUpdated")]
    def OnTarget(actor:ActorRef; stimulus:AIStimulus):void {
        verify(ue_object_valid(actor.handle),"typed callback Actor is live")
        verify(stimulus.ExpirationAge==0.5f && stimulus.Age>=0.0f,"typed stimulus age/expiration")
        if(stimulus.Sense==AISense.Hearing) {
            if(stimulus.SuccessfullySensed) {
                verify(stimulus.Valid && !stimulus.Expired,"active hearing flags")
                verify(ue_name_text(stimulus.Tag)=="DasNoise","hearing tag")
                verify(stimulus.Strength==2.0f,"hearing strength")
                verify(stimulus.StimulusLocation.x==2000.0lf,"hearing location snapshot")
                ue_state_set_int("hearing_on",ue_state_get_int("hearing_on",0)+1)
            }
        } else {
            verify(stimulus.Sense==AISense.Sight,"native sense identity")
            let key=stimulus.SuccessfullySensed ? "sight_on" : "sight_off"
            ue_state_set_int(key,ue_state_get_int(key,0)+1)
        }
    }
}
[export]
def main(phase:int):int {
    let perception=C_AIPerceptionComponent_e0f15310_from(ue_get_self_object("Perception"))
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
