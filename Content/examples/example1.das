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

def SaySomething(obj: UObject?; var msg: FString)
    unsafe
        var params: SaySomethingParams //no inited, unsafe
        params.msg <- msg
        obj |> call_ufunction_on(FString("SaySomething")) <| addr(params)

//--------------------------------------------------------------

[export]
def main
    let className = "TestClass"
    var clazz = get_class_by_name(className)
    if clazz == null
        print("Class {className} not found!")
        return
    print("Class name: {string(*clazz |> GetName)}\n")
    var obj = new_object_from_class(clazz)
    print("Object name: {string(*obj |> GetName)}\n")

    //print("{cls}\n") //TODO: fix crash

    //1
    //obj |> SaySomething(FString("SaySomething"))

    //2
    //obj |> SaySomethingElse(FString("SaySomethingElse"))

    //3 call without macro
    obj |> SaySomething(FString("SaySomething"))

    //print("{clazz}\n")
