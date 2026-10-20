options gen2
require ue_gas

[ue_attribute_set]
class TargetStats { Health:float=100.0f }
[ue_gameplay_effect(attributes="TargetStats")]
class TargetDamage { Health:float= -10.0f }

[ue_gameplay_ability]
class TargetingProbe {
    Selection:int=0
    Previous:GASAbilityContext
    def Activate(context:GASAbilityContext) {
        if(self.Selection!=0) {
            verify(!ue_gas_targeting_preview(context.System,self.Selection).Valid,"ended activation selection discarded")
            verify(!ue_gas_targeting_confirm(self.Previous,self.Selection).Confirmed,"old activation refused")
        }
        var query=ue_gas_overlap(ue_vector(100.0lf,0.0lf,0.0lf),45.0f)
        self.Selection=ue_gas_targeting_begin(context,query)
        self.Previous=context
        verify(self.Selection>0 && ue_gas_targeting_preview(context,self.Selection).Valid,"activation-owned targeting")
    }
    def End(context:GASAbilityContext) {
        verify(!ue_gas_targeting_preview(context.System,self.Selection).Valid,"selection cleaned before End")
    }
}

[export]
def main(phase:int):int {
    let asc=AuthoredAbilitySystemComponentRef_from(ue_get_self_object("Abilities"))
    var query=ue_gas_trace(ue_vector(0.0lf,0.0lf,0.0lf),ue_vector(300.0lf,0.0lf,0.0lf))
    ue_gas_target_class(query,ue_class_loaded(type<ActorRef>,"/Script/Engine.Actor"))
    push(query.RequiredTags,ue_name_value("Data.Healing"))
    let selection=ue_gas_targeting_begin(asc,query)
    var preview=ue_gas_targeting_preview(asc,selection)
    verify(selection>0 && preview.Valid && length(preview.Actors)==1,"real line trace + ASC tag filter")
    var confirmed=ue_gas_targeting_confirm(asc,selection)
    verify(confirmed.Confirmed && length(confirmed.Targets.Hits)==1,"trace confirmation copies native hit")
    verify(!ue_gas_targeting_confirm(asc,selection).Confirmed,"confirm consumes session")
    ue_gas_release_targets(confirmed.Targets)
    query <- ue_gas_overlap(ue_vector(150.0lf,0.0lf,0.0lf),90.0f)
    query.MaxTargets=1
    let overlap=ue_gas_targeting_begin(asc,query)
    preview <- ue_gas_targeting_preview(asc,overlap)
    verify(preview.Valid && length(preview.Actors)==1,"overlap limit")
    query.MaxTargets=0
    verify(!ue_gas_targeting_update(asc,overlap,query),"invalid update refused")
    verify(ue_gas_targeting_preview(asc,overlap).Valid,"invalid update preserves previous preview")
    verify(ue_gas_targeting_cancel(asc,overlap),"explicit cancel")
    verify(!ue_gas_targeting_confirm(asc,overlap).Confirmed,"cancel cannot confirm")
    return 42
}
