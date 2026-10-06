options gen2
require ue_dsl
var calls = 0
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

    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="example.counter.bonus"
    Bonus : int = 5

    @ue_property=(EditAnywhere,BlueprintReadOnly)
    @ue_id="example.counter.locked"
    @Category="Stats"
    @DisplayName="Locked score"
    Locked : int = 12

    [ue_function(id="example.counter.evaluate",BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score + input + self.Bonus + 100 }

    [ue_function(id="example.counter.inspect",BlueprintCallable,Category="State",DisplayName="Inspect state",ToolTip="Named entry point")]
    def Inspect(input:int):int { calls += 1; return self.Locked + input + calls }

    [ue_function(id="example.counter.inspect_again",BlueprintCallable,Category="State",DisplayName="InspectAgain state",ToolTip="Named entry point")]
    def InspectAgain(input:int):int { calls += 1; return input + calls + 1000 }
}
