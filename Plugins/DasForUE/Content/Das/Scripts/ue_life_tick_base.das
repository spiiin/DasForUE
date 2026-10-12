options gen2
module ue_life_tick_base public
require ue_dsl public
[ue_class(module_id="lifetick",parent="Actor",Blueprintable)]
class TickOnlyBase {
    @ue_property=(BlueprintReadWrite)
    BaseSignals:int=0
    @ue_property=(BlueprintReadWrite)
    Ticks:int=0
    [ue_event(source="self",name="OnDasSignal")]
    def ParentSignal(value:int):void { self.BaseSignals+=value }
    [ue_lifecycle]
    def Tick(delta:float):void { self.Ticks+=1 }
}
