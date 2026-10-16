options gen2
require ue_dsl
[ue_class(module_id="purechecks",Blueprintable)]
class BadPure {
    [ue_function(BlueprintPure,BlueprintCallable=false)]
    def Read():int { return 1 }
}
