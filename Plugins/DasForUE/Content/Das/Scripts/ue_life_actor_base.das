options gen2
module ue_life_actor_base public
require ue_dsl public
require ue_scene_components public
[ue_object_ref(script="dsl_life_behavior_child.das",type="ChildBehavior")]
struct ChildBehaviorRef { handle:UObjectHandle }

[ue_class(module_id="lifefamily",Blueprintable)]
class BaseLifeActor : Actor {
    @ue_property=(BlueprintReadWrite)
    BaseSignals:int=0
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef
    @ue_component
    Behavior:ChildBehaviorRef

    @ue_property=(BlueprintReadWrite)
    BaseBegins:int=0
    @ue_property=(BlueprintReadWrite)
    BaseTicks:int=0
    @ue_property=(BlueprintReadWrite)
    BaseEnds:int=0
    @ue_property=(BlueprintReadWrite)
    ConstructionObserved:int=0
    def ConstructionScript():void {
        self.ConstructionObserved=self.Behavior.ConstructionValue
    }
    defaults=ue_defaults() { self.Behavior.Bonus=42 }
    [ue_event(source="self",name="OnDasSignal")]
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
