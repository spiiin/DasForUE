options gen2
require ue_text
[ue_class]
class BadTimer : Actor {
    def BeginPlay():void {

        ue_timer_next_tick("bad","Receive",uint8(1))
    }
    [ue_function(BlueprintCallable)]
    def Receive(var value:uint8&):void {}
}
