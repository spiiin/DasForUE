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

def new_uobject(owner: UObject?; name: FName; objType: auto(TT)-const) : TT?
    let className = typeinfo(stripped_typename objType)
    static_assert(className |> starts_with("unreal::"))
    let cls = get_class_by_name(FString(className |> slice(9))) //remove unreal::A/U/F unreal type prefix
    return null if cls == null
    unsafe { return reinterpret<TT?> new_uobject_of_class(owner, cls, name); }


def new_uobject(objType: auto(TT)-const) : TT?
    let className = typeinfo(stripped_typename objType) 
    static_assert(className |> starts_with("unreal::"))
    let cls = get_class_by_name(FString(className |> slice(9))) //remove unreal::A/U/F unreal type prefix
    return null if cls == null
    unsafe { return reinterpret<TT?> new_uobject_of_class(cls); }

let private propertyFlags = EObjectFlags RF_Public | EObjectFlags RF_Transient | EObjectFlags RF_MarkAsNative

def test_emit_ufunction(var cls: UClass?; var name: FName)
    //check if function with name already exist in this class or it's superclasses
    //  TODO:

    //create new function
    var fn = new_uobject(cls, name, type<UFunction>)
    var field = Construct(
        [[FIntProperty?]], //c++ idiom - explicit typed nullptr just to select correct overload from functions, ugly in das
        FFieldVariant(fn),
        FName("Parameter1"),
        propertyFlags 
    )

    //create function arguments
    var propPtr = unsafe(reinterpret<FProperty?> field) //TODO: find a way to construct field with something return FProperty?
    *fn |> AddCppProperty(propPtr)
    //fn.functionFlags

    //fill class 
    *cls |> AddFunctionToFunctionMap(fn, name)
    //TODO: create new field in class
    //  fn.Next = cls.Children
    //  cls.Children = fn

    //create function body
    //  fn |> SetNativeFunc(ptr)
    //  fn |> StaticLink(true)
    return fn
