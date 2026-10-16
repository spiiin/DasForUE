options gen2
options gc
require ue_dsl
[ue_interface(module_id="pureinterface", BlueprintType)]
class PureContract {
    [ue_function(BlueprintPure)]
    def Query(input:int):int {}
    [ue_function(BlueprintPure)]
    def Snapshot(@ue_out var value:int&):void {}
    [ue_function(BlueprintCallable, BlueprintPure=false)]
    def Impure():int {}
}
