options gen2
require ue_scene_components
require ue_component_types
[ue_native_object(path="/Script/Engine.RotatingMovementComponent")]
struct RotatingComponentRef { handle:UObjectHandle }
def RotatingComponentRef_from(value:UObjectHandle):RotatingComponentRef {
    return RotatingComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.RotatingMovementComponent"))
}

[ue_class(module_id="nativecomponents",Blueprintable)]
class NativeOwner {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    DynamicComponentClass:string=""
    components = ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Child:SceneComponentRef
        var Behavior:ScoreComponent
        var OtherBehavior:ScoreComponent
    }
    @ue_component
    @RootComponent
    Pivot:SceneComponentRef
    @ue_component
    @Attach="Child"
    Grandchild:SceneComponentRef
    defaults = ue_defaults() {
        self.Child.RelativeLocation = ue_vector(0.0lf,0.0lf,50.0lf)
        self.Child.RelativeRotation = ue_rotator(0.0lf,30.0lf,0.0lf)
        self.Child.bHiddenInGame = true
        self.Behavior.Bonus = 42
    }
    [ue_function(BlueprintCallable)]
    def HasPivot(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        let dynamic=ue_component_create(type<ScoreComponent>,actor,"DynamicBehavior",self.DynamicComponentClass)
        let reusedDynamic=ue_component_get_or_create(type<ScoreComponent>,actor,"DynamicBehavior",self.DynamicComponentClass)
        dynamic.Bonus=77
        let dynamicValid=reusedDynamic.Bonus==77
        let dynamicRemoved=ue_component_destroy_handle(dynamic.handle)
        let dynamicGone=!ue_object_valid(reusedDynamic.handle)
        let behavior=ue_component_get(type<ScoreComponent>,actor,"LookupBehavior")
        let absent=ue_component_get(type<ScoreComponent>,actor,"MissingBehavior")
        let authoredLookup=ue_object_valid(behavior.handle) && !ue_object_valid(absent.handle) && behavior.Bonus==42
        let missing=!ue_scene_valid(ue_scene_get(actor,"RuntimePivot"))
        let rotation=ue_component_create(type<RotatingComponentRef>,actor,"RuntimeRotation")
        let sameRotation=ue_component_get_or_create(type<RotatingComponentRef>,actor,"RuntimeRotation")
        let rotationFound=ue_component_get(type<RotatingComponentRef>,actor,"RuntimeRotation")
        let rotationValid=ue_object_valid(rotationFound.handle)
        let rotationRemoved=ue_component_destroy_handle(rotation.handle)
        let rotationGone=!ue_object_valid(sameRotation.handle)
        let sceneClass=ue_class_loaded(type<SceneComponentRef>,"/Script/Engine.SceneComponent")
        let temporary=ue_component_get_or_create(type<SceneComponentRef>,actor,"RuntimePivot",sceneClass)
        let reused=ue_scene_get_or_create(actor,"RuntimePivot")
        ue_scene_set_relative_transform(reused,ue_transform_identity())
        let found=ue_scene_valid(ue_scene_get(actor,"RuntimePivot"))
        ue_scene_set_relative_transform(temporary,ue_transform_identity())
        ue_scene_set_world_transform(temporary,ue_transform_identity(),true)
        let attached=ue_scene_attach(temporary,self.Pivot,"","KeepRelative",false)
        let parentValid=ue_scene_valid(ue_scene_parent(temporary))
        ue_scene_detach(temporary,"KeepWorld")
        let detached=!ue_scene_valid(ue_scene_parent(temporary))
        let removed=ue_scene_destroy(temporary)
        return dynamicValid && dynamicRemoved && dynamicGone && authoredLookup && rotationValid && rotationRemoved && rotationGone && missing && found && !ue_scene_valid(reused) && !ue_scene_valid(ue_scene_get(actor,"RuntimePivot")) && ue_scene_valid(self.Pivot) && attached && parentValid && detached && removed && !ue_scene_valid(temporary)
    }
}
