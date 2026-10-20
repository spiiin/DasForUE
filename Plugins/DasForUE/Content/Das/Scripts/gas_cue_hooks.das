options gen2
require ue_gas
require ue_api_world

[ue_attribute_set]
class CueHookStats { Health:float=100.0f }

// Each target gets its own DAS instance. The native notify asset itself is shared.
[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Hooks")]
class HookCue {
    Executions:int=0
    Active:int=0
    Ongoing:int=0
    RemovedCount:int=0
    First:GASCueEvent

    def Execute(cue:GASCueEvent) {
        self.Executions+=1
        if(self.Executions==1) { self.First <- clone(cue) }
        verify(cue.Parameters.Context.Valid && cue.Parameters.Context.HasHit && cue.Parameters.Context.Hit.ImpactPoint.x==11.0lf,"owned effect context and hit")
        verify(ue_tags_has(cue.Parameters.SourceTags,ue_tag("Data.Healing")),"source tags")
        verify(ue_tags_has(cue.Parameters.TargetTags,ue_tag("GameplayCue.Das.Sample")),"target tags")
        if(cue.Parameters.RawMagnitude==7.0f) {
            let asc=AuthoredAbilitySystemComponentRef_from(cue.Parameters.SourceObject.handle)
            var nested <- clone(cue.Parameters)
            nested.RawMagnitude=8.0f
            nested.Context.Origin=ue_vector(99.0lf,0.0lf,0.0lf)
            verify(ue_gas_cue_execute(asc,ue_tag("GameplayCue.Das.Sample.Hooks"),nested),"nested native cue submission")
            verify(self.Executions==1,"hook cannot re-enter running VM")
        }
        ue_actor_SetActorScale3D(ue_api_world::C_Actor_adf65d5b_from(cue.Target.handle),ue_vector(double(self.Executions),double(self.Active),double(self.RemovedCount)))
    }
    def OnActive(cue:GASCueEvent) {
        self.Active+=1
        ue_actor_SetActorScale3D(ue_api_world::C_Actor_adf65d5b_from(cue.Target.handle),ue_vector(double(self.Executions),double(self.Active),double(self.RemovedCount)))
    }
    def WhileActive(cue:GASCueEvent) {
        self.Ongoing+=1
        verify(self.Active==1 && self.Ongoing==1,"per-target active state")
    }
    def Removed(cue:GASCueEvent) {
        self.RemovedCount+=1
        verify(self.Executions>=2 && self.Active==1 && self.Ongoing==1,"persistent cue instance fields")
        verify(self.First.Parameters.RawMagnitude==7.0f && self.First.Parameters.Context.Origin.x==5.0lf,"retained snapshot after nested cue and GC")
        verify(self.First.Parameters.Context.HasHit && self.First.Parameters.Context.Hit.ImpactPoint.x==11.0lf,"retained hit snapshot")
        verify(ue_is_valid(self.First.Parameters.Context.Hit.Actor) && ue_is_valid(self.First.Parameters.Context.Hit.Component),"checked weak hit references")
        verify(ue_tags_has(self.First.Parameters.SourceTags,ue_tag("Data.Healing")),"retained source tags")
        ue_actor_SetActorScale3D(ue_api_world::C_Actor_adf65d5b_from(cue.Target.handle),ue_vector(double(self.Executions),double(self.Active),double(self.RemovedCount)))
    }
}
[ue_gameplay_effect(attributes="CueHookStats",cue="HookCue")]
class HookStrike { Health:float=-1.0f }

var observed:GASCueEvent
var observedCount:int=0
[ue_class]
class CueHookObserver : Actor {
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
    [ue_event(source="Abilities",name="GameplayCueReceived")]
    def Observe(cue:GASCueEvent) {
        if(cue.Parameters.RawMagnitude==7.0f) { observed <- clone(cue); observedCount+=1 }
    }
}
[export]
def main(phase:int):int {
    if(phase==0) { verify(observedCount==0,"fresh observer") }
    elif(phase==1) {
        verify(observedCount==1 && observed.Parameters.Context.Valid,"ordinary event delivery remains available")
        verify(observed.Parameters.Context.HasHit && observed.Parameters.Context.Origin.x==5.0lf,"observer snapshot after caller mutation and GC")
        verify(ue_tags_has(observed.Parameters.SourceTags,ue_tag("Data.Healing")),"observer tags snapshot")
    } else { return -1 }
    return 42
}
