options gen2
require ue_dsl

[ue_struct(module_id="groupvalues")]
struct GroupValues {
    properties = ue_properties(EditAnywhere, BlueprintReadWrite, Category="State") {
        var Score:int=0
        var Speed:double=100.0lf
    }
}
