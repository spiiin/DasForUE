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

typedef GASTargetData = DasGASTargetData
typedef GASCalculationContext = DasGASCalculationContext
typedef GASAbilityTaskResult = DasGASAbilityTaskResult

def ue_gas_wait_delay(context:GASAbilityContext; seconds:float):int {
    if(!ue_is_valid(context.Ability)) {return 0}
    return context.Ability.WaitScriptDelay(context.ActivationId,seconds)
}
def ue_gas_wait_event(context:GASAbilityContext; tag:ue_gameplay_tags::GameplayTag; once:bool=true; exact:bool=true):int {
    if(!ue_is_valid(context.Ability)) {return 0}
    return context.Ability.WaitScriptEvent(context.ActivationId,ue_name_value(ue_tag_name(tag)),once,exact)
}
def ue_gas_play_montage(context:GASAbilityContext; montage:auto; rate:float=1.0f; section:string=""):int {
    if(!ue_is_valid(context.Ability)) {return 0}
    return context.Ability.PlayScriptMontage(context.ActivationId,montage,rate,ue_name_value(section))
}
def ue_gas_cancel_task(context:GASAbilityContext; task:int):bool {
    return ue_is_valid(context.Ability) && context.Ability.CancelScriptTask(context.ActivationId,task)
}
def ue_gas_target(actor:auto):GASTargetData {
    var data:GASTargetData
    push(data.Actors,actor)
    return <-data
}
def ue_gas_hit_target(asc:AuthoredAbilitySystemComponentRef; hit:UEHitResult):GASTargetData {
    let id=__ue_gas_store_hit(asc.handle,hit)
    verify(id>0,"Invalid hit or GAS hit snapshot capacity exceeded")
    var data:GASTargetData
    data.Source=asc
    push(data.Hits,id)
    return <-data
}
def ue_gas_release_targets(var targets:GASTargetData):void {
    if(ue_is_valid(targets.Source)) {
        for(id in targets.Hits) {targets.Source.ReleaseAuthoredTargetHit(id)}
    }
    clear(targets.Hits)
}
def ue_gas_apply_targets(asc:AuthoredAbilitySystemComponentRef; spec:GASEffectSpec; targets:GASTargetData):array<GASEffectResult> {
    return <-asc.ApplyAuthoredTargets(spec,targets)
}
def ue_gas_apply_targets(context:GASAbilityContext; spec:GASEffectSpec; targets:GASTargetData):array<GASEffectResult> {
    verify(ue_is_valid(context.Ability) && context.Ability.IsScriptActivationCurrent(context.ActivationId) && context.Ability.IsAuthoredCommitted(),"Targeted ability action requires current committed activation")
    verify(ue_same_object(context.System,spec.Source),"Effect source must match the ability ASC")
    return <-context.System.ApplyAuthoredTargets(spec,targets)
}
def ue_gas_captured(context:GASCalculationContext; name:string):float {
    var result=0.0f
    let found=get(context.Values,ue_name_value(name)) $(value) {result=value}
    verify(found,"Unknown captured GAS attribute")
    return result
}

// A query creates an explicit preview session; only confirm allocates owned hit snapshots.
typedef GASTargetingQuery = DasGASTargetingQuery
typedef GASTargetingPreview = DasGASTargetingPreview
typedef GASTargetingResult = DasGASTargetingResult

def ue_gas_trace(start:UEVector; finish:UEVector; channel:int=3; radius:float=0.0f):GASTargetingQuery {
    var query:GASTargetingQuery
    query.IgnoreSelf=true
    query.RequireAbilitySystem=true
    query.MaxTargets=16
    query.Mode=ue_name_value("Trace")
    query.Start=start
    query.End=finish
    query.Channel=channel
    query.Radius=radius
    return <-query
}
def ue_gas_overlap(center:UEVector; radius:float; channel:int=3):GASTargetingQuery {
    var query:GASTargetingQuery
    query.IgnoreSelf=true
    query.RequireAbilitySystem=true
    query.MaxTargets=16
    query.Mode=ue_name_value("Overlap")
    query.Start=center
    query.Radius=radius
    query.Channel=channel
    return <-query
}

def ue_gas_targeting_begin(asc:AuthoredAbilitySystemComponentRef; query:GASTargetingQuery):int {
    if(!(ue_is_valid(asc))) {return 0}
    return asc.BeginAuthoredTargeting(query)
}

def ue_gas_targeting_update(asc:AuthoredAbilitySystemComponentRef; id:int; query:GASTargetingQuery):bool {
    if(!(ue_is_valid(asc))) {return false}
    return asc.UpdateAuthoredTargeting(id,query)
}

def ue_gas_targeting_preview(asc:AuthoredAbilitySystemComponentRef; id:int):GASTargetingPreview {
    if(!(ue_is_valid(asc))) {return <-GASTargetingPreview()}
    return <-asc.PreviewAuthoredTargeting(id)
}

