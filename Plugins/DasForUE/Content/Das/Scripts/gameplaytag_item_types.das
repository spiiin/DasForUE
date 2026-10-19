options gen2
require ue_dsl
require ue_gameplay_tags
[ue_struct(BlueprintType)]
struct ItemTagRecord {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Kind:GameplayTag
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Tags:GameplayTagContainer
}
