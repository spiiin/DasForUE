module unreal_boost //shared

require unreal public
require daslib/templates
require strings

def get_class_by_name(className: FString) : UClass?
    var obj = StaticFindObject(
        StaticClass([[UObject?]]),
        AnyPackage(),
        className |> ToCStr()
    )
    unsafe { return reinterpret<UClass?> obj; }


def new_uobject_of_class(cls: UClass?) : UObject?
    return StaticConstructObject_Internal(FStaticConstructObjectParameters(cls))

def new_uobject_of_class(owner: UObject?; cls: UClass?; name: FName) : UObject?
    var result : UObject?
    using(cls) <| $(var params: FStaticConstructObjectParameters explicit)
        params.Outer = owner //?? unsafe(reinterpret<UObject?> GetTransientPackage())
        params.Name = name
        params.SetFlags = EObjectFlags RF_NoFlags
        result = StaticConstructObject_Internal(params)
    return result


def call_ufunction_on(var executor: UObject?; functionName: string; var inParams: void?)
    let clazz = *executor |> GetClass
    //find function, can be separated
    let func = *clazz |> FindFunctionByName(FName(functionName))
    if func == null
        print("Function {functionName} not found in class {string(*clazz |> GetName)}\n")
        return
    //TODO:
    // - process result/out parameters
    // - support unreal types
    *executor |> ProcessEvent(func, inParams)


//UObjects constructors

def new_uobject(owner: UObject?; name: FName; objType: auto(TT)) : TT?
    let className = typeinfo(stripped_typename objType)
    static_assert(className |> starts_with("unreal::"))
    let cls = get_class_by_name(FString(className |> slice(9))) //remove unreal::A/U/F unreal type prefix
    return null if cls == null
    unsafe { return reinterpret<TT?> new_uobject_of_class(owner, cls, name); }


def new_uobject(objType: auto(TT)) : TT?
    let className = typeinfo(stripped_typename objType) 
    static_assert(className |> starts_with("unreal::"))
    let cls = get_class_by_name(FString(className |> slice(9))) //remove unreal::A/U/F unreal type prefix
    return null if cls == null
    unsafe { return reinterpret<TT?> new_uobject_of_class(cls); }


def emit_ufunction(cls: UClass?; name: FName)
    //RF_Public | RF_Transient | RF_MarkAsRootSet | RF_MarkAsNative
    var fn = new_uobject(cls, name, type<UFunction>)
    //fn.functionFlags
    //TODO: logic for super
    //fn.Next = cls.Children
    //cls.Children = fn
    //foreach ARG
        //fn |> emit_fproperty(ARG)
    //cls |> AddFunctionToFunctionMap(fn, name)
    //fn |> SetNativeFunc
    //fn |> StaticLink(true)
    return fn
