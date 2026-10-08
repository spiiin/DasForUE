options gen2
require ue_dsl
require ue_object_types
[ue_class(module_id="typesyntax", parent=Actor, Blueprintable)]
class TypeSyntax {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : UObjectHandle = ue_object_value("Actor")
    [ue_function(BlueprintCallable, arg_target_class=Actor, return_class="Engine.Actor")]
    def Echo(target:UObjectHandle):UObjectHandle { return target }
    [ue_function(BlueprintCallable, arg_value_struct=DasCallPayload, return_struct="DasRuntime.DasCallPayload")]
    def Payload(value:UEStructValue):UEStructValue { return value }
    [ue_function(BlueprintCallable, arg_value_enum=EDasCallMode, return_enum="DasRuntime.EDasCallMode")]
    def Mode(value:UEEnumValue):UEEnumValue { return value }
}
