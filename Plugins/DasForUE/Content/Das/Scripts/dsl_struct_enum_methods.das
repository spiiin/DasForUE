options gen2
options gc
require ue_dsl
[ue_class(module_id="enumvalues",Blueprintable)]
class EnumStructBridge {
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasNamedMode",return_struct="/Script/DasRuntime.DasNamedMode")]
    def SetMode(value:UEStructValue; symbol:string):UEStructValue {
        return ue_struct_with_enum(value,"Mode","/Script/DasRuntime.EDasCallMode",symbol)
    }
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasNamedMode")]
    def ReadMode(value:UEStructValue):string {
        return ue_struct_get_enum(value,"Mode","/Script/DasRuntime.EDasCallMode")
    }
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasEnumBundle",return_struct="/Script/DasRuntime.DasEnumBundle")]
    def UpdateBundle(value:UEStructValue):UEStructValue {
        var result = ue_struct_with_enum(value,"Legacy","/Script/DasRuntime.EDasLegacyMode","DasLegacyActive")
        return ue_struct_with_enum(result,"Nested.Mode","/Script/DasRuntime.EDasCallMode","Paused")
    }
    [ue_function(BlueprintCallable,arg_value_struct="/Script/DasRuntime.DasNamedMode")]
    def WrongEnumType(value:UEStructValue):string {
        return ue_struct_get_enum(value,"Mode","/Script/DasRuntime.EDasLegacyMode")
    }
}
