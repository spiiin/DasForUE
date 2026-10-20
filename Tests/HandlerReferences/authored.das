options gen2
require ue_dsl
require ue_object_types
[ue_delegate]
struct RefQuery {signature:function<(value:int):int>}
[ue_multicast]
struct RefEvent {signature:function<(value:int):void>}
[ue_delegate]
struct RefMutation {def signature(var value:int&;@ue_out var accepted:bool&):int{}}
[ue_delegate]
struct RefObject {signature:function<(value:ActorRef):ActorRef>}
[ue_delegate]
struct RefArray {signature:function<(value:array<float>):array<float>>}
[ue_class(module_id="handlerrefs",Blueprintable)]
class ReferenceActor {
 @ue_property=(BlueprintReadWrite)
 Total:int=0
 @ue_property=(BlueprintReadWrite)
 HandlerName:string="Query"
 @ue_property=(BlueprintReadWrite)
 Stored:RefQuery
 @ue_property=(BlueprintAssignable)
 Changed:RefEvent
 def Query(value:int):int {return value+10}
 def Hit(value:int):void {self.Total+=value}
 def Mutate(var value:int&;@ue_out var accepted:bool&):int {value+=3;accepted=true;return value*2}
 [ue_function(BlueprintCallable)]
 def Exercise():int {
  let made=ue_bind(type<RefQuery>,ue_self_object(),self.Query)
  var rebound=RefQuery()
  rebound.BindUFunction(ue_self_object(),self.Query)
  var third=RefQuery()
  BindUFunction(third,ue_self_object(),self.Query)
  verify(third.Execute(3)==13,"plain bind form")
  let named=ue_bind(type<RefQuery>,ue_self_object(),self.HandlerName)
  verify(named.Execute(0)==10,"dynamic string field remains a value")
  var signal=RefEvent()
  signal.AddUFunction(ue_self_object(),self.Hit)
  signal.AddUFunction(ue_self_object(),self.Hit)
  signal.Broadcast(2)
  signal.RemoveUFunction(ue_self_object(),self.Hit)
  signal.Broadcast(100)
  AddUFunction(signal,ue_self_object(),self.Hit)
  RemoveUFunction(signal,ue_self_object(),self.Hit)
  verify(!signal.IsBound(),"plain add/remove forms")
  self.Changed.AddUFunction(ue_self_object(),self.Hit)
  self.Changed.Broadcast(3)
  self.Changed.RemoveUFunction(ue_self_object(),self.Hit)
  self.Changed.Broadcast(100)
  var n=1
  var accepted=false
  let mutation=ue_bind(type<RefMutation>,ue_self_object(),self.Mutate)
  verify(mutation.Execute(n,accepted)==8 && n==4 && accepted,"self ref/out")
  return made.Execute(1)+rebound.Execute(2)+self.Total
 }
 [ue_function(BlueprintCallable)]
 def WrongTarget(other:ActorRef):void {let d=ue_bind(type<RefQuery>,other,self.Query)}
 def Tick(delta:float):void {}
}
