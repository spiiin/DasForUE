options gen2
require ue_enhanced_input
[ue_class(module_id="inlinecallbacks",Blueprintable)]
class InlineActor {
    @ue_property=(BlueprintReadWrite)
    TimerHits:int=0
    @ue_property=(BlueprintReadWrite)
    InputHits:int=0
    @ue_property=(BlueprintReadWrite)
    Action:InputActionRef
    def Tick(delta:float):void {}
    [ue_function(BlueprintCallable)]
    def Start():void {
        ue_timer_start("repeat",0.05f,$ { self.TimerHits+=1 },true)
        ue_enhanced_bind(self.Action,TriggerEvent.Triggered,$(value:float) { self.InputHits+=int(value) })
    }
    [ue_function(BlueprintCallable)]
    def StartPayload():void {
        let amount=3
        ue_timer_start("repeat",0.05f,$(payload:int) { self.TimerHits+=payload },true,amount)
    }
    [ue_function(BlueprintCallable)]
    def StartMultiPayload():void {
        let amount=3
        ue_timer_start("repeat",0.05f,$(left:int;right:int) { self.TimerHits+=left+right },true,amount,2)
    }
    [ue_function(BlueprintCallable)]
    def StartNamed():void { ue_timer_start("legacy",99.0f,self.Named__ue_cb_plain,true) }
    [ue_function(BlueprintCallable)]
    def Named__ue_cb_plain():void { self.TimerHits+=9999 }
    [ue_function(BlueprintCallable)]
    def Read():int { return self.TimerHits } // RELOAD_MARKER
}
