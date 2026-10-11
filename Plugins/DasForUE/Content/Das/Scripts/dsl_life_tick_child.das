options gen2
require ue_life_tick_base
[ue_class(module_id="lifetick",Blueprintable)]
class TickOnlyChild : TickOnlyBase {
    [ue_function(BlueprintCallable)]
    def Read():int { return self.Ticks }
}
