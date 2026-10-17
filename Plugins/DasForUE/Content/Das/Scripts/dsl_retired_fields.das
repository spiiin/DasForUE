options gen2
options gc
require ue_dsl
[ue_class(module_id="retire",Blueprintable)]
class RetireFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Marker : int = 1
    //RetireBegin

    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score : int = 5
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Payload : UEStructValue = ue_struct_default("/Script/DasRuntime.DasCallPayload","(Count=10,Label=\"Initial\")")
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Text : string = "Привет"
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Key : UENameValue = ue_name_value("Old_7")
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Enabled : bool = true
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Precise : double = 1.23456789012345lf
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Rate : float = 1.5
    //RetireEnd

    [ue_function(BlueprintCallable)]
    def Read():int { return self.Marker }
}
