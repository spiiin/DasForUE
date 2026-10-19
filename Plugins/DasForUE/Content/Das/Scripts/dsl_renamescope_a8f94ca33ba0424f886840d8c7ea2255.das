options gen2
options gc
require ue_dsl
[ue_class(module_id="rename",Blueprintable)]
class RenameFields {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="rename.RenameFields.Score"
    Score : int = 5
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_id="rename.RenameFields.Payload"
    Payload : UEStructValue = ue_struct_default("/Script/DasRuntime.DasCallPayload","(Count=10,Label=\"Initial\")")
    [ue_function(BlueprintCallable)]
    def Add(amount:int):int {
        self.Score += amount
        self.Payload = ue_struct_with_int(self.Payload,"Count",ue_struct_get_int(self.Payload,"Count")+amount)
        return self.Score + ue_struct_get_int(self.Payload,"Count")
    }
}
