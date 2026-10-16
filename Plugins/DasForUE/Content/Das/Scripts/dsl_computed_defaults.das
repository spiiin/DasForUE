options gen2
module dsl_computed_defaults public
require ue_scene_components public
require ue_object_types public

// Ordinary daScript functions; no separate expression interpreter.
def default_score(value:int):int {
    if (value > 0) {
        return value * 4
    }
    return 0
}
def default_asset():string { return "/Engine/BasicShapes/" + "Cube.Cube" }

[ue_class(module_id="computeddefaults",Blueprintable)]
class ComputedDefaults : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Seed:int=2+3
        var Score:int=0
        var Manual:int=0
        var Derived:int=0
        var Label:string=""
        var Enabled:bool=false
        var Speed:double=25.0lf*2.0lf
        var Position:UEVector
        var Asset:ObjectRef
    }
    @ue_component
    @RootComponent
    Root:SceneComponentRef
    components=ue_components(VisibleAnywhere,BlueprintReadOnly) {
        var Pivot:SceneComponentRef
    }
    defaults=ue_defaults() {
        self.Score=default_score(self.Seed*2)
        self.Manual=default_score(10)
        self.Derived=self.Score+2
        self.Label="computed"+" defaults"
        self.Enabled=default_score(1)>0
        self.Position=ue_vector(self.Seed*2,default_score(2),3.0lf+4.0lf)
        self.Asset=ue_asset_default(default_asset())
        self.Pivot.RelativeLocation=ue_vector(0.0lf,0.0lf,double(self.Derived))
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Score+self.Derived+input }
}
