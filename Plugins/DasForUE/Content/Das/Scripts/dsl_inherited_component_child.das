options gen2
require ue_inherited_component_base public
[ue_class(module_id="inheritedcomponents",Blueprintable)]
class InheritedComponentChild : InheritedComponentBase {
    defaults=ue_defaults() { self.Bonus=10 }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Bonus+input }
}
