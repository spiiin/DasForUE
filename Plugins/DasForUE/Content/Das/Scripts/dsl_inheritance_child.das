options gen2
module dsl_inheritance_child public
require ue_inheritance_base public
[ue_class(module_id="inheritance",Blueprintable)]
class ChildCounter : BaseCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=3
    def override ConstructionScript():void {
        Super.ConstructionScript()
        self.ConstructionValue+=self.Bonus
        self.ConstructionTrace=self.ConstructionTrace*10+2
    }
    def override Offset(value:int):int { return Super.Offset(value)+self.Bonus }
    [ue_function(BlueprintPure,BlueprintEvent)]
    def override const Query(input:int):int { return Super.Query(input)+self.Bonus }
    [ue_function(BlueprintCallable)]
    def HelperProbe(input:int):int { return self.Template(input) }
    [ue_function(BlueprintCallable,BlueprintEvent)]
    def override Compute(input:int):int { return Super.Compute(input)+self.Bonus }
    [ue_function(BlueprintCallable)]
    def Probe(input:int):int { return self.Compute(input)+self.ReadScore() }
}
