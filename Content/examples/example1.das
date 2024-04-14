require unreal_boost
//require unreal_boost_internal

//--------------------------------------------------------------
//1. creating unreal function-wrapper - def SaySomething(var obj: UObject?; msg : FString)
/*[unreal_call_params(name="SaySomething")]
struct SaySomethingParamsMacro
    msg : FString*/

//--------------------------------------------------------------
//2. other way to declare unreal function wrapper
/*[unreal_call]
def SaySomethingElse(obj: UObject?; var msg: FString)
    panic("Somehow we are in this is temporary stub for the resolved SaySomethingElse function")*/

//--------------------------------------------------------------
//3. make wrapper call without macros
struct SaySomethingParams
    msg : FString

def SaySomething(var obj: UObject?; var msg: FString)
    unsafe
        var params: SaySomethingParams //no inited, unsafe
        params.msg <- msg
        obj |> call_ufunction_on("SaySomething") <| addr(params)


//--------------------------------------------------------------
struct SaySomethingParams4
    a : int
    msg : FString
    f: float
    g: int
    result : FString

def SaySomething4(var obj: UObject?; a:int; var msg: FString; f: float; g: int)
    unsafe
        var params: SaySomethingParams4 //no inited, unsafe
        params.a = a
        params.msg <- msg
        params.f = f
        params.g = g
        obj |> call_ufunction_on("SaySomething4") <| addr(params)
        print("Call result: {string(params.result)}\n")

//--------------------------------------------------------------
struct SayArrayParams
    a: TArray_int

def SayArray(var obj: UObject?; var a: TArray_int)
    unsafe
        var params: SayArrayParams //no inited, unsafe
        params.a <- a
        obj |> call_ufunction_on("SayArray") <| addr(params)

//--------------------------------------------------------------
struct SaySomethingGeneratedParams
    a : int
    msg : FString
    f: float
    g: int
    res : FString

def SaySomethingGenerated(var obj: UObject?; a: int; var msg: FString; f: float; g: int) : FString
    unsafe
        var params: SaySomethingGeneratedParams //no inited, unsafe
        params.a = a
        params.msg <- msg
        params.f = f
        params.g = g
        obj |> call_ufunction_on("execSaySomethingGenerated") <| addr(params)
        return params.res

//--------------------------------------------------------------
[export]
def on_after_compile()
    init_script_package()

    //create new properties in class
    /*let className = "TestClass_BP_C"
    var cls = get_class_by_name(FString(className))
    if cls == null
        print("Class {className} not found!\n")
        return
    var updatedClass = emit_properties(cls) <| @(owner:UObject?): array<FProperty?>
        return <- [{auto
            newFIntProperty(owner, "Property1", propFlagsField);
            //newFStringProperty(owner, "Property2", propFlagsField);
            newFFloatProperty(owner, "Property3", propFlagsField)
        }]
    if updatedClass == null
        print("Can't emit properties for class {className}\n")
    [[FAssetRegistryModule?]] |> AssetCreated(updatedClass)*/

    //---
    var emittedClass = test_emit_class(get_script_package(), "DasActor", "Actor")
    print("Emitted class: {string(*emittedClass |> GetName)}\n")
    BroadcastAssets()


[export]
def main
    let className = "TestClass"
    var cls = get_class_by_name(FString(className))
    if cls == null
        print("Class {className} not found!")
        return
    print("Class name: {string(*cls |> GetName)}\n")
    var obj = new_uobject_of_class(cls)
    print("Object name: {string(*obj |> GetName)}\n")

    //1
    //obj |> SaySomething(FString("SaySomething"))

    //2
    //obj |> SaySomethingElse(FString("SaySomethingElse"))

    //3 call existing functions, without macro
    obj |> SaySomething(FString("SaySomething"))
    obj |> SaySomething4(42, FString("SaySomething"), 3.14f, 33)
    obj |> SayArray(TArray_int([{int 1;2;3}]))

    //---
    //new_uobject by type
    var uclassObject = new_uobject(type<UClass>)
    print("uclassObject: {string(*uclassObject |> GetName)}\n")

    //---
    //generate new function and call it
    let createPropertiesCb <- @ <|(owner:UObject?): array<FProperty?>
        var props: array<FProperty?>
        props |> push <| newFIntProperty(owner, "Parameter1", propFlagsParam)
        props |> push <| newFStringProperty(owner, "Parameter2", propFlagsParam)
        props |> push <| newFFloatProperty(owner, "Parameter3", propFlagsParam)
        props |> push <| newFIntProperty(owner, "Parameter4", propFlagsParam)
        props |> push <| newFStringProperty(owner, "Parameter5", propFlagsReturn)
        return <- props

    var nativeFuncPtr <- @ <| (var context: UObject?; var stack: FFrame; param: void?)
        print("In generated function\n")

        /*print("stack.Node {stack.Node}\n")
        print("stack.Object {stack.Object}\n")
        print("stack.Code {stack.Code}\n")
        print("stack.Locals {stack.Locals}\n")
        print("stack.PropertyChainForCompiledIn {stack.PropertyChainForCompiledIn}\n")
        print("stack.CurrentNativeFunction {stack.CurrentNativeFunction}\n")*/

        var fn = stack.Node
        print("  Generated function ParmsSize = {int(fn.ParmsSize)}\n")
        print("  Generated function ReturnValueOffset = {int(fn.ReturnValueOffset)}\n")

        //let obj = unsafe(reinterpret<UMyClassToTestPtr?> context)

        unsafe
            print("  Parameters values:\n")
            //is GetDefaultPropertyValue really need?

            //read parameters by one
            var params: SaySomethingGeneratedParams
            stack |> StepCompiledIn(addr(params.a), [[FIntProperty?]] |> StaticClass)
            stack |> StepCompiledIn(addr(params.msg), [[FStrProperty?]] |> StaticClass)
            stack |> StepCompiledIn(addr(params.f), [[FFloatProperty?]] |> StaticClass)
            stack |> StepCompiledIn(addr(params.g), [[FIntProperty?]] |> StaticClass)
            stack |> StepCompiledIn(addr(params.res), [[FStrProperty?]] |> StaticClass)
            print("    StepCompiledIn params: {params}\n")
            print("    StepCompiledIn result: {string(params.res)}\n") //zero, because of out/result

            //other way - just reinterpret, locals is memcpy-ed copy of param (Object::ProcessEvents)
            //TODO: assert sizeof(SaySomethingGeneratedParams1) == sizeof(fn.ParmsSize)
            var locals = reinterpret<SaySomethingGeneratedParams?>(stack.Locals)
            print("    Stack locals: {locals}\n")
            print("    Stack locals str: {string(locals.res)}\n")

            //param is addr of CPF_ReturnParm
            var result = reinterpret<FString?>(param)
            *result <- FString("ResultOfGeneratedFunction")
            print("    Result str: {string(*result)}\n")
            
            //TODO: fill out parameters logic

            if stack.Code != null
                stack.Code++

    //----
    //create function
    let fn = cls |> emit_ufunction(FName("execSaySomethingGenerated"), createPropertiesCb, nativeFuncPtr)
    print("Generated function name: {string(*fn |> GetName)}\n")
    print("Generated function result: {string(obj |> SaySomethingGenerated(42, FString("SaySomethingGenerated"), 3.14f, 33))}\n")
