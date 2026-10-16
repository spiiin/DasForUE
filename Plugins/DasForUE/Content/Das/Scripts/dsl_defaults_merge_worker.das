options gen2
require ue_dsl
[ue_class(module_id="defaultsmerge",parent="ActorComponent",Blueprintable)]
class MergeWorker {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=7
    [ue_function(BlueprintPure)]
    def ReadBonus():int { return self.Bonus }
}
