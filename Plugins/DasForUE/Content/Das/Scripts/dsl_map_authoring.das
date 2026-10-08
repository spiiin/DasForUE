options gen2
options gc
require ue_dsl

[ue_struct(id="mapdemo.Scores", module_id="mapdemo", BlueprintType)]
struct Scores {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Values : table<int; int>
}
[ue_class(module_id="mapdemo", type_registry="REGISTRY", Blueprintable)]
class MapActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Scores = Scores()
    [ue_function(BlueprintCallable)]
    def Put(key:int; value:int):void {
        var data <- clone(self.Config)
        data.Values[key] = value
        self.Config <- data
    }
    [ue_function(BlueprintCallable)]
    def Total():int {
        var data <- clone(self.Config)
        var result = 0
        for (value in values(data.Values)) { result += value }
        return result
    }
    [ue_function(BlueprintCallable)]
    def Echo(value:Scores):Scores {
        var result <- clone(value)
        return <- result
    }
}
