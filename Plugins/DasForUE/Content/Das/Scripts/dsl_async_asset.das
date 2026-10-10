options gen2
require ue_dsl
require ue_object_types

[ue_class(module_id="asyncasset",Blueprintable)]
class AsyncAssetActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    AssetPath:string=""
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Loaded:ObjectRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Succeeded:bool=false

    [ue_function(BlueprintCallable)]
    def StartLoad():void {
        ue_asset_load_async("asset",self.AssetPath,"OnLoaded")
    }
    [ue_function(BlueprintCallable)]
    def OnLoaded(value:ObjectRef):void {
        self.Loaded = value
        self.Succeeded = ue_object_valid(value.handle)
    }
    [ue_function(BlueprintCallable)]
    def Pending():bool { return ue_asset_is_pending("asset") }
    [ue_function(BlueprintCallable)]
    def Cancel():bool { return ue_asset_cancel("asset") }
    [ue_function(BlueprintCallable)]
    def CancelAll():int { return ue_asset_cancel_all() }
}
