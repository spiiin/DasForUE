options gen2
module ue_life_behavior_base public
require ue_dsl public
require ue_probe_actor
[ue_class(module_id="lifefamily",Blueprintable)]
class BaseBehavior : ActorComponent {
    @ue_property=(BlueprintReadWrite)
    BaseSignals:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Bonus:int=10
    @ue_property=(BlueprintReadWrite)
    BaseBegins:int=0
    @ue_property=(BlueprintReadWrite)
    BaseTicks:int=0
    @ue_property=(BlueprintReadWrite)
    BaseEnds:int=0
    @ue_property=(BlueprintReadWrite)
    ConstructionValue:int=0
    @ue_property=(BlueprintReadWrite)
    ConstructionTrace:int=0
    def ConstructionScript():void {
        self.ConstructionValue=self.Bonus*2
        self.ConstructionTrace=1
    }
    [ue_event(source=owner,source_type=ProbeActorRef,name="OnDasSignal")]
    def ParentSignal(value:int):void { self.BaseSignals+=value }
    [ue_lifecycle]
    def BeginPlay():void { self.BaseBegins+=1 }
    [ue_lifecycle]
    def Tick(delta:float):void { self.BaseTicks+=1 }
    [ue_lifecycle]
    def EndPlay(reason:int):void { self.BaseEnds+=1 }
    [ue_function(BlueprintCallable)]
    def Status():int { return self.BaseBegins+self.BaseEnds }
}
