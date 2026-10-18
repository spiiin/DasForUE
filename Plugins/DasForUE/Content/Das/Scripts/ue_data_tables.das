options gen2
module ue_data_tables public
require ue_dsl public

// Does not load assets. Pass an already loaded items_table handle.
def ue_data_table_has_row(items_table:UObjectHandle;name:string):bool {return __ue_data_table_has(items_table,name)}
def ue_data_table_names(items_table:UObjectHandle):array<string> {
    let snapshot=__ue_data_table_names(items_table)
    var result:array<string>
    for (index in range(ue_struct_array_size(snapshot,"Values"))) {
        push(result,ue_struct_get_string(snapshot,"Values."+string(index)))
    }
    return <- result
}
