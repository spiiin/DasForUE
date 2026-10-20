options gen2
require ue_gas

[ue_attribute_set]
class CueStats { Health:float=100.0f }

// These become GCN_ImpactCue / GCN_AuraCue Blueprint notify assets.
// Tags must already be registered by the project.
[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Impact",min_level=1,max_level=5)]
class ImpactCue {}
[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Aura",min_level=1,max_level=5)]
class AuraCue {}

[ue_gameplay_effect(attributes="CueStats",cue="ImpactCue")]
class CueStrike { Health:float=-5.0f }

// Repeat cue= for several cues. Native GAS owns both active cue lifetimes.
[ue_gameplay_effect(attributes="CueStats",duration="Infinite",cue="ImpactCue",cue="AuraCue")]
class CueAura {}

var first:GASCueEvent
var executed:int=0
var active:int=0
var ongoing:int=0
var removed:int=0
[ue_class]
class CueAssetExample : Actor {
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
    [ue_event(source="Abilities",name="GameplayCueReceived")]
    def OnCue(cue:GASCueEvent) {
        let phase=ue_name_text(cue.Event)
        print("CUE_ASSET_EVENT {phase} {ue_name_text(cue.Tag)}\n")
        if(phase=="Executed") { first=cue; executed+=1 }
        elif(phase=="OnActive") { active+=1 }
        elif(phase=="WhileActive") { ongoing+=1 }
        elif(phase=="Removed") { removed+=1 }
        // Play loaded sound/Niagara assets here, as in gas_cues.das.
    }
}
[export]
def main(phase:int):int {
    if(phase==0) { verify(executed==0 && active==0,"fresh session") }
    elif(phase==1) {
        verify(executed==1 && active==0,"native instant effect delivers Executed")
        verify(first.Parameters.RawMagnitude==0.0f && first.Parameters.NormalizedMagnitude==0.5f,"native default raw magnitude and cue bounds")
        verify(ue_name_text(first.Tag)=="GameplayCue.Das.Sample.Impact","native effect cue tag")
    } elif(phase==2) {
        verify(active==2 && ongoing==2 && removed==0,"two effect-owned cue lifetimes")
    } elif(phase==3) {
        verify(removed==2 && executed==1,"effect removal dispatches both cues")
        verify(first.Parameters.RawMagnitude==0.0f && first.Parameters.NormalizedMagnitude==0.5f,"retained snapshot after GC and removal")
    } else { return -1 }
    return 42
}
