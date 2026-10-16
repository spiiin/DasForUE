options gen2
require ue_dsl
[ue_class(Blueprintable)]
class ConstCounter : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=10
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Values:array<int>
    def const ReadHelper(input:int):int { self.Score+=1;return self.Score+input }
    def Mutate():void { self.Score+=1 }
    [ue_function(BlueprintPure)]
    def const Query(input:int):int { return self.ReadHelper(input) }
    [ue_function(BlueprintCallable,BlueprintPure=false)]
    def const ReadWithExec():int { return self.Score }
    [ue_function(BlueprintPure)]
    def const Snapshot(@ue_out var value:int&):void { value=self.Score }
}
