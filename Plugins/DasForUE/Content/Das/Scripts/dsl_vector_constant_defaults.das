options gen2
require ue_scene_components
require ue_vector_math
let LOCAL_UNIT_SCALE = UE_VECTOR_ONE
let LOCAL_OFFSET = ue_vector(1.0lf, 0.0lf, 0.0lf)

[ue_class(module_id="vectormath", Blueprintable)]
class NamedVectorDefaults : Actor {
    def BeginPlay() : void {}

    @ue_component
    @RootComponent
    Root : SceneComponentRef
    properties = ue_properties(EditAnywhere) { var Offset : UEVector = LOCAL_OFFSET }
    defaults = ue_defaults() {
        self.Root.RelativeScale3D = LOCAL_UNIT_SCALE
        self.Root.RelativeLocation = ue_vector_math::UE_VECTOR_ZERO
        self.Root.RelativeRotation = UE_ROTATOR_ZERO
    }
}
