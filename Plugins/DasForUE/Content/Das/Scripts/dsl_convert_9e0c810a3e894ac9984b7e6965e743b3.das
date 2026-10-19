options gen2
require ue_dsl
[ue_class(module_id="widen",Blueprintable)]
class WidenFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score : float = 5.0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Rate : float = 2.0
    [ue_function(BlueprintCallable)]
    def ReadScore():double {
        return double(self.Score)
    }
    [ue_function(BlueprintCallable)]
    def ReadRate():double {
        return double(self.Rate)
    }
}
