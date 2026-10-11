options gen2
require ue_dsl

[ue_class(module_id="groups",Blueprintable)]
class GroupCounter {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite, Category="State") {
        var Score:int=0
        var Speed:double=100.0lf
        var @Category="Special" @ToolTip="Individual tooltip" Title:string="Counter"
    }
    status = ue_properties(BlueprintReadOnly) {
        var Ready:bool=false
    }
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Score+input }
}
