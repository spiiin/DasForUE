options gen2
require ue_enhanced_input
require ue_gameplay_services
[ue_delegate]
struct InheritQuery {signature:function<(value:int):int>}
[ue_delegate]
struct InheritMutation {def signature(var value:int&;@ue_out var accepted:bool&):int{}}
[ue_multicast]
struct InheritEvent {signature:function<(value:int):void>}
[ue_class(module_id="handlerinherit",Blueprintable)]
class CallbackBase {
 @ue_property=(BlueprintReadWrite)
 Pulses:int=0
 @ue_property=(BlueprintReadWrite)
 Bumps:int=0
 @ue_property=(BlueprintReadWrite)
 PayloadTotal:int=0
 @ue_property=(BlueprintReadWrite)
 InputTotal:int=0
 @ue_property=(BlueprintReadWrite)
 PublicHits:int=0
 @ue_property=(BlueprintReadWrite)
 HitTotal:int=0
 @ue_property=(BlueprintReadWrite)
 Action:InputActionRef
 @ue_property=(BlueprintAssignable)
 Changed:InheritEvent
 def Pulse():void {self.Pulses+=1}
 def Bump():void {self.Bumps+=2}
 def Payload(value:int):void {self.PayloadTotal+=value}
 def Multi(left:int;right:int):void {self.PayloadTotal+=left+right}
 [ue_function(BlueprintEvent)]
 def PublicCallback():void {self.PublicHits+=5}
 [ue_function(BlueprintEvent)]
 def InheritedVisible():void {self.PublicHits+=7}
 [ue_function(BlueprintCallable)]
 def PublicPulse():void {self.Bumps+=7}
 def Clicked():void {self.Bumps+=100}
 def Input(value:float):void {self.InputTotal+=int(value)}
 def Query(value:int):int {return value+10}
 def Mutate(var value:int&;@ue_out var accepted:bool&):int {value+=3;accepted=true;return value*2}
 [ue_function(BlueprintCallable)]
 def BaseMutation():int {
  var n=1
  var accepted=false
  let d=ue_bind(type<InheritMutation>,ue_self_object(),self.Mutate)
  let result=d.Execute(n,accepted)
  verify(n==5 && accepted,"inherited ref/out")
  return result
 }
 def Hit(value:int):void {self.HitTotal+=value}
 [ue_function(BlueprintCallable)]
 def StartBase():void {
  ue_timer_next_tick("base",self.Pulse)
  ue_timer_next_tick("visible",self.PublicCallback)
  ue_enhanced_bind(self.Action,TriggerEvent.Triggered,self.Input)
 }
 [ue_function(BlueprintCallable)]
 def BaseQuery():int {
  let q=ue_bind(type<InheritQuery>,ue_self_object(),self.Query)
  return q.Execute(1)
 }
 [ue_function(BlueprintCallable)]
 def BindBase():void {self.Changed.AddUFunction(ue_self_object(),self.Hit)}
 [ue_function(BlueprintCallable)]
 def UnbindBase():void {self.Changed.RemoveUFunction(ue_self_object(),self.Hit)}
 def Tick(delta:float):void {}
}
