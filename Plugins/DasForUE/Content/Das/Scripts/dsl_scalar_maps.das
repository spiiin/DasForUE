options gen2
options gc
require ue_dsl

[ue_struct(module_id="scalarmap", BlueprintType)]
struct Stats {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights : table<string; double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Enabled : table<int; bool>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Labels : table<string; UENameValue>
}
[ue_class(module_id="scalarmap", type_registry="REGISTRY", Blueprintable)]
class StatsActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Config : Stats = Stats()
    [ue_function(BlueprintCallable)]
    def PutWeight(key:string; weight:double):void {
        var data <- clone(self.Config)
        data.Weights[key] = weight
        self.Config <- data
    }
    [ue_function(BlueprintPure)]
    def TotalWeight():double {
        var data <- clone(self.Config)
        var total = 0.0lf
        for (weight in values(data.Weights)) { total += weight }
        return total
    }
    [ue_function(BlueprintCallable)]
    def Echo(value:Stats):Stats {
        var result <- clone(value)
        return <- result
    }
}
