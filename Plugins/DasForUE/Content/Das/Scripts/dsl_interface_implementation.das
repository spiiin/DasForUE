options gen2
options gc
require ue_dsl
[ue_class(module_id="authoredinterface", Blueprintable),
 ue_implements(interface=CounterContract),
 ue_implements(interface="/Script/DasRuntime.DasEventInterface")]
class CounterActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    LastMessage : string = ""
    [ue_function(BlueprintCallable)]
    def PushLabels(Values:array<string>; message:string):void {
        self.Score += int(length(Values))
        self.LastMessage = message
    }

    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score += amount }
    [ue_function(BlueprintCallable)]
    def ClearScore():void { self.Score = 0 }
    [ue_function(BlueprintCallable)]
    def Notify(Amount:int):void { self.Score += Amount }
    [ue_function(BlueprintCallable)]
    def ResetCounter():void { self.Score = 0 }

    [ue_function(BlueprintCallable)]
    def Query(Input:int):int { return Input + 7 }
    [ue_function(BlueprintCallable)]
    def Label():string { return "Authored 世界" }
    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> { var result <- clone(Values); push(result, 1.23456789012345lf); return <- result }
    [ue_function(BlueprintCallable)]
    def Stats(var Counter:int&; @ue_out var Total:int&):void { Counter += 3; Total = Counter * 2 }
}
