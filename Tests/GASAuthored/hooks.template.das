options gen2
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

[ue_gameplay_ability(effect="ScriptHeal",cost="ScriptCost",cooldown=0.1f,
    cooldown_tag="Ability.Cooldown.Restore")]
class ScriptAbility {
    Activations:int=0
    Ends:int=0
    Cancels:int=0
    Saved:GASAbilityContext

    def Validate(context:GASAbilityContext):void {
        verify(ue_is_valid(context.Owner) && ue_is_valid(context.Avatar) && ue_is_valid(context.System),"live ability actors and ASC")
        verify(context.Level==2 && context.GrantId>0 && context.ActivationId>0,"grant context")
    }
    def Activate(context:GASAbilityContext):void {
        self.Validate(context)
        verify(ue_name_text(context.Phase)=="Activate" && !context.Cancelled,"activation phase")
        self.Activations+=1
        if(self.Activations>1) {
            verify(!ue_gas_commit(self.Saved) && !ue_gas_end(self.Saved) && !ue_gas_cancel(self.Saved),"old context cannot mutate next activation")
        }
        self.Saved=context
        if(self.Activations<=3) {
            verify(!ue_gas_apply_ability_effect(context),"preset requires explicit commit")
            verify(ue_gas_commit(context) && ue_gas_commit(context),"commit is idempotent")
        }
        if(self.Activations==1) {
            verify(self.Ends==0 && self.Cancels==0,"fresh instance")
            verify(ue_gas_apply_ability_effect(context),"heal action")
            verify(ue_gas_end(context),"explicit end")
            verify(self.Ends==1 && self.Cancels==0,"synchronous end callback")
            verify(!ue_gas_end(context) && !ue_gas_commit(context),"inactive context")
        } elif(self.Activations==2) {
            verify(self.Ends==1,"persistent instance fields")
            verify(ue_gas_apply_ability_effect(context),"second heal")
            verify(ue_gas_cancel(context),"explicit cancel")
            verify(self.Ends==2 && self.Cancels==1,"synchronous cancel then end")
        } elif(self.Activations==3) {
            verify(self.Ends==2 && self.Cancels==1,"before external cancel")
        } elif(self.Activations==4) {
            verify(self.Ends==3 && self.Cancels==2,"external cancel hooks")
        } else {
            verify(self.Ends==4 && self.Cancels==2,"external normal end hook")
            verify(ue_gas_end(context),"uncommitted normal end")
            verify(self.Ends==5,"final callback")
        }
    }
    def Cancel(context:GASAbilityContext):void {
        self.Validate(context)
        verify(context.Cancelled && ue_name_text(context.Phase)=="Cancel","cancel phase")
        verify(self.Ends==self.Activations-1,"cancel runs before end")
        self.Cancels+=1
        verify(!ue_gas_commit(context) && !ue_gas_cancel(context),"cancel cannot recommit or recurse")
    }
    def End(context:GASAbilityContext):void {
        self.Validate(context)
        verify(ue_name_text(context.Phase)=="End","end phase")
        verify(!context.System.ActivateAuthoredAbility(context.GrantId),"cannot reactivate from cleanup")
        self.Ends+=1
        verify(!ue_gas_commit(context) && !ue_gas_end(context),"end cannot recommit or recurse")
    }
}

[export]
def main(id:int):int {
    let asc=AuthoredAbilitySystemComponentRef_from(ue_get_self_object("Abilities"))
    verify(asc.ActivateAuthoredAbility(id),"activation from another executing DAS context")
    return 42
}

[ue_gameplay_ability]
class UtilityAbility {
    def Activate(context:GASAbilityContext):void {
        verify(ue_is_valid(context.Owner) && context.Level==1,"utility context")
        verify(ue_gas_commit(context),"no-cost commit")
        verify(!ue_gas_apply_ability_effect(context),"no preset effect")
        verify(ue_gas_end(context),"no-attribute scripted ability completes")
    }
}
