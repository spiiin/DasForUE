options gen2
module dsl_math_library public
require ue_dsl
[ue_library(Category="Examples")]
class GameMath {
    // Складывает два целых числа.
    [ue_function(BlueprintPure)]
    def static Add(left:int; right:int):int { return left+right }
    [ue_function(BlueprintCallable)]
    def static Stats(var counter:int&; @ue_out var doubled:int&):void { counter+=3;doubled=counter*2 }
    [ue_function(BlueprintPure)]
    def static Label(value:string):string { return value+"!" }
    [ue_function(BlueprintPure)]
    def static Precise(value:double):double { return value+0.25lf }
    [ue_function(BlueprintPure)]
    def static EchoName(value:UENameValue):UENameValue { return value }
    [ue_function(BlueprintPure)]
    def static Ping():void {}
    [ue_function(BlueprintCallable)]
    def static Fail(var counter:int&; @ue_out var doubled:int&):void { counter=999;doubled=888;panic("library_expected_failure") }
}
