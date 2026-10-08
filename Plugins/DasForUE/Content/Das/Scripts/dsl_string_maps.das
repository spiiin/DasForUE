options gen2
options gc
require ue_dsl

[ue_struct(id="mapdemo.Scores", module_id="mapdemo", BlueprintType)]
struct Scores {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Values : table<string; string>
}
[ue_class(module_id="mapdemo", type_registry="REGISTRY", Blueprintable)]
class MapActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Scores = Scores()
    [ue_function(BlueprintCallable)]
    def Put(key:string; value:string):void {
        var data <- clone(self.Config)
        data.Values[key] = value
        self.Config <- data
    }
    [ue_function(BlueprintCallable)]
    def Count():int {
        var data <- clone(self.Config)
        return int(length(data.Values))
    }
    [ue_function(BlueprintCallable)]
    def Echo(value:Scores):Scores {
        var result <- clone(value)
        return <- result
    }
}