def ue_gas_targeting_confirm(asc:AuthoredAbilitySystemComponentRef; id:int):GASTargetingResult {
    if(!(ue_is_valid(asc))) {return <-GASTargetingResult()}
    return <-asc.ConfirmAuthoredTargeting(id)
}

def ue_gas_targeting_cancel(asc:AuthoredAbilitySystemComponentRef; id:int):bool {
    if(!(ue_is_valid(asc))) {return false}
    return asc.CancelAuthoredTargeting(id)
}

def ue_gas_targeting_begin(context:GASAbilityContext; query:GASTargetingQuery):int {
    if(!(ue_is_valid(context.Ability))) {return 0}
    return context.Ability.BeginScriptTargeting(context.ActivationId,query)
}

def ue_gas_targeting_update(context:GASAbilityContext; id:int; query:GASTargetingQuery):bool {
    if(!(ue_is_valid(context.Ability))) {return false}
    return context.Ability.UpdateScriptTargeting(context.ActivationId,id,query)
}

def ue_gas_targeting_preview(context:GASAbilityContext; id:int):GASTargetingPreview {
    if(!(ue_is_valid(context.Ability))) {return <-GASTargetingPreview()}
    return <-context.Ability.PreviewScriptTargeting(context.ActivationId,id)
}

def ue_gas_targeting_confirm(context:GASAbilityContext; id:int):GASTargetingResult {
    if(!(ue_is_valid(context.Ability))) {return <-GASTargetingResult()}
    return <-context.Ability.ConfirmScriptTargeting(context.ActivationId,id)
}

def ue_gas_targeting_cancel(context:GASAbilityContext; id:int):bool {
    if(!(ue_is_valid(context.Ability))) {return false}
    return context.Ability.CancelScriptTargeting(context.ActivationId,id)
}

// Preserve a typed public API; the transport keeps only the already-loaded class path.
def ue_gas_target_class(var query:GASTargetingQuery; kind:auto):void {
    verify(ue_class_valid(kind),"Targeting class must be a loaded Actor class")
    query.ActorClassPath=ue_class_path(kind)
}

// Execution output aliases map to attributes/operations in the saved calculation schema.
typedef GASExecutionResult = DasGASExecutionResult

def ue_gas_output(var result:GASExecutionResult; output:string; magnitude:float):void {
    verify(magnitude>=-3.402823466e38f && magnitude<=3.402823466e38f,"Execution output must be finite")
    let name=ue_name_value(output)
    verify(!key_exists(result.Outputs,name),"Execution output was already supplied")
    result.Outputs[name]=magnitude
}
def ue_gas_caller(context:GASCalculationContext; alias:string):float {
    return ue_gas_captured(context,alias)
}

// Cue callbacks use [ue_event(source="Abilities",name="GameplayCueReceived")].
typedef GASCueParameters = DasGASCueParameters
typedef GASCueEvent = DasGASCueEvent

def ue_gas_cue_params(magnitude:float=0.0f):GASCueParameters {
    var params:GASCueParameters
    params.RawMagnitude=magnitude
    params.AbilityLevel=1
    params.EffectLevel=1
    return params
}
def ue_gas_cue_watch(asc:AuthoredAbilitySystemComponentRef; tag:ue_gameplay_tags::GameplayTag):bool {
    return asc.WatchAuthoredCue(ue_name_value(ue_tag_name(tag)))
}
def ue_gas_cue_unwatch(asc:AuthoredAbilitySystemComponentRef; tag:ue_gameplay_tags::GameplayTag):bool {
    return asc.UnwatchAuthoredCue(ue_name_value(ue_tag_name(tag)))
}
def ue_gas_cue_execute(asc:AuthoredAbilitySystemComponentRef; tag:ue_gameplay_tags::GameplayTag; params:GASCueParameters):bool {
    return asc.ExecuteAuthoredCue(ue_name_value(ue_tag_name(tag)),params)
}
def ue_gas_cue_add(asc:AuthoredAbilitySystemComponentRef; tag:ue_gameplay_tags::GameplayTag; params:GASCueParameters):bool {
    return asc.AddAuthoredCue(ue_name_value(ue_tag_name(tag)),params)
}
def ue_gas_cue_remove(asc:AuthoredAbilitySystemComponentRef; tag:ue_gameplay_tags::GameplayTag):bool {
    return asc.RemoveAuthoredCue(ue_name_value(ue_tag_name(tag)))
}
def ue_gas_cue_active(asc:AuthoredAbilitySystemComponentRef; tag:ue_gameplay_tags::GameplayTag):bool {
    return asc.AuthoredCueActive(ue_name_value(ue_tag_name(tag)))
}
