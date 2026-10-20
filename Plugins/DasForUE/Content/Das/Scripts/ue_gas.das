options gen2
module ue_gas public
require ue_gas_types public
require ue_gameplay_tags public
require ue_native_gameplay public

// Reuse the canonical native struct views emitted by discovery / generation.
typedef GameplayAttributeChange = DasGameplayAttributeChange
typedef HealthEffectSpec = DasHealthEffectSpec
typedef HealthEffectResult = DasHealthEffectResult

def ue_gas_initialize_health(asc:HealthAbilitySystemComponentRef; maximum:float):void {
    verify(asc.InitializeHealth(maximum),"GAS requires a live owned ASC and finite positive maximum")
}
def ue_gas_value(asc:HealthAbilitySystemComponentRef; attribute:HealthAttribute):float {
    verify(asc.HealthReady(),"GAS health is not initialized")
    verify(attribute==HealthAttribute.Health || attribute==HealthAttribute.MaxHealth,"Unknown health attribute")
    return asc.ReadHealthAttribute(attribute)
}
def ue_gas_make_spec(asc:HealthAbilitySystemComponentRef; effect:HealthEffect; level:float=1.0f):HealthEffectSpec {
    verify(asc.HealthReady(),"GAS source health is not initialized")
    verify(effect==HealthEffect.Heal || effect==HealthEffect.Damage,"Unknown starter effect")
    verify(level>0.0f && level<=3.402823466e38f,"GAS effect level must be finite and positive")
    return asc.MakeHealthSpec(effect,level)
}
def ue_gas_set_magnitude(var spec:HealthEffectSpec; tag:ue_gameplay_tags::GameplayTag; amount:float):void {
    verify(amount>=0.0f && amount<=3.402823466e38f,"GAS magnitude must be finite and nonnegative")
    let required=spec.Effect==HealthEffect.Heal ? "Data.Healing" : "Data.Damage"
    verify(ue_tag_name(tag)==required,"Wrong SetByCaller magnitude tag for starter effect")
    spec.MagnitudeTag=ue_name_value(ue_tag_name(tag))
    spec.Magnitude=amount
    spec.HasMagnitude=true
}
def ue_gas_apply(target:HealthAbilitySystemComponentRef; spec:HealthEffectSpec):HealthEffectResult {
    let result=target.ApplyHealthSpec(spec)
    verify(result.Valid,"Invalid GAS spec, magnitude, source or target world")
    return result
}
def ue_gas_heal(asc:HealthAbilitySystemComponentRef; amount:float):bool {
    var spec=ue_gas_make_spec(asc,HealthEffect.Heal)
    ue_gas_set_magnitude(spec,ue_tag("Data.Healing"),amount)
    return ue_gas_apply(asc,spec).Applied
}
