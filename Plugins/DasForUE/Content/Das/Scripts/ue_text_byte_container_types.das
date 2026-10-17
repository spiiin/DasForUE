options gen2
module ue_text_byte_container_types public
require ue_text public
[ue_enum(BlueprintType)]
enum ContainerState {
 Idle=7
 Active=42
}
[ue_struct(BlueprintType)]
struct TextByteBag {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Weights:array<uint8>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Messages:array<FText>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Tags:table<uint8>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Labels:table<uint8;FText>
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Values:table<string;uint8>
}
