options gen2
require ue_dsl
[ue_class(module_id="cleaneditor",Blueprintable)]
class CleanActor : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=10
    [ue_function(BlueprintCallable)]
    def Add(value:int):int { self.Score+=value; return self.Score }
}
