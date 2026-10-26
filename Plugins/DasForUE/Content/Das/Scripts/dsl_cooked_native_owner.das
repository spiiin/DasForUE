options gen2
require ue_scene_components
[ue_object_ref(script="dsl_cooked_component.das",type="CookedBehavior")]
struct CookedComponentRef { handle:UObjectHandle }
[ue_class(parent="/Script/DasRuntime.DasProbeActor",module_id="cookednative",Blueprintable)]
class CookedNativeOwner {
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Child:SceneComponentRef
        var Behavior:CookedComponentRef
        var OtherBehavior:CookedComponentRef
    }
    @ue_component
    @Attach="Child"
    Grandchild:SceneComponentRef
    defaults=ue_defaults() {
        self.Child.RelativeLocation=ue_vector(0.0lf,0.0lf,50.0lf)
        self.Child.RelativeRotation=ue_rotator(0.0lf,30.0lf,0.0lf)
        self.Child.bHiddenInGame=true
        self.Behavior.Bonus=42
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Behavior.GetBonus(input) }
}
