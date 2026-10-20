options gen2
module gas_scripted_abilities public
require ue_gas

[ue_attribute_set]
class ScriptAttributes {
    @min=0
    @max=100
    Health:float=40.0f
    @min=0
    Mana:float=20.0f
}
[ue_gameplay_effect(attributes="ScriptAttributes")]
class ScriptHeal { Health:float=25.0f }
[ue_gameplay_effect(attributes="ScriptAttributes")]
class ScriptCost { Mana:float= -5.0f }

// Real GameplayAbility instance; these counters belong to its persistent DAS object.
[ue_gameplay_ability(effect="ScriptHeal",cost="ScriptCost",cooldown=0.4f,
    cooldown_tag="Ability.Cooldown.Restore")]
class ScriptedRestore {
    Activations:int=0
    Cancellations:int=0
    LastOwner:ActorRef

    def Activate(context:GASAbilityContext) {
        self.LastOwner=context.Owner
        self.Activations+=1
        if(!ue_gas_commit(context)) {
            ue_gas_cancel(context)
            return
        }
        if(!ue_gas_apply_ability_effect(context)) {
            ue_gas_cancel(context)
            return
        }
        ue_gas_end(context)
    }
    def Cancel(context:GASAbilityContext) {
        self.Cancellations+=1
    }
    def End(context:GASAbilityContext) {
        // Both normal completion and cancellation arrive here.
        self.LastOwner=context.Owner
    }
}
