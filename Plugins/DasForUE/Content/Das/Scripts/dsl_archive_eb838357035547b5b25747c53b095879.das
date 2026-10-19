options gen2
options gc
require ue_dsl
require ue_text
[ue_class(module_id="retire",Blueprintable)]
class RetireFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Marker : int = 1

    [ue_function(BlueprintCallable)]
    def Read():int { return self.Marker }
}
