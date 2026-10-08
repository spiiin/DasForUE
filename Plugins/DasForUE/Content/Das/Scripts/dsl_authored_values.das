options gen2
options gc
require ue_dsl
[ue_class(module_id="authoredvalues",Blueprintable)]
class TypedActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Mode : UEEnumValue = ue_enum_value("/Game/DasAuthoredTypes/State.State", "Idle")
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Config : UEStructValue = ue_struct_default("/Game/DasAuthoredTypes/Settings.Settings", "(Count=15,Title=\"Initial\")")
    [ue_function(BlueprintCallable,arg_value_enum="/Game/DasAuthoredTypes/State.State")]
    def SetMode(value:UEEnumValue):void { self.Mode = value }
    [ue_function(BlueprintCallable,return_enum="/Game/DasAuthoredTypes/State.State")]
    def GetMode():UEEnumValue { return self.Mode }
    [ue_function(BlueprintCallable)]
    def ModeNumber():int { return ue_enum_number(self.Mode) }
    [ue_function(BlueprintCallable,arg_value_struct="/Game/DasAuthoredTypes/Settings.Settings")]
    def SetConfig(value:UEStructValue):void { self.Config = value }
    [ue_function(BlueprintCallable,return_struct="/Game/DasAuthoredTypes/Settings.Settings")]
    def GetConfig():UEStructValue { return self.Config }
    [ue_function(BlueprintCallable,arg_mode_enum="/Game/DasAuthoredTypes/State.State",arg_data_struct="/Game/DasAuthoredTypes/Settings.Settings",return_struct="/Game/DasAuthoredTypes/Settings.Settings")]
    def Evaluate(mode:UEEnumValue; data:UEStructValue):UEStructValue {
        return ue_struct_with_int(data, "Count", ue_struct_get_int(data, "Count") + ue_enum_number(mode))
    }
}
