options gen2
require ue_dsl
[ue_class(module_id="computedcomponents",Blueprintable)]
class ComputedComponent : ActorComponent {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=6*7
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Bonus+input }
}
