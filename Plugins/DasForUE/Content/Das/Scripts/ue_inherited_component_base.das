options gen2
module ue_inherited_component_base public
require ue_dsl public
[ue_class(module_id="inheritedcomponents",parent="ActorComponent",Blueprintable)]
class InheritedComponentBase {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=7
    [ue_function(BlueprintCallable)]
    def BaseRead(input:int):int { return self.Bonus+input }
}
