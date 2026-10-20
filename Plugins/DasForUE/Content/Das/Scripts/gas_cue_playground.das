options gen2
require ue_gas
require ue_effects
require ue_api_world

// Demo assets are prepared under /Game/DasExamples/GASCues by gas-cue-playground.
// Production games should preload these assets before combat.
[ue_attribute_set]
class PlaygroundStats { Health:float=100.0f }

[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Playground.Impact")]
class PlaygroundImpactCue {
    Sound:SoftSound=ue_soft(type<SoftSound>,"/Engine/EngineSounds/1kSineTonePing.1kSineTonePing")
    Burst:SoftNiagaraSystem=ue_soft(type<SoftNiagaraSystem>,"/Game/DasExamples/GASCues/NS_Impact.NS_Impact")
    def Execute(cue:GASCueEvent) {
        var point=ue_actor_K2_GetActorLocation(ue_api_world::C_Actor_adf65d5b_from(cue.Target.handle))
        if(cue.Parameters.Context.HasHit) { point=cue.Parameters.Context.Hit.ImpactPoint }
        // Repeated hits replace this instance's previous named burst, bounding component count.
        verify(ue_effect_at("impact",ue_soft_load(self.Sound),ue_soft_load(self.Burst),point,"",0.25f,1.2f,0.4f),"Cannot start impact cosmetics")
    }
}

[ue_gameplay_cue(tag="GameplayCue.Das.Sample.Playground.Aura")]
class PlaygroundAuraCue {
    Fountain:SoftNiagaraSystem=ue_soft(type<SoftNiagaraSystem>,"/Game/DasExamples/GASCues/NS_Aura.NS_Aura")
    def OnActive(cue:GASCueEvent) {
        if(ue_effect_active("aura")) { return }
        let point=ue_actor_K2_GetActorLocation(ue_api_world::C_Actor_adf65d5b_from(cue.Target.handle))
        verify(ue_niagara_spawn("aura",ue_soft_load(self.Fountain),point),"Cannot start aura cosmetics")
    }
    def WhileActive(cue:GASCueEvent) {
        // Also covers native join-in-progress delivery. Never spawn a second copy.
        self.OnActive(cue)
    }
    def Removed(cue:GASCueEvent) {
        ue_effect_stop("aura")
        verify(!ue_effect_active("aura"),"Removed must release the aura")
    }
}
[ue_gameplay_effect(attributes="PlaygroundStats",cue="PlaygroundImpactCue")]
class PlaygroundHit { Health:float=-1.0f }
[ue_gameplay_effect(attributes="PlaygroundStats",duration="Infinite",cue="PlaygroundAuraCue")]
class PlaygroundAura {}
