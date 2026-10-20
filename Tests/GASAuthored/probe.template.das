options gen2
require ue_gas

var saved:GASAttributeChange
var changes:int=0
[ue_class]
class AuthoredGASProbe : Actor {
    @ue_component
    Abilities:AuthoredAbilitySystemComponentRef
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
    } else {return -1}
    return 42
}
