options gen2
require ue_gas
require ue_gameplay_services

var saved:GameplayAttributeChange
var changes:int=0

[ue_class]
class GASHealthProbe : Actor {
    @ue_component
    Abilities:HealthAbilitySystemComponentRef
    [ue_event(source="Abilities",name="AttributeChanged")]
    def OnAttribute(change:GameplayAttributeChange):void {
        verify(change.Attribute==HealthAttribute.Health,"Health event identity")
        if(changes==0) { saved=change }
        changes+=1
    }
}

[export]
def main(phase:int):int {
    let asc=ue_ref(type<HealthAbilitySystemComponentRef>,ue_get_self_object("Abilities"))
    if(phase==0) {
        ue_gas_initialize_health(asc,100.0f)
        verify(ue_gas_value(asc,HealthAttribute.Health)==100.0f,"Initial GAS health")
        ue_apply_damage(ue_self_actor(),60.0f)
        verify(ue_gas_value(asc,HealthAttribute.Health)==40.0f,"Real GAS damage")
        verify(changes==0,"Callbacks must not re-enter an active VM")
    } elif(phase==1) {
        verify(changes==1 && saved.OldValue==100.0f && saved.NewValue==40.0f,"Owned first snapshot")
        verify(ue_gas_heal(asc,25.0f),"Instant heal applied")
        verify(ue_gas_value(asc,HealthAttribute.Health)==65.0f,"Heal 40 to 65")
    } elif(phase==2) {
        verify(changes==2,"Exactly one heal callback")
        var spec=ue_gas_make_spec(asc,HealthEffect.Heal)
        ue_gas_set_magnitude(spec,ue_tag("Data.Healing"),100.0f)
        let result=ue_gas_apply(asc,spec)
        verify(result.Applied && !result.HasActiveHandle,"Instant success is not handle validity")
        verify(ue_gas_value(asc,HealthAttribute.Health)==100.0f,"Maximum clamp")
    } elif(phase==3) {
        verify(changes==3 && saved.NewValue==40.0f,"Retained snapshot remains unchanged")
        var rejected=false
        try {ue_gas_heal(asc,-1.0f)} recover {rejected=true}
        verify(rejected && ue_gas_value(asc,HealthAttribute.Health)==100.0f,"Invalid heal has no side effect")
        ue_gas_initialize_health(asc,200.0f)
        verify(ue_gas_value(asc,HealthAttribute.MaxHealth)==100.0f,"Repeated init does not reset attributes")
    } elif(phase==4) {
        verify(changes==0,"Fresh callback state")
        ue_gas_initialize_health(asc,100.0f)
        verify(ue_gas_value(asc,HealthAttribute.Health)==90.0f,"Native health persists across VM release")
    } else {return -1}
    return 42
}
