options gen2
options gc
require ue_dsl

[ue_class(module_id="timerarrays",Blueprintable)]
class TimerArrayActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weights:array<double>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Messages:array<string>

    def BeginPlay():void {
        var scores:array<int>
        push(scores,5)
        ue_timer_start("scores",1.0,"ApplyScores",true,scores)
        push(scores,99) // Does not change the saved timer argument.
        var weights:array<double>
        push(weights,1.25lf)
        ue_timer_start("weights",0.5,"ApplyWeights",false,weights)
        var messages:array<string>
        push(messages,"Timer 世界")
        ue_timer_start("messages",0.5,"ApplyMessages",false,messages)
    }
    [ue_function(BlueprintCallable)]
    def ApplyScores(var values:array<int>):void {
        for (value in values) { self.Score += value }
        clear(values) // Only this delivery's owned DAS copy is modified.
    }
    [ue_function(BlueprintCallable)]
    def ApplyWeights(values:array<double>):void { self.Weights := values }
    [ue_function(BlueprintCallable)]
    def ApplyMessages(values:array<string>):void { self.Messages := values }
}
