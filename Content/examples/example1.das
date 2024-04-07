//options log=true
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
struct SaySomethingGeneratedParams
    a : int

def SaySomethingGenerated(var obj: UObject?)
    unsafe
        var params: SaySomethingGeneratedParams //no inited, unsafe
        params.a = 42
        obj |> call_ufunction_on("SaySomethingGenerated") <| addr(params)

//--------------------------------------------------------------


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

    //print("{cls}\n") //TODO: fix crash

    //1
    //obj |> SaySomething(FString("SaySomething"))

    //2
    //obj |> SaySomethingElse(FString("SaySomethingElse"))

    //3 call without macro
    obj |> SaySomething(FString("SaySomething"))

    //---
    //new_uobject by type
    var uclassObject = new_uobject(type<UClass>)
    print("uclassObject: {string(*uclassObject |> GetName)}\n")

    //----
    //create function
    let fn = cls |> test_emit_ufunction() <| FName("SaySomethingGenerated")
    print("Generated function name: {string(*fn |> GetName)}\n")

    //check if generated function really has int parameter
    let param = fn |> GetChildFieldsFromStruct_FField
    print("Generated function parameter name: {string(*param |> GetName)}\n")

    obj |> SaySomethingGenerated
