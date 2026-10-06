options gen2
options gc
require ue_dsl
[ue_class(module_id="structfields",Blueprintable)]
class StructFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Payload : UEStructValue = ue_struct_default("/Script/DasRuntime.DasCallPayload", "(Metrics=(Ratio=1.5),Count=10,Label=\"Initial\",bEnabled=True,Weight=2.25)")
    @ue_property=(EditAnywhere,BlueprintReadOnly)
    Mode : UEStructValue = ue_struct_default("/Script/DasRuntime.DasNamedMode", "(Mode=Active,Key=Initial_7)")
    [ue_function(BlueprintCallable)]
    def Add(amount:int):int {
        let next = ue_struct_get_int(self.Payload,"Count") + amount
        self.Payload = ue_struct_with_int(self.Payload,"Count",next)
        return next
    }
    [ue_function(BlueprintCallable,return_struct="/Script/DasRuntime.DasCallPayload")]
    def Read():UEStructValue {
        return self.Payload
    }
    [ue_function(BlueprintCallable)]
    def Wrong():void {
        self.Payload = ue_struct_value("/Script/DasRuntime.DasCallMetrics")
    }
}
