options gen2
options gc
require ue_dsl

[ue_class(module_id="nexttick",Blueprintable)]
class NextTickActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Values:array<int>

    def BeginPlay():void {
        ue_timer_next_tick("refresh","Refresh")
        ue_timer_next_tick("add","AddScore",5)
        ue_timer_next_tick("add","AddScore",10) // Replaces the previous named request.
        var values:array<int>
        push(values,7)
        ue_timer_next_tick("values","ApplyValues",values)
    }
    [ue_function(BlueprintCallable)]
    def Refresh():void { self.Score += 1 }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score += amount }
    [ue_function(BlueprintCallable)]
    def ApplyValues(values:array<int>):void { self.Values := values }
    [ue_function(BlueprintCallable)]
    def CancelScheduled():int { return ue_timer_cancel_all() }
    [ue_function(BlueprintCallable)]
    def Paused():bool { return ue_timer_is_paused("add") }
    [ue_function(BlueprintCallable)]
    def Elapsed():float { return ue_timer_elapsed("add") }
}
