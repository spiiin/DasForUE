options gen2
require ue_text
[ue_interface(BlueprintType)]
class TextByteContract {
    [ue_function(BlueprintCallable)]
    def EchoByte(value:uint8):uint8 {}
    [ue_function(BlueprintCallable)]
    def EchoText(value:FText):FText {}
    [ue_function(BlueprintCallable)]
    def Change(var number:uint8&; var title:FText&;
               @ue_out var previous:uint8&; @ue_out var oldTitle:FText&):FText {}
    [ue_function(BlueprintCallable)]
    def ChangeArrays(var numbers:array<uint8>&; @ue_out var messages:array<FText>&):void {}
}
