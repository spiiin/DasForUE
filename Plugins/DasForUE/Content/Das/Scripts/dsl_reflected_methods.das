options gen2
options gc
require ue_dsl
var remembered = ue_struct_value("/Script/DasRuntime.DasCallPayload")
[ue_class(module_id="reflected",Blueprintable)]
class StructBridge {
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasCallPayload",return_struct="/Script/DasRuntime.DasCallPayload")]
    def Edit(value:UEStructValue; amount:int):UEStructValue {
        var result = ue_struct_with_int(value,"Count",ue_struct_get_int(value,"Count")+amount)
        result = ue_struct_with_string(result,"Label",ue_struct_get_string(value,"Label")+" !")
        result = ue_struct_with_float(result,"Metrics.Ratio",ue_struct_get_float(value,"Metrics.Ratio")+0.5)
        return result
    }
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasCallPayload")]
    def Remember(value:UEStructValue):void { remembered = value }
    [ue_function(BlueprintCallable,return_struct="/Script/DasRuntime.DasCallPayload")]
    def Recall():UEStructValue { return remembered }
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasCallPayload")]
    def Count(value:UEStructValue):int { return ue_struct_get_int(value,"Count") }
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasCallPayload",return_struct="/Script/DasRuntime.DasCallMetrics")]
    def Wrong(value:UEStructValue):UEStructValue { return value }
}
