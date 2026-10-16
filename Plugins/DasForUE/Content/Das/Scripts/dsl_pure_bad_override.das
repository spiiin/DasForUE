options gen2
require ue_inheritance_base
[ue_class(module_id="purechecks",Blueprintable)]
class BadPure : BaseCounter {
    [ue_function(BlueprintCallable,BlueprintEvent)]
    def override Query(input:int):int { return input }
}
