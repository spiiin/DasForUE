options gen2
require ue_dsl
require ue_object_types
[ue_class(parent=Actor, module_id="typecalls", Blueprintable)]
class TypeCalls {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : UObjectHandle = ue_object_value(ue_type(Actor, Object))
    [ue_function(BlueprintCallable, arg_target_class=Object)]
    def Check(target:UObjectHandle):bool {
        let converted = ue_cast_nullable_object(target, ue_type(Actor, Object))
        return ue_object_is_a(converted, "Actor")
    }
    [ue_function(BlueprintCallable)]
    def Values():int {
        let mode = ue_enum_from_number(ue_type(EDasCallMode), 7)
        let data = ue_struct_value(ue_type(DasCallPayload))
        return ue_enum_number(mode) + ue_struct_get_int(data, "Count")
    }
    [ue_function(BlueprintCallable)]
    def Member():int { return ue_enum_number(ue_enum_value("EDasCallMode", "Idle")) }
    [ue_function(BlueprintCallable, arg_target_class=Object)]
    def Implements(target:UObjectHandle):bool {
        return ue_implements_interface(target, ue_type(DasProbeInterface))
    }
    [ue_function(BlueprintCallable, arg_target_class=Object)]
    def Query(target:UObjectHandle):int {
        let call = ue_begin_interface_call(target, ue_type(DasProbeInterface), "Query")
        ue_arg_int(call, "Input", 5)
        ue_invoke_call(call)
        let result = ue_result_int(call, "ReturnValue")
        ue_release_call(call)
        return result
    }
}
