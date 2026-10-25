options gen2
require ue_probe_actor
require ue_scene_components
[ue_native_object(path="/Script/Engine.ActorComponent")]
struct EventComponentRef {handle:UObjectHandle}
[ue_multicast]
struct DynamicChanged {signature:function<(left:int;right:float):void>}
[ue_class(module_id="dynamicevents",Blueprintable,parent="/Script/DasRuntime.DasProbeActor")]
class DynamicActor {
 components=ue_components(){var Scene:SceneComponentRef}
 @ue_property=(BlueprintReadWrite)
 Total:int=0
 @ue_property=(BlueprintReadWrite)
 Delta:int=3
 @ue_property=(BlueprintAssignable)
 Changed:DynamicChanged
 [ue_function(BlueprintCallable)]
 def Start(enabled:bool):void {
  if(enabled){
   self.OnDasSignal += self.OnSignal
   self.Changed += self.OnChanged
   self.Scene.OnComponentActivated += self.OnActivated
  }
 }
 [ue_function(BlueprintCallable)]
 def StartOther():void {self.OnDasSignal += self.OnOther}
 [ue_function(BlueprintCallable)]
 def Stop():void {
  self.OnDasSignal -= self.OnSignal
  self.Changed -= self.OnChanged
  self.Scene.OnComponentActivated -= self.OnActivated
 }
 def OnSignal(value:int):void {self.Total+=value}
 def OnOther(value:int):void {self.Total+=value*10}
 def OnChanged(left:int;right:float):void {self.Total+=left+int(right)}
 def OnActivated(component:EventComponentRef;reset:bool):void {self.Total+=100}
 [ue_function(BlueprintCallable)]
 def Fire():void {self.Changed.Broadcast(2,3.0f)}
 [ue_function(BlueprintCallable)]
 def Arithmetic():void {self.Total+=self.Delta}
 def EndPlay(reason:int):void {self.Stop()}
 def Tick(delta:float):void {} // DYNAMIC_RELOAD_MARKER
}
