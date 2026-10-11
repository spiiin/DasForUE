options gen2
require ue_inheritance_base
[ue_class(module_id="inheritance",Blueprintable)]
class ChildCounter : BaseCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=3
    [ue_function(BlueprintCallable,BlueprintEvent)]
    def override Compute(input:int):int { return Super.Compute(input)+self.Bonus }
    [ue_function(BlueprintCallable)]
    def Probe(input:int):int { return self.Compute(input)+self.ReadScore() }
}
