module unreal_boost //shared

require unreal public

def get_class_by_name(className: string) : UClass?
    var obj = StaticFindObject(
        StaticClass([[UObject?]]),
        AnyPackage(),
        FString(className) |> ToCStr()
    )
    unsafe { return reinterpret<UClass?> obj; }


def new_object_from_class(clazz: UClass?) : UObject?
    return StaticConstructObject_Internal(FStaticConstructObjectParameters(clazz))


def call_ufunction_on(var executor: UObject?; functionName: string; var inParams: void?)
    let clazz = *executor |> GetClass
    //find function, can be separated
    let func = *clazz |> FindFunctionByName(FName(functionName))
    if func == null
        print("Function {functionName} not found in class {string(*clazz |> GetName)}\n")
        return
    //call function
    *executor |> ProcessEvent(func, inParams)