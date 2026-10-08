options gen2
options gc
require ue_dsl
[ue_interface(module_id="authoredinterface", BlueprintType)]
class CounterContract {
    [ue_function(BlueprintCallable, Category="Counter", DisplayName="Get Score", ToolTip="Read the current score")]
    def Query(Input:int):int {}
    [ue_function(BlueprintCallable)]
    def Label():string {}
    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> {}
    [ue_function(BlueprintCallable, ref="Counter", out="Total", UnknownFlag)]
    def Stats(var Counter:int&; var Total:int&):void {}
}
