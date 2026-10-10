options gen2
require ue_dsl

[ue_class(module_id="timerargs",Blueprintable)]
class TimerArgumentActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Enabled:bool=false
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Message:string=""
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Position:UEVector=ue_vector(0.0lf,0.0lf,0.0lf)

    def BeginPlay():void {
        ue_timer_start("score",1.0,"AddScore",true,5)
        ue_timer_start("enable",0.5,"SetEnabled",false,true)
        ue_timer_start("message",0.5,"SetMessage",false,"Timer 世界")
        ue_timer_start("position",0.5,"SetPosition",false,ue_vector(1.0lf,2.0lf,3.0lf))
    }
    [ue_function(BlueprintCallable)]
    def AddScore(amount:int):void { self.Score += amount }
    [ue_function(BlueprintCallable)]
    def SetEnabled(value:bool):void { self.Enabled = value }
    [ue_function(BlueprintCallable)]
    def SetMessage(value:string):void { self.Message = value }
    [ue_function(BlueprintCallable)]
    def SetPosition(value:UEVector):void { self.Position = value }
}
