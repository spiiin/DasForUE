options gen2
module ue_interface_values public
require ue_dsl public
[ue_enum(module_id="interfacevalues", id="interfacevalues.State", BlueprintType)]
enum InterfaceState {
    Idle = 7
    Active = 42
}
[ue_struct(module_id="interfacevalues", id="interfacevalues.Settings", BlueprintType)]
struct InterfaceSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 15
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
