options gen2
require ue_dsl
// Current operating mode.
[ue_enum(module_id="doctypes",BlueprintType)]
enum DocMode {
    Idle=0
    Active=1
}
// Editable score settings.
[ue_struct(module_id="doctypes",BlueprintType)]
struct DocSettings {
    // Initial score.
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=10
}
