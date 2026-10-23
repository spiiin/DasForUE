options gen2
require ue_dsl
[ue_class(module_id="subsystemdemo", Blueprintable)]
class SessionService : GameInstanceSubsystem {
    @ue_property=(BlueprintReadWrite)
    Starts:int = 0
    @ue_property=(BlueprintReadWrite)
    Stops:int = 0
    @ue_property=(BlueprintReadWrite)
    Score:int = 0
    def Initialize() { self.Starts += 1 }
    def Deinitialize() { self.Stops += 1 }
    [ue_function(BlueprintCallable)]
    def Advance():int { self.Score += 1; return self.Score }
    [ue_function(BlueprintCallable)]
    def ArmTimer():void { ue_timer_start("maintenance",60.0f,$ { self.Score = self.Score+1000 },false) }
}
