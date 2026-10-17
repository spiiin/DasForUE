options gen2
module ue_text_byte_types public
require ue_text public
[ue_struct(module_id="dsl_text_byte", BlueprintType)]
struct TextByteData {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Priority:uint8 = uint8(7)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title:FText = ue_text("DasTextByte", "StructTitle", "Struct title")
}
