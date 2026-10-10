// Legacy class-annotation compatibility rejection fixture.
options gen2
require ue_dsl
[ue_class(module_id="compositionprobe", Blueprintable),
 ue_component(name="Secondary", script="dsl_actor_component_owner.das", type="ComponentOwner"),
 ue_component(name="Primary", script="dsl_actor_component_owner.das", type="ComponentOwner")]
class DualHost {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0
    [ue_function(BlueprintCallable)]
    def GetScore():int { return self.Score }
}
