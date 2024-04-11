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
    a: TArray_int32

def SayArray(var obj: UObject?; var a: TArray_int32)
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

def SaySomethingGenerated(var obj: UObject?; a: int; var msg: FString; f: float; g: int; var res: FString)
    unsafe
        var params: SaySomethingGeneratedParams //no inited, unsafe
        params.a = a
        params.msg <- msg
        params.f = f
        params.g = g
        params.res <- res
        obj |> call_ufunction_on("SaySomethingGenerated") <| addr(params)
        print("  Params after call SaySomethingGenerated: {params}\n")
        print("  Params.res after call SaySomethingGenerated: {string(params.res)}\n")

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

    obj |> SaySomething4(42, FString("SaySomething"), 3.14f, 33)

    obj |> SayArray(make_TArray_int32())

    //---
    //new_uobject by type
    var uclassObject = new_uobject(type<UClass>)
    print("uclassObject: {string(*uclassObject |> GetName)}\n")

    //----
    //create function
    let fn = cls |> test_emit_ufunction() <| FName("SaySomethingGenerated")
    print("Generated function name: {string(*fn |> GetName)}\n")

    obj |> SaySomethingGenerated(42, FString("SaySomethingGenerated"), 3.14f, 33, FString("---"))
