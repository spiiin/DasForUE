options gen2
module ue_catalog_contract public
require ue_dsl public
require ue_struct_map_types public

// Import the MapEntry asset into /Game/DasStructMaps before collecting this interface.
[ue_interface(module_id="catalogcontract", type_folder="/Game/DasStructMaps", BlueprintType)]
class CatalogContract {
    [ue_function(BlueprintCallable)]
    def GetScore():int {}
    [ue_function(BlueprintCallable)]
    def PutEntry(entry:MapEntry):void {}
}
