options gen2
require ue_dsl
require ue_component_types
[ue_class(module_id="compositionprobe", Blueprintable)]
class DualHost {
    @ue_component
    Primary : ScoreComponent
    @ue_component
    Secondary : ScoreComponent
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0
    [ue_function(BlueprintCallable)]
    def GetScore():int { return self.Score }
}
