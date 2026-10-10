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
    [ue_lifecycle, ue_lifecycle]
    def BeginPlay() { self.Starts += 1 }
    [ue_lifecycle]
    def Tick(delta:float) { self.Seconds += delta }
    [ue_lifecycle]
    def EndPlay(reason:int) { self.Stops += 1 }
}
