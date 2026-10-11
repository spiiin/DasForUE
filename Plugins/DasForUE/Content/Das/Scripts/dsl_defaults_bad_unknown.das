options gen2
require ue_scene_components
[ue_class(module_id="baddefaults",Blueprintable)]
class BadDefaults {
    @ue_component
    Pivot:SceneComponentRef
    defaults = ue_defaults() {
        self.Missing.bHiddenInGame = true
    }
    [ue_function(BlueprintCallable)]
    def Ping(input:bool):bool { return input }
}
