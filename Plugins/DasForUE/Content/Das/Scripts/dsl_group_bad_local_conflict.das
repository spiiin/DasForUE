options gen2
require ue_dsl
[ue_class(module_id="badgroups",Blueprintable)]
class BadGroup {
fields=ue_properties(BlueprintReadWrite) {
var @ue_property=(BlueprintReadOnly) Score:int=0
}
}
