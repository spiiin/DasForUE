options gen2
require ue_text
[ue_class]
class BadTimer : Actor {
    def BeginPlay():void {
        var values:array<array<int>>
        ue_timer_next_tick("bad","Receive",values)
    }
    [ue_function(BlueprintCallable)]
    def Receive(value:array<array<int>>):void {}
}
