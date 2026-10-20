options gen2
module ue_gas_authored public
require ue_gas_authored_types public
require ue_gameplay_tags public
require ue_native_gameplay public

typedef GASAttributeChange = DasAuthoredAttributeChange
typedef GASEffectSpec = DasAuthoredEffectSpec
typedef GASEffectResult = DasAuthoredEffectResult
typedef GASEffectState = DasAuthoredEffectState

def ue_gas_add_attributes(asc:AuthoredAbilitySystemComponentRef; attributes:AttributeSetClass):void {
    verify(asc.AddAuthoredAttributes(ue_class_handle(attributes)),"Cannot register authored AttributeSet on this ASC")
}
def ue_gas_value(asc:AuthoredAbilitySystemComponentRef; set_name:string; attribute:string):float {
    let set=ue_name_value(set_name)
    let name=ue_name_value(attribute)
    verify(asc.HasAuthoredAttribute(set,name),"Unknown authored attribute set or attribute")
    return asc.ReadAuthoredAttribute(set,name)
}
def ue_gas_make_spec(asc:AuthoredAbilitySystemComponentRef; effect:AuthoredGameplayEffectClass; level:float=1.0f):GASEffectSpec {
    verify(level>0.0f && level<=3.402823466e38f,"GAS level must be finite and positive")
    var spec=asc.MakeAuthoredSpec(ue_class_handle(effect),level)
    verify(ue_is_valid(spec.Source),"Invalid authored effect class or uninitialized ASC")
    return <- spec
}
def ue_gas_set_magnitude(var spec:GASEffectSpec; tag:ue_gameplay_tags::GameplayTag; amount:float):void {
    verify(amount>=-3.402823466e38f && amount<=3.402823466e38f,"GAS magnitude must be finite")
    verify(ue_tag_valid(tag),"SetByCaller requires a registered GameplayTag")
    spec.Magnitudes[ue_name_value(ue_tag_name(tag))]=amount
}
def ue_gas_apply(target:AuthoredAbilitySystemComponentRef; spec:GASEffectSpec):GASEffectResult {
    let result=target.ApplyAuthoredSpec(spec)
    verify(result.Valid,"Invalid authored GAS spec, magnitudes, attributes or source world")
    return result
}

// HasActiveHandle describes the apply result; this queries current native liveness.
def ue_gas_effect_state(effect:GASEffectResult):GASEffectState {
    if(!ue_is_valid(effect.Target) || effect.ActiveId<=0) { return GASEffectState() }
    return effect.Target.AuthoredEffectState(effect.ActiveId)
}

def ue_gas_remove_effect(effect:GASEffectResult; stacks:int= -1):bool {
    verify(stacks== -1 || stacks>0,"Remove one or more stacks, or -1 for all")
    if(!ue_is_valid(effect.Target) || effect.ActiveId<=0) { return false }
    return effect.Target.RemoveAuthoredEffect(effect.ActiveId,stacks)
}
