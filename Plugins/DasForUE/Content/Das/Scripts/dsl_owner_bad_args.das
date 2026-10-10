options gen2
require ue_dsl
require ue_actor_refs
require ue_scalar_refs
[ue_class(module_id="ownerprobe", parent="ActorComponent", Blueprintable)]
class OwnerAccessorProbe {
 [ue_function(BlueprintCallable)]
 def Read():int { let actor=ue_owner();return actor.Score }
 [ue_function(BlueprintCallable)]
 def Wrong():int { let actor=ue_owner(type<ScalarTargetRef>);return actor.Number }
}
