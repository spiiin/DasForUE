options gen2
require ue_primary_assets

var current:UEPrimaryAssetId
var loaded_item:UObjectHandle
var finished:bool=false

[export]
def item_loaded(asset:UObjectHandle):void {
    loaded_item=asset
    finished=true
    // Cast asset to the generated reference view for your Item data class here.
}

// Requires Item assets registered in the project's AssetManager settings.
// Call from a persistent script owned by an Actor or widget in a game world.
[export]
def main(input:int):int {
    let items=ue_primary_assets("Item")
    if (length(items)==0) {return 0}
    current=items[0]
    finished=false
    ue_primary_asset_load_async("item",current,"item_loaded")
    return length(items)
}

[export]
def release_item(input:int):int {
    loaded_item=default<UObjectHandle>
    return ue_primary_asset_unload(current) ? 1 : 0
}
