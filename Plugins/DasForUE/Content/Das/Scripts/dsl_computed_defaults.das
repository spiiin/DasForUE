options gen2
module dsl_computed_defaults public
require ue_scene_components public
require ue_object_types public
require ue_native_engine_values public
require ue_default_assets public

// Ordinary daScript functions; no separate expression interpreter.
def default_score(value:int):int {
    if (value > 0) {
        return value * 4
    }
    return 0
}
def default_asset():string { return "/Engine/BasicShapes/" + "Cube.Cube" }

def default_array_asset():string { return "/Game/DasAssetDefaults/"+"ArrayAsset.ArrayAsset" }
def default_map_asset():string { return "/Game/DasAssetDefaults/"+"MapAsset.MapAsset" }

[ue_class(module_id="computeddefaults",Blueprintable)]
class ComputedDefaults : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Assets:array<CurveAssetRef> = [ue_asset_default(default_array_asset()),ue_asset_default("")]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetMap:table<UENameValue;CurveAssetRef>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetSet:table<CurveAssetKey> = {ue_asset_default(default_array_asset()),ue_asset_default("")}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetsById:table<int;CurveAssetRef> = {7=>ue_asset_default(default_map_asset())}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetsByText:table<string;CurveAssetRef> = {"main"=>ue_asset_default(default_array_asset())}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetWeights:table<CurveAssetKey;double> = {ue_asset_default(default_map_asset())=>2.5lf,ue_asset_default("")=>1.0lf}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Numbers:array<int> = [self.Seed,default_score(2)]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Ratios:array<double> = [1.2345678901234567lf]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ManualNumbers:array<int> = [default_score(2)]
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    TagsDefault:table<string> = {"red","blue",string(default_score(2))}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ScoresDefault:table<string;int> = {"one"=>default_score(2),"two"=>self.Seed+1}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    EnumDefault:table<SpawnCollision;int> = {SpawnCollision.AlwaysSpawn=>default_score(2)}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    NamesDefault:table<UENameValue;int> = {ue_name_value("Item_3")=>self.Seed}
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    VectorsDefault:array<UEVector> = [ue_vector(double(self.Seed),double(default_score(2)),3.0lf)]

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
        self.AssetMap=({ue_name_value("Main")=>ue_asset_default(default_map_asset()),ue_name_value("NoneValue")=>ue_asset_default("")})
        self.Numbers=[self.Seed,default_score(2)]
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
