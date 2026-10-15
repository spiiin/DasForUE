options gen2
module ue_inheritance_base public
require ue_dsl public
[ue_class(module_id="inheritance",Blueprintable)]
class BaseCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=10
    @ue_property=(BlueprintReadWrite)
    ConstructionValue:int=0
    @ue_property=(BlueprintReadWrite)
    ConstructionTrace:int=0
    def ConstructionScript():void {
        self.ConstructionValue=self.Score*2
        self.ConstructionTrace=1
    }
    def Offset(value:int):int { self.Score+=value;return self.Score }
    def Template(value:int):int { return self.Offset(value)+1 }
    [ue_function(BlueprintCallable,BlueprintEvent)]
    def Compute(input:int):int { self.Score+=input;return self.Score }
    [ue_function(BlueprintCallable)]
    def ReadScore():int { return self.Score }
}
