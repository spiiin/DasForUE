options gen2
require ue_dsl
require ue_scalar_refs
var remembered:ScalarTargetRef
[ue_class(module_id="refscalar", Blueprintable)]
class RefHarness {
 [ue_function(BlueprintCallable)]
 def Exercise(target:UObjectHandle):int {
  var actor = ue_ref(type<ScalarTargetRef>,target)
  let alias = actor
  actor.Locked = 1
  actor.Ratio = alias.Ratio + 0.5
  actor.Precise *= 2.0lf
  actor.Enabled = !alias.Enabled
  actor.Label = alias.Label + " changed"
  actor.Key = ue_name_value("Changed")
  return actor.Number + (actor.Ratio == 2.0 && actor.Precise == 4.5lf && !actor.Enabled && actor.Label == "Initial changed" && ue_names_equal(actor.Key,ue_name_value("Changed")) ? 100 : 0)
 }
 [ue_function(BlueprintCallable)]
 def Remember(target:UObjectHandle):void {remembered = ue_ref(type<ScalarTargetRef>,target)}
 [ue_function(BlueprintCallable)]
 def Recall():int {return remembered.Number}
 [ue_function(BlueprintCallable)]
 def Forged(target:UObjectHandle):int {
  let actor = ScalarTargetRef(handle=target)
  return actor.Number
 }
}
