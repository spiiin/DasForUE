options gen2
module ue_text_byte_delegates public
require ue_text public
[ue_delegate]
struct TextQuery { signature:function<(value:FText):FText> }
[ue_delegate]
struct ByteQuery { signature:function<(value:uint8):uint8> }
[ue_delegate]
struct TextByteMutation {
    def signature(var number:uint8&; var title:FText&;
                  @ue_out var previous:uint8&; @ue_out var oldTitle:FText&):FText {}
}
[ue_multicast]
struct TextByteChanged { signature:function<(number:uint8; title:FText):void> }
[ue_multicast]
struct TextByteRefChanged { def signature(var number:uint8&; var title:FText&):void {} }
[ue_delegate]
struct TextByteArrays {
    def signature(var numbers:array<uint8>&; @ue_out var messages:array<FText>&):void {}
}
