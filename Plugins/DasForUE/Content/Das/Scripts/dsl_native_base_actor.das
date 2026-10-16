options gen2
require ue_dsl
[ue_class(module_id="nativebase", Blueprintable)]
class NativeActor : Actor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score:int=10
    [ue_function(BlueprintCallable)]
    def Add(input:int):int { self.Score+=input;return self.Score }
}
