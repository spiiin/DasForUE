options gen2
module ue_dsl public
require ue_foundation public

// Read the latest native snapshot after evaluating the assigned value.
def ue_dsl_commit_struct_member_int(property:string; member:string; value:int):void {
    ue_set_self_struct(property, ue_struct_with_int(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member_float(property:string; member:string; value:float):void {
    ue_set_self_struct(property, ue_struct_with_float(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member_double(property:string; member:string; value:double):void {
    ue_set_self_struct(property, ue_struct_with_double(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member_bool(property:string; member:string; value:bool):void {
    ue_set_self_struct(property, ue_struct_with_bool(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member_string(property:string; member:string; value:string):void {
    ue_set_self_struct(property, ue_struct_with_string(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member_name(property:string; member:string; value:UENameValue):void {
    ue_set_self_struct(property, ue_struct_with_name(ue_get_self_struct(property), member, value))
}
