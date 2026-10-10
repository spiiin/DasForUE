options gen2
require ue_dsl
[ue_class(module_id="callbackprobe", Blueprintable)]
class LifecycleOnly {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Starts : int = 0
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Stops : int = 0
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Seconds : float = 0.0
    def BeginPlay() { self.Starts += 1 }
    def Tick(delta:int) { self.Seconds += delta }
    def EndPlay(reason:int) { self.Stops += 1 }
}
