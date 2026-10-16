options gen2
module dsl_inherited_defaults_leaf public
require dsl_inherited_defaults public
[ue_class(module_id="inheriteddefaults",Blueprintable)]
class DefaultsLeaf : DefaultsChild {
    defaults=ue_defaults() { self.Score=30 }
    [ue_function(BlueprintCallable)]
    def LeafRead(input:int):int { return self.Score+input }
}
