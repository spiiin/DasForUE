options gen2
options gc
require ue_dsl

[ue_class(module_id="containers", Blueprintable)]
class ContainerActor {
    [ue_function(BlueprintCallable)]
    def EchoArrayInt(input:array<int>):array<int> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoArrayFloat(input:array<float>):array<float> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoArrayDouble(input:array<double>):array<double> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoArrayBool(input:array<bool>):array<bool> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoArrayString(input:array<string>):array<string> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoArrayName(input:array<UENameValue>):array<UENameValue> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoSetInt(input:table<int>):table<int> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoSetString(input:table<string>):table<string> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapIntInt(input:table<int; int>):table<int; int> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapIntFloat(input:table<int; float>):table<int; float> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapIntDouble(input:table<int; double>):table<int; double> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapIntBool(input:table<int; bool>):table<int; bool> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapIntString(input:table<int; string>):table<int; string> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapIntName(input:table<int; UENameValue>):table<int; UENameValue> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapStringInt(input:table<string; int>):table<string; int> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapStringFloat(input:table<string; float>):table<string; float> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapStringDouble(input:table<string; double>):table<string; double> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapStringBool(input:table<string; bool>):table<string; bool> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapStringString(input:table<string; string>):table<string; string> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def EchoMapStringName(input:table<string; UENameValue>):table<string; UENameValue> { var result <- clone(input); return <- result }
    [ue_function(BlueprintCallable)]
    def Mixed(numbers:array<double>; tags:table<string>; weights:table<int; float>; offset:int):double {
        var total = double(offset)
        for (value in numbers) { total += value }
        for (weight in values(weights)) { total += double(weight) }
        return total + double(length(tags))
    }
}
