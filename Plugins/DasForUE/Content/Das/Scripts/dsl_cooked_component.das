options gen2
require ue_dsl
[ue_class(module_id="cookedcomponent",parent="ActorComponent",Blueprintable)]
class CookedBehavior {
    properties=ue_properties(BlueprintReadWrite) {
        var Bonus:int=10
        var Begins:int=0
        var Ticks:int=0
        var Ends:int=0
        var Signals:int=0
    }
    def BeginPlay():void { self.Begins+=1 }
    def Tick(delta:float):void { self.Ticks+=1 }
    def EndPlay(reason:int):void { self.Ends+=1 }
    [ue_event(source="owner",name="OnDasSignal")]
    def Signal(value:int):void { self.Signals+=value }
    [ue_function(BlueprintCallable)]
    def GetBonus(input:int):int { return self.Bonus+input }
}
