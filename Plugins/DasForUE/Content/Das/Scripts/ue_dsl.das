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
[ue_native_base(path="/Script/Engine.DataAsset")]
class DataAsset {}
[ue_native_base(path="/Script/Engine.PrimaryDataAsset")]
class PrimaryDataAsset {}

// Commit a non-scalar member of a self struct property; reads the latest native snapshot after evaluating
// the assigned value. Scalar leaves are lowered to __ue_self_struct_set_<kind> instead.
def ue_dsl_commit_struct_member(property:string; member:string; value:UEEnumValue):void {
    ue_set_self_struct(property, __ue_struct_with_enum_value(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member(property:string; member:string; value:UObjectHandle):void {
    ue_set_self_struct(property, ue_struct_with_object(ue_get_self_struct(property), member, value))
}
def ue_dsl_commit_struct_member(property:string; member:string; value:UEStructValue):void {
    ue_set_self_struct(property, ue_struct_nested_with(ue_get_self_struct(property), member, -1, value))
}

// Concrete weak handles and nominal object views share the bound-delegate factory.
def ue_delegate_target(target:UObjectHandle):UObjectHandle { return target }
def ue_delegate_target(target:auto):UObjectHandle { return target.handle }
def ue_delegate_name(method:string):string { return method }
def ue_delegate_name(method:UENameValue):string { return ue_name_text(method) }
