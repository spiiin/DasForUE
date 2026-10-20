options gen2
require ue_dsl

[ue_class(module_id="timers", Blueprintable)]
class TimerCounter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score:int=0

    def BeginPlay():void {
        ue_timer_start("score",1.0,self.Pulse,true)
    }
    def Pulse():void {
        self.Score += 1
    }
    [ue_function(BlueprintCallable)]
    def Start():void {
        ue_timer_start("score",1.0,self.Pulse,true)
    }
    [ue_function(BlueprintCallable)]
    def Stop():bool { return ue_timer_cancel("score") }
    [ue_function(BlueprintCallable)]
    def Pause():bool { return ue_timer_pause("score") }
    [ue_function(BlueprintCallable)]
    def Resume():bool { return ue_timer_resume("score") }
    [ue_function(BlueprintCallable)]
    def Active():bool { return ue_timer_is_active("score") }
    [ue_function(BlueprintCallable)]
    def Remaining():float { return ue_timer_remaining("score") }
}
