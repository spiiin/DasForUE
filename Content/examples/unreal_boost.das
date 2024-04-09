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

struct SaySomethingGeneratedParams1
    g:float
    f: float
    msg : FString
    a : int

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
    *fn |> AddCppProperty(unsafe(reinterpret<FProperty?> field1))

    var field2 = Construct(
        [[FStrProperty?]],
        FFieldVariant(fn),
        FName("Parameter2"),
        propertyObjFlags 
    )
    *fn |> AddCppProperty(unsafe(reinterpret<FProperty?> field2))

    var field3 = Construct(
        [[FFloatProperty?]], //c++ idiom - explicit typed nullptr just to select correct overload from functions, ugly in das
        FFieldVariant(fn),
        FName("Parameter3"),
        propertyObjFlags 
    )
    *fn |> AddCppProperty(unsafe(reinterpret<FProperty?> field3))

    var field4 = Construct(
        [[FFloatProperty?]], //c++ idiom - explicit typed nullptr just to select correct overload from functions, ugly in das
        FFieldVariant(fn),
        FName("Parameter4"),
        propertyObjFlags 
    )
    *fn |> AddCppProperty(unsafe(reinterpret<FProperty?> field4))

    //fill class 
    *cls |> AddFunctionToFunctionMap(fn, name)
    //TODO: create new field in class
    //  fn.Next = cls.Children
    //  cls.Children = fn

    //create function body
    fn |> SetNativeFunc <| @(var context: UObject?; var stack: FFrame; param: void?)
        //init debug
        print("In generated funcion\n")
        let selfFunction = stack.Node

        /*let param4f = selfFunction |> GetChildFieldsFromStruct_FField
        print("  Generated function parameter4 name: {string(*param4f |> GetName)}\n")
        let param3f = param4f.Next //stack
        print("  Generated function parameter3 name: {string(*param3f |> GetName)}\n")
        let param2f = param3f.Next //stack
        print("  Generated function parameter2 name: {string(*param2f |> GetName)}\n")
        let param1f = param2f.Next //stack
        print("  Generated function parameter1 name: {string(*param1f |> GetName)}\n")*/

        print("  Generated function ParmsSize = {fn.ParmsSize}\n")

        print("ppp {stack.Node}\n")
        print("ppp {stack.Object}\n")
        print("ppp {stack.Code}\n")
        print("ppp {stack.Locals}\n")
        print("ppp {stack.PropertyChainForCompiledIn}\n")
        print("ppp {stack.CurrentNativeFunction}\n")

        unsafe
            var f = reinterpret<FProperty?> stack.PropertyChainForCompiledIn
            print("PropertyChainForCompiledIn name: {string(*f |> GetName)}\n")

        //let obj = unsafe(reinterpret<UMyClassToTestPtr?> context)

        //prolog
        unsafe
            print("  Parameters values:\n")
            //is GetDefaultPropertyValue really need?

            /*var param1 = 0 //GetDefaultPropertyValue([[FIntProperty?]])
            stack |> StepCompiledIn(addr(param1), [[FIntProperty?]] |> StaticClass)
            print("    {string(*param1f |> GetName)} : {param1}\n")*/

            /*var param2 : FString //GetDefaultPropertyValue([[FStrProperty?]])
            stack |> StepCompiledIn(addr(param2), [[FStrProperty?]] |> StaticClass)
            print("    {string(*param2f |> GetName)} : {string(param2)}\n")*/

            /*var param3 = 0.f //GetDefaultPropertyValue([[FFloatProperty?]])
            stack |> StepCompiledIn(addr(param3), [[FFloatProperty?]] |> StaticClass)
            print("    {string(*param3f |> GetName)} : {param3}\n")*/


            var prop1Ptr = reinterpret<FProperty?> stack.PropertyChainForCompiledIn
            print("    PropertyChainForCompiledIn current name: {string(*prop1Ptr |> GetName)}\n")

            var param4 = 33.f //GetDefaultPropertyValue([[FFloatProperty?]])
            stack |> StepCompiledIn(addr(param4), [[FFloatProperty?]] |> StaticClass)
            print("        StepCompiledIn : {param4}\n")

            var prop2Ptr = reinterpret<FProperty?> stack.PropertyChainForCompiledIn
            print("    PropertyChainForCompiledIn current name: {string(*prop2Ptr |> GetName)}\n")

            let params = reinterpret<SaySomethingGeneratedParams1?>(stack.Locals)
            print("    Stack locals: {params.a}\n")

            if stack.Code != null //stack.Code += !!stack.Code
                stack.Code++
            
            
        //epilogue
        //*((FVector*)(Z_Param__Result)) = Result;
        
    *fn |> StaticLink(true)
    print("  Generated function ParmsSize = {fn.ParmsSize}\n")
    return fn
