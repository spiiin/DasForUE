options gen2
require ue_inheritance_base
[ue_class(module_id="inheritance",Blueprintable)]
class BadFinal : BaseCounter {
    [ue_function(BlueprintCallable)]
    def override ReadScore():int { return 0 }
}
