options gen2
require ue_dsl
require ue_object_types
[ue_class(module_id="extendeddefaults",parent="ActorComponent",Blueprintable)]
class DefaultsWorker {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=7
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Pose:UETransform
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Visual:ObjectRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    SpawnType:ActorClass
    [ue_function(BlueprintPure)]
    def ReadBonus():int { return self.Bonus }
}
