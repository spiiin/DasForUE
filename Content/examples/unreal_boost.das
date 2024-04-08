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

def new_uobject_of_class(owner: UObject?; cls: UClass?; name: FName; flags: EObjectFlags = EObjectFlags RF_NoFlags) : UObject?
    var result : UObject?
    using(cls) <| $(var params: FStaticConstructObjectParameters explicit)
        params.Outer = owner //?? unsafe(reinterpret<UObject?> GetTransientPackage())
        params.Name = name
        params.SetFlags = flags
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

def new_uobject(owner: UObject?; name: FName; flags: EObjectFlags; objType: auto(TT)-const) : TT?
    let className = typeinfo(stripped_typename objType)
    static_assert(className |> starts_with("unreal::"))
    let cls = get_class_by_name(FString(className |> slice(9))) //remove unreal::A/U/F unreal type prefix
    return null if cls == null
    unsafe { return reinterpret<TT?> new_uobject_of_class(owner, cls, name, flags); }


def new_uobject(objType: auto(TT)-const) : TT?
    let className = typeinfo(stripped_typename objType) 
    static_assert(className |> starts_with("unreal::"))
    let cls = get_class_by_name(FString(className |> slice(9))) //remove unreal::A/U/F unreal type prefix
    return null if cls == null
    unsafe { return reinterpret<TT?> new_uobject_of_class(cls); }

let private propertyObjFlags = EObjectFlags RF_Public | EObjectFlags RF_Transient | EObjectFlags RF_MarkAsNative
let private funcObjFlags = EObjectFlags RF_Public | EObjectFlags RF_Transient | EObjectFlags RF_MarkAsRootSet | EObjectFlags RF_MarkAsNative
let private funcFlags = (
    EFunctionFlags FUNC_Native |
    EFunctionFlags FUNC_Public
)

def test_emit_ufunction(var cls: UClass?; var name: FName)
    //check if function with name already exist in this class or it's superclasses
    //  TODO:

    //create new function
    var fn = new_uobject(cls, name, funcObjFlags, type<UFunction>)
    fn.FunctionFlags = funcFlags

    //create function arguments
    var field1 = Construct(
        [[FIntProperty?]], //c++ idiom - explicit typed nullptr just to select correct overload from functions, ugly in das
        FFieldVariant(fn),
        FName("Parameter1"),
        propertyObjFlags 
    )
    var propPtr1 = unsafe(reinterpret<FProperty?> field1) //TODO: find a way to construct field with something return FProperty?
    *fn |> AddCppProperty(propPtr1)

    var field2 = Construct(
        [[FStrProperty?]],
        FFieldVariant(fn),
        FName("Parameter2"),
        propertyObjFlags 
    )
    var propPtr2 = unsafe(reinterpret<FProperty?> field2) //TODO: find a way to construct field with something return FProperty?
    *fn |> AddCppProperty(propPtr2)

    //fill class 
    *cls |> AddFunctionToFunctionMap(fn, name)
    //TODO: create new field in class
    //  fn.Next = cls.Children
    //  cls.Children = fn

    //create function body
    fn |> SetNativeFunc <| @(var context: UObject?; var stack: FFrame; param: void?)
        print("In native func\n")
        let fn = stack.Node
        let param2 = fn |> GetChildFieldsFromStruct_FField
        print("Generated function parameter2 name: {string(*param2 |> GetName)}\n")
        let param1 = param2.Next //stack
        print("Generated function parameter1 name: {string(*param1 |> GetName)}\n")
        //let obj = unsafe(reinterpret<UMyClassToTestPtr?> context)
    *fn |> StaticLink(true)
    return fn
