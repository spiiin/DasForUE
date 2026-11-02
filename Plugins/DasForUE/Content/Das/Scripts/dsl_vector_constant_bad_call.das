options gen2
require ue_scene_components
def make_vector() : UEVector { return ue_vector(1.0lf, 1.0lf, 1.0lf) }
let NON_CONSTANT = make_vector()
[ue_class(Blueprintable)]
class InvalidVectorDefaults : Actor {
    def BeginPlay() : void {}

    @ue_component
    @RootComponent
    Root : SceneComponentRef
    defaults = ue_defaults() { self.Root.RelativeScale3D = NON_CONSTANT }
}
