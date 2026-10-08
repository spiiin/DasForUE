options gen2
require ue_dsl
[ue_enum(module_id="memberids", BlueprintType,
         member_id_Active="memberids.State.running")]
enum State {
    Idle = 7
    Active = 42
}
