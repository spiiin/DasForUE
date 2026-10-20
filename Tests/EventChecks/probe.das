options gen2
require ue_dsl
require ue_object_types
[ue_multicast]
struct ChangedSignal {signature:function<(left:int;right:float):void>}
[ue_class(module_id="eventchecks",Blueprintable)]
class EventActor {
 @ue_property=(BlueprintReadWrite)
 Total:int=0
 @ue_property=(BlueprintAssignable)
 Changed:ChangedSignal
 def BindEvents():void {self.OnDasSignal += self.OnSignal;self.Changed += self.OnChanged}
 def OnSignal(value:int):void {self.Total+=value}
 def OnChanged(left:int;right:float):void {self.Total+=left+int(right)}
 [ue_event(source=self,name=OnDasFloatSignal)]
 def OnFloat(value:float):void {self.Total+=int(value)}
 [ue_function(BlueprintCallable)]
 def Fire():void {self.Changed.Broadcast(2,3.0f)}
 def Tick(delta:float):void {} // RELOAD_MARKER
}
