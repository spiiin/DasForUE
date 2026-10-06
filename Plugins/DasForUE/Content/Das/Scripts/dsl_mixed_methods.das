options gen2
require ue_dsl
[ue_class(module_id="mixed",Blueprintable)]
class MixedCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    health : int = 10
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    speed : float = 1.5
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    bScalar : bool = true

    [ue_function(BlueprintCallable)]
    def Compute(count:int; factor:float; apply:bool):float {
        if (apply) { self.health += count; self.speed += factor }
        return float(self.health) * self.speed
    }
    [ue_function(BlueprintCallable)]
    def IsReady(minimum:int; enabled:bool):bool { return enabled && self.health >= minimum }
    [ue_function(BlueprintCallable)]
    def ResetWith(amount:int; resetGain:bool):void {
        self.health = amount
        if (resetGain) { self.speed = 1.5 }
    }
    [ue_function(BlueprintCallable)]
    def ReadScore():int { return self.health }
    [ue_function(BlueprintCallable)]
    def BoolAsInt(value:bool):int { if (value) { return self.health }; return 0 }
}
