options gen2
module ue_interface_values public
require ue_dsl public
require ue_object_types public
[ue_enum(module_id="interfacevalues", id="interfacevalues.State", BlueprintType)]
enum InterfaceState {
    Idle = 7
    Active = 42
}
[ue_struct(module_id="interfacevalues", id="interfacevalues.Details", BlueprintType)]
struct InterfaceDetails {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Events : DasEventInterface
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Weights : table<string; double>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Labels : table<int; UENameValue>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Tags : table<string>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Ids : table<int>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Key : UENameValue
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : ActorRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Object : ObjectRef
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Amount : int = 2
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Text : string = "Detail"
}
[ue_struct(module_id="interfacevalues", id="interfacevalues.Settings", BlueprintType)]
struct InterfaceSettings {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Details : InterfaceDetails = InterfaceDetails()
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Items : array<InterfaceDetails>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Count : int = 15
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Title : string = "Initial"
}
