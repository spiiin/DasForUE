options gen2
module ue_dsl public
require ue_foundation public

// Native host identity markers for authored class inheritance.
// These declarations do not construct Unreal objects or expose native virtuals.
[ue_native_base(path="/Script/Engine.Actor")]
class Actor {}
[ue_native_base(path="/Script/Engine.Pawn")]
class Pawn {}
[ue_native_base(path="/Script/Engine.Character")]
class Character {}
[ue_native_base(path="/Script/Engine.ActorComponent")]
class ActorComponent {}
[ue_native_base(path="/Script/Engine.SaveGame")]
class SaveGame {}

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

def ue_dsl_commit_struct_member_enum(property:string; member:string; value:UEEnumValue):void {
    ue_set_self_struct(property, __ue_struct_with_enum_value(ue_get_self_struct(property), member, value))
}

def ue_dsl_commit_struct_member_object(property:string; member:string; value:UObjectHandle):void {
    ue_set_self_struct(property, ue_struct_with_object(ue_get_self_struct(property), member, value))
}

def ue_dsl_commit_struct_member_struct(property:string; member:string; value:UEStructValue):void {
    ue_set_self_struct(property, ue_struct_nested_with(ue_get_self_struct(property), member, -1, value))
}

// Concrete weak handles and nominal object views share the bound-delegate factory.
def ue_delegate_target(target:UObjectHandle):UObjectHandle { return target }
def ue_delegate_target(target:auto):UObjectHandle { return target.handle }
def ue_delegate_name(method:string):string { return method }
def ue_delegate_name(method:UENameValue):string { return ue_name_text(method) }

def ue_dsl_commit_struct_member_byte(property:string; member:string; value:uint8):void {
 ue_set_self_struct(property,ue_struct_with_byte(ue_get_self_struct(property),member,value))
}
def ue_dsl_commit_struct_member_text(property:string; member:string; value:UETextValue):void {
 ue_set_self_struct(property,ue_struct_with_text(ue_get_self_struct(property),member,value))
}
