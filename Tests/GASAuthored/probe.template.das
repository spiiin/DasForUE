options gen2
require ue_gas

var saved:GASAttributeChange
var changes:int=0
var restore:GASAbilityGrant
var held:GASAbilityGrant
var abilitySnapshot:GASAbilityChange
var abilityEvents:int=0
var cancelledEvents:int=0
var endedEvents:int=0
var fortitude:GASEffectResult
var aura:GASEffectResult
var regen:GASEffectResult
var layers:GASEffectResult
var stackSnapshot:GASEffectState
[ue_class]
class AuthoredGASProbe : Actor {
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
    [ue_event(source="Abilities",name="AbilityChanged")]
    def OnAbility(change:GASAbilityChange):void {
        abilityEvents+=1
        if(ue_name_text(change.Phase)=="Activated" && abilitySnapshot.GrantId==0) {abilitySnapshot=change}
        if(change.Cancelled) {cancelledEvents+=1}
        if(ue_name_text(change.Phase)=="Ended") {endedEvents+=1}
    }
    [ue_event(source="Abilities",name="AttributeChanged")]
    def OnChange(change:GASAttributeChange):void {
        if(changes==0) {saved=change}
        changes+=1
    }
}
[export]
def main(phase:int):int {
    let asc=ue_ref(type<AuthoredAbilitySystemComponentRef>,ue_get_self_object("Abilities"))
    if(phase==0) {
        let stats=AttributeSetClass(handle=ue_class_loaded(type<AttributeSetRef>,"@ASSET_FOLDER@/AS_CombatAttributes.AS_CombatAttributes_C"))
        ue_gas_add_attributes(asc,stats)
        ue_gas_add_attributes(asc,stats)
        verify(ue_gas_value(asc,"CombatAttributes","Health")==100.0f,"Authored initial Health")
        let hit=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_Hit.GE_Hit_C"))
        let result=ue_gas_apply(asc,ue_gas_make_spec(asc,hit))
        verify(result.Applied && !result.HasActiveHandle,"Real authored instant effect")
        verify(ue_gas_value(asc,"CombatAttributes","Health")==40.0f,"Authored damage")
        verify(changes==0,"Authored callback must not re-enter VM")
    } elif(phase==1) {
        verify(changes==1 && ue_name_text(saved.Set)=="CombatAttributes" && ue_name_text(saved.Attribute)=="Health","Typed authored snapshot identity")
        verify(saved.OldValue==100.0f && saved.NewValue==40.0f,"Owned snapshot after GC")
        let heal=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_PotionHeal.GE_PotionHeal_C"))
        verify(ue_gas_apply(asc,ue_gas_make_spec(asc,heal)).Applied,"Constant heal")
        verify(ue_gas_value(asc,"CombatAttributes","Health")==65.0f,"Heal 40 to 65")
    } elif(phase==2) {
        let heal=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_CallerHeal.GE_CallerHeal_C"))
        var spec=ue_gas_make_spec(asc,heal)
        var rejected=false
        try {ue_gas_apply(asc,spec)} recover {rejected=true}
        verify(rejected,"Missing SetByCaller is rejected before GAS")
        ue_gas_set_magnitude(spec,ue_tag("Data.Healing"),100.0f)
        verify(ue_gas_apply(asc,spec).Applied,"Authored SetByCaller")
        verify(ue_gas_value(asc,"CombatAttributes","Health")==100.0f,"Authored linked max clamp")
        verify(saved.NewValue==40.0f,"Old snapshot remains detached")
    } elif(phase==3) {
        let boost=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_ManaBoost.GE_ManaBoost_C"))
        verify(ue_gas_apply(asc,ue_gas_make_spec(asc,boost)).Applied,"Multiply operation")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==40.0f,"Mana 20 times 2")
        let assign=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_SetMana.GE_SetMana_C"))
        verify(ue_gas_apply(asc,ue_gas_make_spec(asc,assign)).Applied,"Override operation")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==7.0f,"Mana override")
    } elif(phase==4) {
        let effect=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_Fortitude.GE_Fortitude_C"))
        fortitude=ue_gas_apply(asc,ue_gas_make_spec(asc,effect))
        let state=ue_gas_effect_state(fortitude)
        verify(fortitude.Applied,"Timed effect applied")
        verify(fortitude.HasActiveHandle,"Timed effect returned a native handle")
        verify(state.Active,"Timed handle resolves on its target")
        verify(!state.Infinite,"Timed effect is finite")
        verify(state.Stacks==1,"Timed effect has one stack")
        verify(state.Duration==0.5f && state.Remaining>0.4f && ue_gas_value(asc,"CombatAttributes","MaxHealth")==120.0f,"Timed aggregate applies immediately")
        let mana=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_ManaAura.GE_ManaAura_C"))
        for(i in range(3)) {
            let next=ue_gas_apply(asc,ue_gas_make_spec(asc,mana))
            verify(next.Applied && (i==0 || next.ActiveId==aura.ActiveId),"Stack applications share one active handle")
            aura=next
        }
        stackSnapshot=ue_gas_effect_state(aura)
        verify(stackSnapshot.Active && stackSnapshot.Infinite && stackSnapshot.Remaining== -1.0f && stackSnapshot.Stacks==3,"Infinite three-stack state")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==16.0f,"Native stack magnitude scaling")
        let overflow=ue_gas_apply(asc,ue_gas_make_spec(asc,mana))
        verify(!overflow.Applied && !overflow.HasActiveHandle && ue_gas_effect_state(aura).Stacks==3,"Overflow denied without removing existing stacks")
        verify(ue_gas_remove_effect(aura,1) && ue_gas_effect_state(aura).Stacks==2,"Remove one stack")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==13.0f && stackSnapshot.Stacks==3,"State snapshot is detached from later changes")
    } elif(phase==5) {
        let state=ue_gas_effect_state(fortitude)
        verify(state.Active && state.Remaining<0.4f && state.Remaining>0.2f,"World time advances a duration effect")
        verify(ue_gas_remove_effect(aura) && !ue_gas_effect_state(aura).Active && !ue_gas_remove_effect(aura),"Remove all stacks; stale removal is harmless")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==7.0f,"Infinite aggregate is reversible")
        let effect=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_ManaRegen.GE_ManaRegen_C"))
        regen=ue_gas_apply(asc,ue_gas_make_spec(asc,effect))
        verify(ue_gas_effect_state(regen).Period==0.1f && ue_gas_value(asc,"CombatAttributes","Mana")==7.0f,"Periodic effect has no application tick when disabled")
    } elif(phase==6) {
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==9.0f,"Real GAS timer executes the periodic modifier once")
        verify(ue_gas_remove_effect(regen) && ue_gas_value(asc,"CombatAttributes","Mana")==9.0f,"Periodic base gains survive removal")
    } elif(phase==7) {
        verify(!ue_gas_effect_state(fortitude).Active && ue_gas_value(asc,"CombatAttributes","MaxHealth")==100.0f,"Timed aggregate expires automatically")
        let effect=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_LayeredFortitude.GE_LayeredFortitude_C"))
        layers=ue_gas_apply(asc,ue_gas_make_spec(asc,effect))
        verify(ue_gas_apply(asc,ue_gas_make_spec(asc,effect)).Applied && ue_gas_effect_state(layers).Stacks==2,"AggregateBySource stacking")
        verify(ue_gas_value(asc,"CombatAttributes","MaxHealth")==110.0f,"Two timed source stacks")
    } elif(phase==8) {
        verify(ue_gas_effect_state(layers).Active && ue_gas_effect_state(layers).Stacks==1 && ue_gas_value(asc,"CombatAttributes","MaxHealth")==105.0f,"Native expiration removes a single stack and refreshes duration")
        verify(!ue_gas_remove_effect(fortitude) && ue_gas_effect_state(layers).Active,"Expired IDs cannot remove later effects")
    } elif(phase==9) {
        verify(!ue_gas_effect_state(layers).Active && ue_gas_value(asc,"CombatAttributes","MaxHealth")==100.0f,"Final timed stack expires")
    } elif(phase==10) {
        let kind=AuthoredGameplayAbilityClass(handle=ue_class_loaded(type<AuthoredGameplayAbilityRef>,"@ASSET_FOLDER@/GA_RestoreHealth.GA_RestoreHealth_C"))
        let hit=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_Hit.GE_Hit_C"))
        verify(ue_gas_apply(asc,ue_gas_make_spec(asc,hit)).Applied,"Prepare Health for ability action")
        let before=abilityEvents
        restore=ue_gas_give_ability(asc,kind)
        verify(ue_gas_give_ability(asc,kind).Id==restore.Id,"Grant is idempotent for same class and level")
        let state=ue_gas_ability_state(restore)
        verify(state.Granted && state.CanActivate && !state.Active && state.Level==1,"Granted ability is activatable")
        verify(ue_gas_activate_ability(restore),"Native ability commits and applies its effect")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==4.0f && ue_gas_value(asc,"CombatAttributes","Health")==65.0f,"Ability cost and action are real GameplayEffects")
        let cooldown=ue_gas_ability_state(restore)
        verify(!cooldown.Active && !cooldown.CanActivate && cooldown.CooldownRemaining>0.3f,"Auto-ended ability remains on native cooldown")
        verify(!ue_gas_activate_ability(restore) && ue_gas_value(asc,"CombatAttributes","Mana")==4.0f,"Cooldown blocks without charging again")
        verify(abilityEvents==before,"Ability callbacks must not re-enter active VM")
    } elif(phase==11) {
        verify(ue_gas_ability_state(restore).CooldownRemaining==0.0f,"Native cooldown expires with world time")
        verify(!ue_gas_activate_ability(restore),"Insufficient Mana rejects activation")
        let auraClass=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_ManaAura.GE_ManaAura_C"))
        let temporary=ue_gas_apply(asc,ue_gas_make_spec(asc,auraClass))
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==7.0f && !ue_gas_activate_ability(restore),"Temporary Mana bonus cannot hide insufficient spendable base")
        verify(ue_gas_remove_effect(temporary) && ue_gas_value(asc,"CombatAttributes","Mana")==4.0f,"Failed activation preserves base Mana")
        let refill=AuthoredGameplayEffectClass(handle=ue_class_loaded(type<AuthoredGameplayEffectRef>,"@ASSET_FOLDER@/GE_SetMana.GE_SetMana_C"))
        verify(ue_gas_apply(asc,ue_gas_make_spec(asc,refill)).Applied && ue_gas_activate_ability(restore),"Ability reactivates after cooldown and resource refill")
        verify(ue_gas_value(asc,"CombatAttributes","Mana")==2.0f && ue_gas_value(asc,"CombatAttributes","Health")==90.0f,"Second activation pays exactly once")
        let kind=AuthoredGameplayAbilityClass(handle=ue_class_loaded(type<AuthoredGameplayAbilityRef>,"@ASSET_FOLDER@/GA_HeldHeal.GA_HeldHeal_C"))
        held=ue_gas_give_ability(asc,kind)
        verify(ue_gas_activate_ability(held) && ue_gas_ability_state(held).Active,"Manual-lifecycle ability stays active")
        verify(!ue_gas_activate_ability(held),"An active instanced ability cannot retrigger")
        verify(ue_gas_cancel_ability(held) && !ue_gas_ability_state(held).Active && !ue_gas_cancel_ability(held),"Cancel an active ability only once")
    } elif(phase==12) {
        verify(abilitySnapshot.GrantId==restore.Id && abilitySnapshot.Active && !abilitySnapshot.Cancelled && ue_name_text(abilitySnapshot.Ability)=="RestoreHealth","Owned activation snapshot survives callback, end and GC")
        verify(cancelledEvents==1 && endedEvents==2,"Cancelled and normal-ended lifecycles are distinct")
        verify(ue_gas_activate_ability(held) && ue_gas_finish_ability(held) && !ue_gas_ability_state(held).Active,"Manual normal completion")
    } elif(phase==13) {
        let old=held
        verify(ue_gas_remove_ability(old) && !ue_gas_remove_ability(old) && !ue_gas_ability_state(old).Granted,"Revoke grant and reject repeated removal")
        let kind=AuthoredGameplayAbilityClass(handle=ue_class_loaded(type<AuthoredGameplayAbilityRef>,"@ASSET_FOLDER@/GA_HeldHeal.GA_HeldHeal_C"))
        held=ue_gas_give_ability(asc,kind)
        verify(held.Id!=old.Id && ue_gas_activate_ability(held),"Regrant uses a fresh non-reused ID")
        verify(!ue_gas_cancel_ability(old) && !ue_gas_finish_ability(old) && ue_gas_ability_state(held).Active,"Stale grant cannot affect a later instance")
    } elif(phase==14) {
        verify(ue_gas_remove_ability(held) && !ue_gas_ability_state(held).Granted,"Revoking an active grant cancels and removes it")
        verify(ue_gas_remove_ability(restore),"Revoke auto-ended ability")
        let kind=AuthoredGameplayAbilityClass(handle=ue_class_loaded(type<AuthoredGameplayAbilityRef>,"@ASSET_FOLDER@/GA_RestoreHealth.GA_RestoreHealth_C"))
        let replacement=ue_gas_give_ability(asc,kind)
        verify(replacement.Id!=restore.Id && ue_gas_ability_state(replacement).CooldownRemaining>0.3f,"Revoking/regranting cannot bypass an existing tag cooldown")
        verify(ue_gas_remove_ability(replacement),"Clean up replacement grant")
    } elif(phase==15) {
        verify(!ue_gas_ability_state(restore).Granted && !ue_gas_ability_state(held).Granted,"Removed grants remain stale after cooldown expires")
        verify(cancelledEvents==2 && endedEvents==3,"Final cancellation and normal completion events delivered")
    } else {return -1}
    return 42
}
