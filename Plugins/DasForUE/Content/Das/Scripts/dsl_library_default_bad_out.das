options gen2
require ue_dsl
[ue_library]
class BadDefaults {
[ue_function(BlueprintCallable)] def static Bad(@ue_out var value:int&=2):void {}
}
