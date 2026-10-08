options gen2
require ue_dsl
[ue_class(module_id="paramdirections", Blueprintable)]
class Counter {
    [ue_function(BlueprintCallable)]
    def Adjust(var counter:int&; @ue_ouut var total:int&; amount:int):int {
        counter += amount
        total = counter * 2
        return counter
    }
    [ue_function(BlueprintCallable)]
    def Bump(var value:int&):int { value += 1; return value }
}
