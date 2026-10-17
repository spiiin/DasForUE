options gen2
require ue_soft_references
[export]
def main(input:int):int {
    let empty=default<SoftObject>
    if(!ue_soft_is_null(empty)) { return -1 }
    let value=ue_soft(type<SoftObject>,"/Game/NotLoaded/Asset.Asset")
    if(ue_soft_is_loaded(value) || ue_soft_path(value)!="/Game/NotLoaded/Asset.Asset") { return -2 }
    if(ue_object_valid(ue_soft_get(value).handle)) { return -3 }
    return input+2
}
