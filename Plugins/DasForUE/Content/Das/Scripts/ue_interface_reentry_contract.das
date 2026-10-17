options gen2
module ue_interface_reentry_contract public
require ue_text public
require ue_object_types public
[ue_interface(BlueprintType)]
class ReentryContract {
    [ue_function(BlueprintCallable)]
    def Bounce(Input:int):int {}
    [ue_function(BlueprintCallable)]
    def Label():string {}
    [ue_function(BlueprintCallable)]
    def Fault(@ue_out var number:uint8&; var title:FText&):void {}
}
