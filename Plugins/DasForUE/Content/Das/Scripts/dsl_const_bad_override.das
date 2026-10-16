options gen2
require ue_inheritance_base
[ue_class(Blueprintable)]
class BadConst : BaseCounter {
    [ue_function(BlueprintPure,BlueprintEvent)]
    def override Query(input:int):int { return input }
}
