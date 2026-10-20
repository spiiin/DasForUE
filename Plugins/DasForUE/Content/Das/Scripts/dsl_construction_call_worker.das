options gen2
require ue_dsl
require ue_probe_actor
var calls:int=0
[ue_class(module_id="constructioncalls",Blueprintable)]
class ConstructionCallWorker : ActorComponent {
    @ue_property=(BlueprintReadWrite)
    FailureMode:int=0
    @ue_property=(BlueprintReadWrite)
    InitCount:int=0
    @ue_property=(BlueprintReadWrite)
    Signals:int=0
    def Initialize():void { self.InitCount+=1 }
    [ue_event(source=owner,source_type=ProbeActorRef,name="OnDasSignal")]
    def OnSignal(value:int):void { self.Signals+=value }
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int {
        if(self.FailureMode==1) { ue_timer_next_tick("forbidden","Pulse") }
        if(self.FailureMode==2) { panic("construction nested method failure") }
        calls+=1
        return input+5+calls
    }
    [ue_function(BlueprintCallable)]
    def Pulse():void {}
}
