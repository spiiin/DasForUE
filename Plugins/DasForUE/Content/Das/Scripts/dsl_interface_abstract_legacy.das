options gen2
options gc
require ue_dsl
[ue_interface(module_id="authoredinterface", BlueprintType, out="Stats.Total")]
class CounterContract {
    def abstract Query(Input:int):int
    def abstract Label():string
    def abstract Weights(Values:array<double>):array<double>
    def abstract Stats(var Counter:int&; var Total:int&):void
}
