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
var Total:int=0
var Signal=RefEvent()
var Saved=RefQuery()
def FreeQuery(value:int):int {return value+10}
def Hit(value:int):void {Total+=value}
def Pulse():void {Total+=1}
def Mutate(var value:int&;@ue_out var accepted:bool&):int {value+=3;accepted=true;return value*2}
def EchoObject(value:ActorRef):ActorRef {return value}
def EchoArray(value:array<float>):array<float> {var result<-clone(value);return <-result}
def Wrong(value:float):int {return int(value)}
[export]
def main(input:int):int {
 let made=ue_bind(type<RefQuery>,ue_self_object(),@@FreeQuery)
 Saved.BindUFunction(ue_self_object(),@@FreeQuery)
 verify(made.Execute(1)==11 && Saved.Execute(2)==12,"free factory and method bind")
 var third=RefQuery()
 BindUFunction(third,ue_self_object(),@@FreeQuery)
 verify(third.Execute(3)==13,"free plain bind form")
 Signal.AddUFunction(ue_self_object(),@@Hit)
 Signal.AddUFunction(ue_self_object(),@@Hit)
 Signal.Broadcast(2)
 Signal.RemoveUFunction(ue_self_object(),@@Hit)
 Signal.Broadcast(100)
 AddUFunction(Signal,ue_self_object(),@@Hit)
 RemoveUFunction(Signal,ue_self_object(),@@Hit)
 verify(!Signal.IsBound(),"free plain add/remove forms")
 verify(Total==2,"free add/remove identity")
 var n=1
 var accepted=false
 let mutation=ue_bind(type<RefMutation>,ue_self_object(),@@Mutate)
 let result=mutation.Execute(n,accepted)
 verify(result==8,"free ref/out result")
 verify(n==4,"free ref/out inout")
 verify(accepted,"free ref/out out")
 let echo=ue_bind(type<RefObject>,ue_self_object(),@@EchoObject)
 verify(ue_is_valid(echo.Execute(ue_ref(type<ActorRef>,ue_self_object()))),"typed object ABI")
 var values:array<float>
 push(values,1.0f)
 push(values,2.0f)
 let arrayEcho=ue_bind(type<RefArray>,ue_self_object(),@@EchoArray)
 let copied<-arrayEcho.Execute(values)
 verify(length(copied)==2 && copied[1]==2.0f,"array ABI")
 let native=RefEvent_from_property(ue_self_object(),"OnDasSignal")
 native.AddUFunction(ue_self_object(),@@Hit)
 native.AddUFunction(ue_self_object(),@@Hit)
 return 42
}
[export]
def read():int {return Total}
[export]
def stop():void {let e=RefEvent_from_property(ue_self_object(),"OnDasSignal");e.RemoveUFunction(ue_self_object(),@@Hit)}
[export]
def timer():void {ue_timer_next_tick("free-timer",@@Pulse)}
[export]
def tick(delta:float):void {} // RELOAD_MARKER
