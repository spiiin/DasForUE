options gen2
require ue_primary_assets
require ue_dsl
require ue_soft_references
[ue_object_ref(script="__AUTHOR__",type="ItemDefinition")]
struct ItemRef {handle:UObjectHandle}
var phase:int=0
var retained:UObjectHandle
[export]
def loaded(asset:UObjectHandle):void {
    if (!ue_object_valid(asset)) {phase=-1;return}
    retained=asset
    let item=ue_ref(type<ItemRef>,asset)
    phase=(item.Score==73 && item.Label=="Potion" && item.ComputeValue(5)==78 && ue_soft_path(item.Icon)=="__BUNDLE__") ? 1 : -2
}
[export]
def start(input:int):int {
    let items=ue_primary_assets("__TYPE__")
    if (length(items)!=1 || items[0].asset_name!="Potion") {return -3}
    var bundles:array<string>
    push(bundles,"UI")
    ue_primary_asset_load_async("item",items[0],bundles,"loaded")
    return 1
}
[export]
def status(input:int):int {return phase}
[export]
def after_gc(input:int):int {
    let item=ue_ref(type<ItemRef>,retained)
    return item.Score==73 && item.ComputeValue(7)==80 ? 1 : 0
}
[export]
def unload(input:int):int {return ue_primary_asset_unload(ue_primary_asset_id("__TYPE__","Potion")) ? 1 : 0}
