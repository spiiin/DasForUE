options gen2
options gc
require ue_dsl

[ue_class(module_id="outref", Blueprintable)]
class OutActor {
    [ue_function(BlueprintCallable, ref=counter, out=total)]
    def Adjust(var counter:int&; var total:int&; amount:int):int {
        counter += amount
        total += counter * 2
        return counter
    }
    [ue_function(BlueprintCallable, ref=(numbers,tags,weights), out=(message,numbers))]
    def Change(var numbers:array<double>&; var tags:table<string>&; var weights:table<int;bool>&; var message:string&):void {
        push(numbers, 1.23456789012345lf)
        insert(tags, "世界")
        weights[-3] = true
        message = "Hello 世界"
    }
    [ue_function(BlueprintCallable, ref=counter, out=total)]
    def Fail(var counter:int&; var total:int&):void {
        counter = 99
        total = 88
        panic("out fixture failure")
    }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefInt(var value:int&; var output:int&):void { value = 31; output = value }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefFloat(var value:float&; var output:float&):void { value = 1.25; output = value }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefDouble(var value:double&; var output:double&):void { value = 1.23456789012345lf; output = value }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefBool(var value:bool&; var output:bool&):void { value = true; output = value }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefString(var value:string&; var output:string&):void { value = "Hello 世界"; output = value }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefName(var value:UENameValue&; var output:UENameValue&):void { value = ue_name_value("Key_17"); output = value }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefArrayInt(var value:array<int>&; var output:array<int>&):void { push(value, 31); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefArrayFloat(var value:array<float>&; var output:array<float>&):void { push(value, 1.25); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefArrayDouble(var value:array<double>&; var output:array<double>&):void { push(value, 1.23456789012345lf); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefArrayBool(var value:array<bool>&; var output:array<bool>&):void { push(value, true); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefArrayString(var value:array<string>&; var output:array<string>&):void { push(value, "Hello 世界"); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefArrayName(var value:array<UENameValue>&; var output:array<UENameValue>&):void { push(value, ue_name_value("Key_17")); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefSetInt(var value:table<int>&; var output:table<int>&):void { insert(value, 31); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefSetString(var value:table<string>&; var output:table<string>&):void { insert(value, "Hello 世界"); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapIntInt(var value:table<int;int>&; var output:table<int;int>&):void { value[-3] = 31; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapIntFloat(var value:table<int;float>&; var output:table<int;float>&):void { value[-3] = 1.25; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapIntDouble(var value:table<int;double>&; var output:table<int;double>&):void { value[-3] = 1.23456789012345lf; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapIntBool(var value:table<int;bool>&; var output:table<int;bool>&):void { value[-3] = true; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapIntString(var value:table<int;string>&; var output:table<int;string>&):void { value[-3] = "Hello 世界"; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapIntName(var value:table<int;UENameValue>&; var output:table<int;UENameValue>&):void { value[-3] = ue_name_value("Key_17"); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapStringInt(var value:table<string;int>&; var output:table<string;int>&):void { value["Hello 世界"] = 31; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapStringFloat(var value:table<string;float>&; var output:table<string;float>&):void { value["Hello 世界"] = 1.25; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapStringDouble(var value:table<string;double>&; var output:table<string;double>&):void { value["Hello 世界"] = 1.23456789012345lf; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapStringBool(var value:table<string;bool>&; var output:table<string;bool>&):void { value["Hello 世界"] = true; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapStringString(var value:table<string;string>&; var output:table<string;string>&):void { value["Hello 世界"] = "Hello 世界"; output <- clone(value) }
    [ue_function(BlueprintCallable, ref="value", out="output")]
    def RefMapStringName(var value:table<string;UENameValue>&; var output:table<string;UENameValue>&):void { value["Hello 世界"] = ue_name_value("Key_17"); output <- clone(value) }
    [ue_function(BlueprintCallable, ref="counter", out="numbers")]
    def LateFail(var counter:int&; var numbers:array<int>&):void {
        counter = 99
        for (index in range(4097)) { push(numbers, index) }
    }
}
