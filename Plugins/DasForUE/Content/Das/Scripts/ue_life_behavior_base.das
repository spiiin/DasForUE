options gen2
module ue_life_behavior_base public
require ue_dsl public
[ue_class(module_id="lifefamily",parent="ActorComponent",Blueprintable)]
class BaseBehavior {
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
    [ue_event(source="owner",name="OnDasSignal")]
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
