options gen2
require ue_dsl
[ue_class(module_id="example",id="example.counter",parent="/Script/Engine.Actor",Blueprintable,DisplayName="Counter DSL",ToolTip="Authored class metadata")]
class Counter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="example.counter.score"
    @Category="Stats"
    @ToolTip="Score tooltip"
    Score : int = 10

    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="example.counter.gain"
    Gain : float = 1.5

    [ue_function(id="example.counter.evaluate",BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score + input }
}
