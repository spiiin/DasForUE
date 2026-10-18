options gen2
module ue_primary_assets public
require ue_foundation public

// A value ID, independent of object lifetime or VM token tables.
struct UEPrimaryAssetId {
    asset_type:string
    asset_name:string
}
def ue_primary_asset_id(asset_type:string;asset_name:string):UEPrimaryAssetId {
    return UEPrimaryAssetId(asset_type=asset_type,asset_name=asset_name)
}
def ue_primary_assets(asset_type:string):array<UEPrimaryAssetId> {
    let query=__ue_primary_find(asset_type)
    var result:array<UEPrimaryAssetId>
    resize(result,__ue_primary_query_count(query))
    for (index in range(length(result))) {
        result[index]=ue_primary_asset_id(asset_type,__ue_primary_query_name(query,index))
    }
    __ue_primary_query_release(query)
    return <- result
}
def ue_primary_asset_exists(id:UEPrimaryAssetId):bool {return __ue_primary_exists(id.asset_type,id.asset_name)}
def ue_primary_asset_path(id:UEPrimaryAssetId):string {return __ue_primary_path(id.asset_type,id.asset_name)}
def ue_primary_asset_object(id:UEPrimaryAssetId):UObjectHandle {return __ue_primary_object(id.asset_type,id.asset_name)}
// Exported callback(game:UObjectHandle):void, nullable on failure.
def ue_primary_asset_load_async(request:string;id:UEPrimaryAssetId;bundles:array<string>;handler:string):void {
    __ue_primary_load_async(request,id.asset_type,id.asset_name,bundles,handler)
}
def ue_primary_asset_load_async(request:string;id:UEPrimaryAssetId;handler:string):void {
    var bundles:array<string>
    ue_primary_asset_load_async(request,id,bundles,handler)
}
def ue_primary_asset_pending(request:string):bool {return __ue_primary_pending(request)}
def ue_primary_asset_cancel(request:string):bool {return __ue_primary_cancel(request)}
// Release this VM's lease, revoke its pending callbacks for this ID.
def ue_primary_asset_unload(id:UEPrimaryAssetId):bool {return __ue_primary_unload(id.asset_type,id.asset_name)}
