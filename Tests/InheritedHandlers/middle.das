options gen2
require @BASE@ public
require ue_gameplay_services
[ue_class(module_id="handlerinherit",Blueprintable)]
class CallbackMiddle : CallbackBase {
 [ue_function(BlueprintCallable)]
 def StartChild():void {
  ue_timer_next_tick("child",self.Pulse)
  ue_timer_next_tick("bump",self.Bump)
  ue_timer_next_tick("public",self.PublicPulse)
  ue_timer_next_tick("inherited-visible",self.InheritedVisible)
  ue_timer_next_tick("payload",self.Payload,3)
  ue_timer_next_tick("multi",self.Multi,4,5)
 }
 [ue_function(BlueprintCallable)]
 def ChildQuery():int {
  var q=InheritQuery()
  q.BindUFunction(ue_self_object(),self.Query)
  return q.Execute(2)
 }
 [ue_function(BlueprintCallable)]
 def ChildEvents():void {
  var e=InheritEvent()
  e.AddUFunction(ue_self_object(),self.Hit)
  e.Broadcast(2)
  e.RemoveUFunction(ue_self_object(),self.Hit)
  e.Broadcast(100)
 }
 [ue_function(BlueprintCallable)]
 def BindButton(button:ButtonRef):void {ue_widget_bind_click(button,self.Clicked)}
}
