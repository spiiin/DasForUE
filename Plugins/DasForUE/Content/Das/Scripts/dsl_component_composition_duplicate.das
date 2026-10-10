// Legacy class-annotation compatibility rejection fixture.
options gen2
require ue_dsl
[ue_class(module_id="compositionprobe", Blueprintable),
 ue_component(name="Primary", script="dsl_component_behavior.das", type="ScoreBehavior"),
 ue_component(name="Primary", script="dsl_component_behavior.das", type="ScoreBehavior")]
class DualHost {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0
    [ue_function(BlueprintCallable)]
    def GetScore():int { return self.Score }
}
