options gen2
module ue_inherited_defaults_base public
require ue_dsl public
require ue_object_types public
[ue_class(module_id="inheriteddefaults",Blueprintable)]
class DefaultsBase {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) {
        var Score:int=12
        var Manual:int=12
        var Keep:int=100
        var Frozen:int=12
        var Position:UEVector=ue_vector(1.0lf,2.0lf,3.0lf)
        var Asset:ObjectRef=ue_asset_default("/Engine/BasicShapes/Cube.Cube")
        var SpawnType:ActorClass=ue_class_default("/Script/Engine.Actor")
    }
    [ue_function(BlueprintCallable)]
    def BaseRead(input:int):int { return self.Score+input }
}
