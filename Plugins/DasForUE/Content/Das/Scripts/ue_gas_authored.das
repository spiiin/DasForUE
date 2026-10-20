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

typedef GASAbilityGrant = DasAuthoredAbilityGrant
typedef GASAbilityState = DasAuthoredAbilityState
typedef GASAbilityChange = DasAuthoredAbilityState

def ue_gas_give_ability(asc:AuthoredAbilitySystemComponentRef; kind:AuthoredGameplayAbilityClass; level:int=1):GASAbilityGrant {
    verify(level>=1 && level<=10000,"Ability level must be in 1..10000")
    let grant=asc.GiveAuthoredAbility(ue_class_handle(kind),level)
    verify(ue_is_valid(grant.Target) && grant.Id>0,"Cannot grant authored ability on this ASC")
    return grant
}

def ue_gas_ability_state(grant:GASAbilityGrant):GASAbilityState {
    if(!ue_is_valid(grant.Target) || grant.Id<=0) {return GASAbilityState()}
    return grant.Target.AuthoredAbilityState(grant.Id)
}

def ue_gas_activate_ability(grant:GASAbilityGrant):bool {
    if(!ue_is_valid(grant.Target) || grant.Id<=0) {return false}
    return grant.Target.ActivateAuthoredAbility(grant.Id)
}

def ue_gas_cancel_ability(grant:GASAbilityGrant):bool {
    if(!ue_is_valid(grant.Target) || grant.Id<=0) {return false}
    return grant.Target.CancelAuthoredAbility(grant.Id)
}

def ue_gas_finish_ability(grant:GASAbilityGrant):bool {
    if(!ue_is_valid(grant.Target) || grant.Id<=0) {return false}
    return grant.Target.FinishAuthoredAbility(grant.Id)
}

def ue_gas_remove_ability(grant:GASAbilityGrant):bool {
    if(!ue_is_valid(grant.Target) || grant.Id<=0) {return false}
    return grant.Target.RemoveAuthoredAbility(grant.Id)
}

// A snapshot belongs to one native activation. Old serials cannot affect a later activation.
typedef GASAbilityContext = DasAuthoredAbilityContext

def ue_gas_commit(context:GASAbilityContext):bool {
    return ue_is_valid(context.Ability) && context.Ability.CommitScriptAbility(context.ActivationId)
}
def ue_gas_end(context:GASAbilityContext):bool {
    return ue_is_valid(context.Ability) && context.Ability.EndScriptAbility(context.ActivationId)
}
def ue_gas_cancel(context:GASAbilityContext):bool {
    return ue_is_valid(context.Ability) && context.Ability.CancelScriptAbility(context.ActivationId)
}
def ue_gas_apply_ability_effect(context:GASAbilityContext):bool {
    return ue_is_valid(context.Ability) && context.Ability.ApplyScriptAbilityEffect(context.ActivationId)
}
