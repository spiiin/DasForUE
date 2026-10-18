options gen2
module ue_curve_tables public
require ue_foundation public

// Reads an already loaded table. Missing rows are distinct from empty curves.
def ue_curve_table_has_row(curves:UObjectHandle;name:string):bool {return __ue_curve_table_has(curves,name)}
def ue_curve_table_names(curves:UObjectHandle):array<string> {
    let snapshot=__ue_curve_table_names(curves)
    var result:array<string>
    for (index in range(ue_struct_array_size(snapshot,"Values"))) {
        push(result,ue_struct_get_string(snapshot,"Values."+string(index)))
    }
    return <- result
}
// DefaultValue is UE's fallback for a curve with no keys and no stored default.
// It is not a fallback for a missing row or invalid handle.
def ue_curve_table_eval(curves:UObjectHandle;name:string;time:float;default_value:float=0.0):float {
    return __ue_curve_table_eval(curves,name,time,default_value)
}
