options gen2
options gc
require ue_dsl
[ue_class(module_id="rename",Blueprintable)]
class RenameFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="rename.RenameFields.Score"
    Total : int = 5
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="rename.RenameFields.Payload"
    Data : UEStructValue = ue_struct_default("/Script/DasRuntime.DasCallPayload","(Count=10,Label=\"Initial\")")
    [ue_function(BlueprintCallable)]
    def Add(amount:int):int {
        self.Total += amount
        self.Data = ue_struct_with_int(self.Data,"Count",ue_struct_get_int(self.Data,"Count")+amount)
        return self.Total + ue_struct_get_int(self.Data,"Count")
    }
}
