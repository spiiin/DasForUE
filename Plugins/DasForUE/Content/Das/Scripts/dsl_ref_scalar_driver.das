options gen2
require ue_dsl
require ue_scalar_refs
require ue_reference_functions
var remembered:ScalarTargetRef
[ue_class(module_id="refscalar", Blueprintable)]
class RefHarness {
 [ue_function(BlueprintCallable)]
 def Exercise(target:UObjectHandle):int {
  let actor = ue_ref(type<ScalarTargetRef>,target)
  return apply_scalar_fields(actor)
 }
 [ue_function(BlueprintCallable)]
 def Values(target:UObjectHandle; peer:UObjectHandle):int {
  let actor = ue_ref(type<ScalarTargetRef>,target)
  return apply_value_fields(actor,peer)
 }
 [ue_function(BlueprintCallable)]
 def WrongStruct(target:UObjectHandle):void {let actor=ue_ref(type<ScalarTargetRef>,target);actor.Payload=ue_struct_value("DasCallMetrics")}
 [ue_function(BlueprintCallable)]
 def WrongEnum(target:UObjectHandle):void {let actor=ue_ref(type<ScalarTargetRef>,target);actor.Mode=ue_enum_value("EDasLegacyMode","DasLegacyActive")}
 [ue_function(BlueprintCallable)]
 def WrongObject(target:UObjectHandle; value:UObjectHandle):void {let actor=ue_ref(type<ScalarTargetRef>,target);actor.Target=value}
 [ue_function(BlueprintCallable)]
 def Remember(target:UObjectHandle):void {remembered = ue_ref(type<ScalarTargetRef>,target)}
 [ue_function(BlueprintCallable)]
 def Recall():int {return read_scalar_field(remembered)}
 [ue_function(BlueprintCallable)]
 def Forged(target:UObjectHandle):int {
  let actor = ScalarTargetRef(handle=target)
  return actor.Number
 }
}
