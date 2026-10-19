options gen2
require ue_native_gameplay public

// Negative fixture: the timer handler must be a method of the class.
[ue_class(Blueprintable)]
class MethodRefBadActor : Actor {
    state=ue_properties(BlueprintReadWrite) {
        var Pulses:int=0
    }
    def BeginPlay():void { ue_timer_start("pulse",1.0f,self.Pulses,true) }
}
