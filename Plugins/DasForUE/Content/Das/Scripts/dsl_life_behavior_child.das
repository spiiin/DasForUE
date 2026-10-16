options gen2
require ue_life_behavior_base
[ue_class(module_id="lifefamily",Blueprintable)]
class ChildBehavior : BaseBehavior {
    @ue_property=(BlueprintReadWrite)
    ChildBegins:int=0
    @ue_property=(BlueprintReadWrite)
    ChildTicks:int=0
    @ue_property=(BlueprintReadWrite)
    ChildEnds:int=0
    @ue_property=(BlueprintReadWrite)
    ChildSignals:int=0
    def override ConstructionScript():void {
        Super.ConstructionScript()
        self.ConstructionValue+=3
        self.ConstructionTrace=self.ConstructionTrace*10+2
    }
    [ue_event(source="owner",name="OnDasSignal")]
    def ChildSignal(value:int):void { self.ChildSignals+=value*10 }
    [ue_lifecycle]
    def override BeginPlay():void { Super.BeginPlay();self.ChildBegins+=1 }
    [ue_lifecycle]
    def override Tick(delta:float):void { Super.Tick(delta);self.ChildTicks+=1 }
    [ue_lifecycle]
    def override EndPlay(reason:int):void { self.ChildEnds+=1;Super.EndPlay(reason) }
    [ue_function(BlueprintPure)]
    def const ChildStatus():int { return self.ChildBegins+self.ChildEnds }
    [ue_function(BlueprintPure)]
    def const Snapshot(@ue_out var value:int&):void { value=self.Bonus }
}
