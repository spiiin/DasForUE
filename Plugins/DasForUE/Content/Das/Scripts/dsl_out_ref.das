options gen2
options gc
require ue_dsl

[ue_class(module_id="outref", Blueprintable)]
class OutActor {
    [ue_function(BlueprintCallable)]
    def Adjust(@ue_ref var counter:int&; @ue_out var total:int&; amount:int):int {
        counter += amount
        total += counter * 2
        return counter
    }
    [ue_function(BlueprintCallable)]
    def Change(var numbers:array<double>&; var tags:table<string>&; var weights:table<int;bool>&; @ue_out var message:string&):void {
        push(numbers, 1.23456789012345lf)
        insert(tags, "世界")
        weights[-3] = true
        message = "Hello 世界"
    }
    [ue_function(BlueprintCallable)]
    def Fail(var counter:int&; @ue_out var total:int&):void {
        counter = 99
        total = 88
        panic("out fixture failure")
    }
    [ue_function(BlueprintCallable)]
    def RefInt(var value:int&; @ue_out var output:int&):void { value = 31; output = value }
    [ue_function(BlueprintCallable)]
    def RefFloat(var value:float&; @ue_out var output:float&):void { value = 1.25; output = value }
    [ue_function(BlueprintCallable)]
    def RefDouble(var value:double&; @ue_out var output:double&):void { value = 1.23456789012345lf; output = value }
    [ue_function(BlueprintCallable)]
    def RefBool(var value:bool&; @ue_out var output:bool&):void { value = true; output = value }
    [ue_function(BlueprintCallable)]
    def RefString(var value:string&; @ue_out var output:string&):void { value = "Hello 世界"; output = value }
    [ue_function(BlueprintCallable)]
    def RefName(var value:UENameValue&; @ue_out var output:UENameValue&):void { value = ue_name_value("Key_17"); output = value }
    [ue_function(BlueprintCallable)]
    def RefArrayInt(var value:array<int>&; @ue_out var output:array<int>&):void { push(value, 31); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefArrayFloat(var value:array<float>&; @ue_out var output:array<float>&):void { push(value, 1.25); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefArrayDouble(var value:array<double>&; @ue_out var output:array<double>&):void { push(value, 1.23456789012345lf); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefArrayBool(var value:array<bool>&; @ue_out var output:array<bool>&):void { push(value, true); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefArrayString(var value:array<string>&; @ue_out var output:array<string>&):void { push(value, "Hello 世界"); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefArrayName(var value:array<UENameValue>&; @ue_out var output:array<UENameValue>&):void { push(value, ue_name_value("Key_17")); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefSetInt(var value:table<int>&; @ue_out var output:table<int>&):void { insert(value, 31); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefSetString(var value:table<string>&; @ue_out var output:table<string>&):void { insert(value, "Hello 世界"); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapIntInt(var value:table<int;int>&; @ue_out var output:table<int;int>&):void { value[-3] = 31; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapIntFloat(var value:table<int;float>&; @ue_out var output:table<int;float>&):void { value[-3] = 1.25; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapIntDouble(var value:table<int;double>&; @ue_out var output:table<int;double>&):void { value[-3] = 1.23456789012345lf; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapIntBool(var value:table<int;bool>&; @ue_out var output:table<int;bool>&):void { value[-3] = true; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapIntString(var value:table<int;string>&; @ue_out var output:table<int;string>&):void { value[-3] = "Hello 世界"; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapIntName(var value:table<int;UENameValue>&; @ue_out var output:table<int;UENameValue>&):void { value[-3] = ue_name_value("Key_17"); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapStringInt(var value:table<string;int>&; @ue_out var output:table<string;int>&):void { value["Hello 世界"] = 31; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapStringFloat(var value:table<string;float>&; @ue_out var output:table<string;float>&):void { value["Hello 世界"] = 1.25; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapStringDouble(var value:table<string;double>&; @ue_out var output:table<string;double>&):void { value["Hello 世界"] = 1.23456789012345lf; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapStringBool(var value:table<string;bool>&; @ue_out var output:table<string;bool>&):void { value["Hello 世界"] = true; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapStringString(var value:table<string;string>&; @ue_out var output:table<string;string>&):void { value["Hello 世界"] = "Hello 世界"; output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def RefMapStringName(var value:table<string;UENameValue>&; @ue_out var output:table<string;UENameValue>&):void { value["Hello 世界"] = ue_name_value("Key_17"); output <- clone(value) }
    [ue_function(BlueprintCallable)]
    def LateFail(var counter:int&; @ue_out var numbers:array<int>&):void {
        counter = 99
        for (index in range(4097)) { push(numbers, index) }
    }
}
