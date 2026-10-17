options gen2
require ue_callback_value_signals
[ue_class]
class BadMulti : Actor {
[ue_function(BlueprintCallable)]
 def Receive(a:int;b:int):void {}
 def BeginPlay():void { ue_timer_next_tick("bad","Receive",1,2,3) }
}
