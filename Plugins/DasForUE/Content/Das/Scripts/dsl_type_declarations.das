options gen2
require ue_dsl
[ue_enum(module_id="types", BlueprintType, ToolTip="Example authored enum")]
enum State {
    Idle = 7
    Active = 42
}
[ue_struct(module_id="types", BlueprintType, ToolTip="Example authored struct")]
struct Settings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 10
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weight : float = 1.5
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Enabled : bool = true
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Привет"
}
