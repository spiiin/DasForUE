options gen2
require ue_life_actor_base
[ue_class(module_id="lifefamily",Blueprintable)]
class ChildLifeActor : BaseLifeActor {
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Extension:SceneComponentRef
        var @Attach="Extension" Marker:SceneComponentRef
        var ExtraBehavior:ChildBehaviorRef
    }
    defaults=ue_defaults() {
        self.Extension.RelativeLocation=ue_vector(0.0lf,0.0lf,25.0lf)
        self.ExtraBehavior.Bonus=77
        self.Behavior.Bonus=55
        self.Pivot.bHiddenInGame=true
        self.Pivot.RelativeLocation=ue_vector(0.0lf,0.0lf,5.0lf)
    }

    @ue_property=(BlueprintReadWrite)
    ChildBegins:int=0
    @ue_property=(BlueprintReadWrite)
    ChildEnds:int=0
    [ue_lifecycle]
    def override BeginPlay():void {
        Super.BeginPlay()
        self.ChildBegins+=1
    }
    // Tick is inherited automatically.
    [ue_lifecycle]
    def override EndPlay(reason:int):void {
        self.ChildEnds+=1
        Super.EndPlay(reason)
    }
    [ue_function(BlueprintCallable)]
    def Probe(input:int):int {
        return self.BaseBegins+self.Behavior.BaseBegins+self.Behavior.ChildBegins
    }
}
