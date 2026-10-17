options gen2
options gc
require ue_dsl
require ue_text

require ue_text_byte_types

[ue_class(type_folder="/Game/DasTextByte/Types", Blueprintable)]
class TextByteActor : Actor {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite) {
        var Priority:uint8 = uint8(255)
        var Title:FText = ue_text("DasTextByte", "Title", "Example title")
        var Config:TextByteData = TextByteData()
    }
    [ue_function(BlueprintCallable)]
    def EchoByte(value:uint8):uint8 { self.Priority=value; return self.Priority }
    [ue_function(BlueprintCallable)]
    def EchoText(value:FText):FText { self.Title=value; return self.Title }
    [ue_function(BlueprintCallable)]
    def Adjust(var priority:uint8&; @ue_out var title:FText&):FText {
        priority=uint8(int(priority)+1)
        title=ue_text("DasTextByte", "Output", "Output text")
        self.Config.Priority=priority
        self.Config.Title=title
        return self.Config.Title
    }
    [ue_function(BlueprintCallable)]
    def DefaultByte(value:uint8=uint8(255)):uint8 { return value }
    [ue_function(BlueprintCallable)]
    def DefaultText(value:FText=ue_text("DasTextByte","Default","Default text")):FText { return value }
    [ue_function(BlueprintCallable)]
    def Reverse(var title:FText&; @ue_out var priority:uint8&):uint8 {
        self.Title=title
        title=ue_text_invariant("Replaced")
        priority=uint8(255)
        return priority
    }
    [ue_function(BlueprintPure)]
    def Probe():int { return int(self.Priority) }
}
