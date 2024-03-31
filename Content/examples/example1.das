//options log=true
require unreal
require unreal_boost_internal

//--------------------------------------------------------------
//1. creating unreal function-wrapper - def SaySomething(var obj: UObject?; msg : FString)
[unreal_call_params(name="SaySomething")]
struct SaySomethingParams
    msg : FString

//--------------------------------------------------------------
//2. other way to declare unreal function wrapper
[unreal_call]
def SaySomethingElse(obj: UObject?; var msg: FString)
    panic("Somehow we are in this is temporary stub for the resolved SaySomethingElse function")

//--------------------------------------------------------------
[export]
def main
    let clazz = get_class_by_name(FString("TestClass"))
    var obj = new_object_from_class(FStaticConstructObjectParameters(clazz))
    obj |> SaySomething(FString("SaySomething"))
    obj |> SaySomethingElse(FString("SaySomethingElse"))
    print("das_root: {get_das_root()}\n")
