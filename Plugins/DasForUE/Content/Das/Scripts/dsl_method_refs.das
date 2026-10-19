options gen2
require ue_native_gameplay public

// Method references as callbacks; must compile.
[ue_class(Blueprintable)]
class MethodRefActor : Actor {
    state=ue_properties(BlueprintReadWrite) {
        var Pulses:int=0
    }
    def BeginPlay():void {
        ue_timer_start("pulse",1.0f,self.Pulse,true)
        ue_timer_next_tick("once",self.Pulse)
        ue_latent_delay("ready",0.5f,self.Pulse)
    }
    [ue_function(BlueprintCallable)]
    def Pulse():void { self.Pulses+=1 }
}
