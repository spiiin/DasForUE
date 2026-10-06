options gen2
require ue_dsl
[ue_class(module_id="widen",Blueprintable)]
class WidenFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score : int = 5
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Rate : float = 1.5
    [ue_function(BlueprintCallable)]
    def ReadScore():double {
        return double(self.Score)
    }
    [ue_function(BlueprintCallable)]
    def ReadRate():double {
        return double(self.Rate)
    }
}
