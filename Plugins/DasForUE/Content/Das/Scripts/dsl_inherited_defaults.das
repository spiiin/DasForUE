options gen2
module dsl_inherited_defaults public
require ue_inherited_defaults_base public
[ue_class(module_id="inheriteddefaults",Blueprintable)]
class DefaultsChild : DefaultsBase {
    defaults=ue_defaults() {
        self.Score=25
        self.Manual=25
        self.Frozen=10
        self.Position=ue_vector(4.0lf,5.0lf,6.0lf)
        self.Asset=ue_asset_default("/Engine/BasicShapes/Sphere.Sphere")
        self.SpawnType=ue_class_default("/Script/Engine.Pawn")
    }
    [ue_function(BlueprintCallable)]
    def Read(input:int):int { return self.Score+self.Manual+input }
}
