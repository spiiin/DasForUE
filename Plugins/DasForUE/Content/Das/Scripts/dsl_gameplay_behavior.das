options gen2
require ue_gameplay_refs

[ue_class(module_id="gameplaydemo",Blueprintable)]
class GameplayBehavior : ActorComponent {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        // Время работы компонента в секундах.
        var Seconds:float=0.0
        var SignalCount:int=0
    }

    def BeginPlay():void {
        ue_timer_start("score",1.0,"Pulse",true,1)
    }
    def Tick(delta:float):void { self.Seconds += delta }
    [ue_function(BlueprintCallable)]
    def Pulse(amount:int):void {
        let owner = ue_owner(type<GameplayActorRef>)
        owner.Score += amount
    }
    [ue_event(source="owner",name="OnDasSignal")]
    def OnSignal(value:int):void {
        self.SignalCount += 1
        let owner = ue_owner(type<GameplayActorRef>)
        owner.Score += value
    }
    [ue_function(BlueprintCallable)]
    def StopScore():bool { return ue_timer_cancel("score") }
    [ue_function(BlueprintCallable)]
    def StartScore():void { ue_timer_start("score",1.0,"Pulse",true,1) }
}
