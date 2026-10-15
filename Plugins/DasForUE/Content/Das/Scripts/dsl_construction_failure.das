options gen2
require ue_inheritance_base
[ue_class(module_id="construction",Blueprintable)]
class ConstructionFailure : BaseCounter {
    def override ConstructionScript():void {
        self.ConstructionTrace+=1
        ue_timer_next_tick("forbidden","Pulse")
    }
    [ue_function(BlueprintCallable)]
    def Pulse():void {}
}
