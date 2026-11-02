options gen2
require ue_scene_components

[ue_class(module_id="vectormath", Blueprintable)]
class LiteralVectorDefaults : Actor {
    def BeginPlay() : void {}

    @ue_component
    @RootComponent
    Root : SceneComponentRef
    properties = ue_properties(EditAnywhere) { var Offset : UEVector = ue_vector(1.0lf, 0.0lf, 0.0lf) }
    defaults = ue_defaults() {
        self.Root.RelativeScale3D = ue_vector(1.0lf, 1.0lf, 1.0lf)
        self.Root.RelativeLocation = ue_vector(0.0lf, 0.0lf, 0.0lf)
        self.Root.RelativeRotation = ue_rotator(0.0lf, 0.0lf, 0.0lf)
    }
}
