options gen2
module ue_asset_optional public
require ue_object_types public

// Game content only; an absent optional package resolves quietly to null.
def ue_asset_load_optional(path:string):ObjectRef {
    let call=ue_begin_static_call("/Script/DasRuntime.DasAssetLibrary","LoadOptionalAsset","")
    ue_arg_string(call,"assetpath",path)
    ue_invoke_call(call)
    let result=ue_ref(type<ObjectRef>,ue_result_object(call,"returnvalue"))
    ue_release_call(call)
    return result
}
