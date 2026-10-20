options gen2
require ue_enhanced_input
require ue_gameplay_services
[ue_class(module_id="hiddencallbacks",Blueprintable)]
class HiddenActor {
 @ue_property=(BlueprintReadWrite)
 TimerHits:int=0
 @ue_property=(BlueprintReadWrite)
 InputHits:int=0
 @ue_property=(BlueprintReadWrite)
 Action:InputActionRef
 def Tick(delta:float):void {}
 [ue_function(BlueprintCallable)]
 def Start():void {
  ue_timer_start("repeat",0.05f,self.Pulse,true)
  ue_enhanced_bind(self.Action,TriggerEvent.Triggered,self.Input)
 }
 def Pulse():void {self.TimerHits+=1}
 [ue_function]
 def Input(value:float):void {self.InputHits+=int(value)}
 [ue_function(BlueprintCallable)]
 def StartPayload():void {ue_timer_next_tick("payload",self.Payload,3)}
 def Payload(value:int):void {self.TimerHits+=value}
 [ue_function(BlueprintCallable)]
 def StartMulti():void {ue_timer_next_tick("multi",self.Multi,3,4)}
 def Multi(left:int;right:int):void {self.TimerHits+=left+right}
 [ue_function(BlueprintCallable)]
 def BindButton(button:ButtonRef):void {ue_widget_bind_click(button,self.Clicked)}
 def Clicked():void {self.TimerHits+=10}
 [ue_function(BlueprintCallable)]
 def StartInline():void {ue_timer_next_tick("inline",$ {self.TimerHits+=20})}
 [ue_function(BlueprintCallable)]
 def StartPublic():void {ue_timer_next_tick("public",self.PublicHandler)}
 [ue_function(BlueprintCallable)]
 def PublicHandler():void {self.TimerHits+=30}
 [ue_function(BlueprintCallable)]
 def Direct():void {self.Pulse()}
 def Unused():int {return 99}
 [ue_function(BlueprintCallable)]
 def Read():int {return self.TimerHits} // RELOAD_MARKER
}
