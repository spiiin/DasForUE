options gen2
require ue_gas

var first:GASCueEvent
var executed:int=0
var active:int=0
var ongoing:int=0
var removed:int=0

[ue_class]
class GASCueExample : Actor {
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef

    [ue_event(source="Abilities",name="GameplayCueReceived")]
    def OnCue(cue:GASCueEvent) {
        let event=ue_name_text(cue.Event)
        if(event=="Executed") {
            if(cue.Parameters.RawMagnitude==21.0f) {
                verify(!ue_gas_cue_watch(self.Abilities,ue_tag("GameplayCue.Das.Sample")),"new route refused during synchronous native cue callback")
            }
            if(executed==0) { first <- clone(cue) }
            executed+=1
        } elif(event=="OnActive") { active+=1 }
        elif(event=="WhileActive") { ongoing+=1 }
        elif(event=="Removed") { removed+=1 }
        // A game can call ue_effect_at / ue_effect_stop here using its loaded sound/Niagara assets.
        // Keep gameplay damage/state changes in the ability/effect, not in cosmetic cue handlers.
    }
}

[export]
def main(phase:int):int {
    let asc=AuthoredAbilitySystemComponentRef_from(ue_get_self_object("Abilities"))
    let impact=ue_tag("GameplayCue.Das.Sample.Impact")
    let aura=ue_tag("GameplayCue.Das.Sample.Aura")
    if(phase==0) {
        verify(ue_gas_cue_watch(asc,impact) && ue_gas_cue_watch(asc,aura),"register native cue routing")
        verify(ue_gas_cue_watch(asc,impact),"watch is idempotent")
        var params=ue_gas_cue_params(7.0f)
        params.Location=ue_vector(11.0lf,22.0lf,33.0lf)
        params.Normal=ue_vector(0.0lf,0.0lf,1.0lf)
        params.NormalizedMagnitude=0.5f
        params.AbilityLevel=2
        params.EffectLevel=3
        verify(ue_gas_cue_execute(asc,impact,params),"execute native cue")
        verify(ue_gas_cue_add(asc,aura,params) && ue_gas_cue_active(asc,aura),"native active cue")
        verify(!ue_gas_cue_add(asc,aura,params),"duplicate manual add refused")
        verify(!ue_gas_cue_unwatch(asc,aura),"remove active manual cue before unwatch")
        params.RawMagnitude=99.0f
        params.Location=ue_vector(99.0lf,0.0lf,0.0lf)
        verify(executed==0 && active==0 && ongoing==0,"events cannot re-enter running VM")
    } elif(phase==1) {
        verify(executed==1 && active==1 && ongoing==1,"native execute/active/while-active events")
        verify(first.Parameters.RawMagnitude==7.0f && first.Parameters.Location.x==11.0lf && first.Parameters.Normal.z==1.0lf,"owned parameter snapshot")
        verify(first.Parameters.NormalizedMagnitude==0.5f && first.Parameters.AbilityLevel==2 && first.Parameters.EffectLevel==3,"levels and normalized magnitude")
        verify(ue_is_valid(first.Target) && ue_is_valid(first.Parameters.Instigator),"target and default instigator")
        verify(ue_name_text(first.Tag)=="GameplayCue.Das.Sample.Impact" && ue_name_text(first.MatchedTag)=="GameplayCue.Das.Sample.Impact","cue tag identity")
        verify(ue_gas_cue_remove(asc,aura) && !ue_gas_cue_active(asc,aura),"remove native active cue")
        verify(!ue_gas_cue_remove(asc,aura),"repeat remove refused")
        verify(removed==0,"remove callback is queued during current VM call")
    } elif(phase==2) {
        verify(removed==1 && first.Parameters.RawMagnitude==7.0f && first.Parameters.Location.x==11.0lf,"removed event and retained first snapshot")
        verify(ue_gas_cue_unwatch(asc,aura),"unwatch after removal")
        verify(!ue_gas_cue_unwatch(asc,aura),"repeated unwatch")
    } elif(phase==3) {
        verify(executed==2 && removed==1 && first.Parameters.RawMagnitude==7.0f,"native caller routed through real notify; prior snapshot unchanged")
    } elif(phase==4) {
        verify(executed==0 && active==0 && removed==0,"fresh VM has no stale callback state")
    } else { return -1 }
    return 42
}
