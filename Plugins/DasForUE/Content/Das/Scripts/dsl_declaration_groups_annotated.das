options gen2
require ue_dsl
[ue_class(module_id="groups",Blueprintable)]
class GroupCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @Category="State"
    Score:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @Category="State"
    Speed:double=100.0lf
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @Category="Special"
    @ToolTip="Individual tooltip"
    Title:string="Counter"
    @ue_property=(BlueprintReadOnly)
    Ready:bool=false
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score+input }
}
