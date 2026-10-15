options gen2
require ue_dsl
require dsl_inheritance_child
[ue_class(module_id="inheritance",Blueprintable)]
class GrandCounter : ChildCounter {
    @ue_property=(BlueprintReadWrite)
    GrandBonus:int=5
    def override Offset(value:int):int { return Super.Offset(value)+self.GrandBonus }
    [ue_function(BlueprintCallable)]
    def LocalProbe(input:int):int { return self.Template(input) }
}
