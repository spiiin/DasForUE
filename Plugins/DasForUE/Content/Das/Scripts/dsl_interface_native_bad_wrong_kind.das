options gen2
require ue_dsl
require ue_object_types
[ue_interface(module_id="badnative", BlueprintType)]
class Contract {
[ue_function(BlueprintCallable, arg_value_enum=Actor, return_enum=EDasCallMode)]
 def Echo(value:UEEnumValue):UEEnumValue {}
}
