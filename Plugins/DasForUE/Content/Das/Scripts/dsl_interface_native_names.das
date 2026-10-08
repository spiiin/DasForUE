options gen2
options gc
require ue_dsl

[ue_class(module_id="interfaces", implements=DasProbeInterface, Blueprintable)]
class InterfaceActor {
    [ue_function(BlueprintCallable)]
    def Query(Input:int):int { return Input + 7 }
    [ue_function(BlueprintCallable)]
    def Label():string { return "Interface 世界" }
    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> {
        var result <- clone(Values)
        push(result, 1.23456789012345lf)
        return <- result
    }
    [ue_function(BlueprintCallable, ref="Counter", out="Total")]
    def Stats(var Counter:int&; var Total:int&):void { Counter += 3; Total = Counter * 2 }
    [ue_function(BlueprintCallable)]
    def Implements(Target:UObjectHandle):bool { return ue_implements_interface(Target, "/Script/DasRuntime.DasProbeInterface") }
    [ue_function(BlueprintCallable)]
    def InterfaceQuery(Target:UObjectHandle; Value:int):int { return ue_interface_call_int(Target, "/Script/DasRuntime.DasProbeInterface", "Query", Value) }
    [ue_function(BlueprintCallable)]
    def InterfaceLabel(Target:UObjectHandle):string { return ue_interface_call_string(Target, "/Script/DasRuntime.DasProbeInterface", "Label") }
    [ue_function(BlueprintCallable)]
    def BadInterface(Target:UObjectHandle):bool { return ue_implements_interface(Target, "/Script/Engine.Actor") }
    [ue_function(BlueprintCallable)]
    def BadMethod(Target:UObjectHandle):int { return ue_interface_call_int(Target, "/Script/DasRuntime.DasProbeInterface", "Missing", 1) }
    [ue_function(BlueprintCallable)]
    def TypedQuery(Target:UObjectHandle; Input:int):int {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasProbeInterface", "Query")
        ue_arg_int(call, "Input", Input)
        ue_invoke_call(call)
        let result = ue_result_int(call, "ReturnValue")
        ue_release_call(call)
        return result
    }
    [ue_function(BlueprintCallable)]
    def TypedWeights(Target:UObjectHandle):double {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasProbeInterface", "Weights")
        ue_arg_array(call, "Values", 2)
        ue_arg_double(call, "Values.0", 2.5lf)
        ue_arg_double(call, "Values.1", 1.23456789012345lf)
        ue_invoke_call(call)
        let count = ue_result_array_size(call, "ReturnValue")
        let result = ue_result_double(call, "ReturnValue.0") + ue_result_double(call, "ReturnValue.1")
        ue_release_call(call)
        return result + double(count)
    }
    [ue_function(BlueprintCallable)]
    def TypedStats(Target:UObjectHandle):int {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasProbeInterface", "Stats")
        ue_arg_int(call, "Counter", 10)
        ue_invoke_call(call)
        let result = ue_result_int(call, "Counter") + ue_result_int(call, "Total")
        ue_release_call(call)
        return result
    }
    [ue_function(BlueprintCallable)]
    def TypedMissingInput(Target:UObjectHandle):int {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasProbeInterface", "Query")
        ue_invoke_call(call)
        return 0
    }
    [ue_function(BlueprintCallable)]
    def TypedWrongScalar(Target:UObjectHandle):int {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasProbeInterface", "Query")
        ue_arg_double(call, "Input", 2.5lf)
        return 0
    }
    [ue_function(BlueprintCallable)]
    def TypedReleased(Target:UObjectHandle):int {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasProbeInterface", "Query")
        ue_release_call(call)
        ue_arg_int(call, "Input", 1)
        return 0
    }
    [ue_function(BlueprintCallable)]
    def TypedMap(Target:UObjectHandle):string {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasCollectionInterface", "EchoMap")
        ue_arg_hash(call, "Values", 1)
        ue_arg_int(call, "Values.0.key", -3)
        ue_arg_string(call, "Values.0.value", "Map 世界")
        ue_invoke_call(call)
        let result = ue_result_string(call, "ReturnValue.0.value")
        ue_release_call(call)
        return result
    }
    [ue_function(BlueprintCallable)]
    def TypedSet(Target:UObjectHandle):int {
        let call = ue_begin_interface_call(Target, "/Script/DasRuntime.DasCollectionInterface", "EchoSet")
        ue_arg_hash(call, "Values", 1)
        ue_arg_int(call, "Values.0", 31)
        ue_invoke_call(call)
        let result = ue_result_int(call, "ReturnValue.0")
        ue_release_call(call)
        return result
    }
}
