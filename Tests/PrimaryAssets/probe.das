options gen2
require ue_primary_assets
var phase:int=0
var retained:UObjectHandle
[export]
def loaded(asset:UObjectHandle):void {
    if (!ue_object_valid(asset)) {phase=-10;return}
    retained=asset
    phase=ue_object_get_int(asset,"Score")==73 ? 1 : -11
}
[export]
def missing(asset:UObjectHandle):void {phase=ue_object_valid(asset) ? -12 : 2}
[export]
def cancelled(asset:UObjectHandle):void {phase=-13}
[export]
def start(input:int):int {
    let assets=ue_primary_assets("__TYPE__")
    if (length(assets)!=1 || assets[0].asset_name!="Profile") {return -1}
    let id=assets[0]
    if (!ue_primary_asset_exists(id) || ue_primary_asset_path(id)!="__PATH__") {return -2}
    var bundles:array<string>
    push(bundles,"UI")
    ue_primary_asset_load_async("profile",id,bundles,"loaded")
    return 1
}
[export]
def status(input:int):int {return phase}
[export]
def retained_after_gc(input:int):int {
    let owned=ue_primary_asset_object(ue_primary_asset_id("__TYPE__","Profile"))
    return ue_object_valid(retained) && ue_object_valid(owned) && ue_object_get_int(owned,"Score")==73 ? 1 : 0
}
[export]
def unload(input:int):int {
    return ue_primary_asset_unload(ue_primary_asset_id("__TYPE__","Profile")) ? 1 : 0
}
[export]
def load_missing(input:int):int {
    ue_primary_asset_load_async("missing",ue_primary_asset_id("__TYPE__","Missing"),"missing")
    return 1
}
[export]
def cancel_load(input:int):int {
    var bundles:array<string>
    push(bundles,"UI")
    ue_primary_asset_load_async("cancel",ue_primary_asset_id("__TYPE__","Profile"),bundles,"cancelled")
    if (!ue_primary_asset_pending("cancel") || !ue_primary_asset_cancel("cancel")) {return -1}
    return ue_primary_asset_pending("cancel") ? -2 : 1
}
