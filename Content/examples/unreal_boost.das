module unreal_boost //shared

require unreal public
require daslib/templates
require daslib/contracts
require daslib/apply
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

[sideeffects]
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
    print("Func.ParmsSize {int(func.ParmsSize)}\n")
    *executor |> ProcessEvent(func, inParams)

//TODO: better to have macro parse all arguments
[expect_any_tuple(arguments) || expect_any_struct(arguments)]
def call_ufunction(var executor: UObject?; functionName: string; var arguments: auto(TT))
    let clazz = *executor |> GetClass
    //find function, can be separated
    let func = *clazz |> FindFunctionByName(FName(functionName))
    if func == null
        print("Function {functionName} not found in class {string(*clazz |> GetName)}\n")
        return
    //TODO:
    // - process result/out parameters
    unsafe
        var buf: array<uint8>
        buf |> resize_no_init(int(func.ParmsSize))
        apply(arguments) <| $(name:string; field)
            print("{name} = {field} ")
        //*executor |> ProcessEvent(func, buf)


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
let private funcObjFlags = EObjectFlags RF_Public | EObjectFlags RF_Transient | EObjectFlags RF_MarkAsNative
let private funcFlags = (
    EFunctionFlags FUNC_Native |
    EFunctionFlags FUNC_Public |
    EFunctionFlags FUNC_BlueprintCallable |
    EFunctionFlags FUNC_HasOutParms //if function return something
)
let private propFlags = EPropertyFlags CPF_Parm //| EPropertyFlags CPF_NativeAccessSpecifierPublic
let private propFlagsReturn = EPropertyFlags CPF_ReturnParm
let private propFlagsOut = EPropertyFlags CPF_OutParm

struct SaySomethingGeneratedParams1
    a: int
    msg: FString
    f: float
    g: int
    res: FString

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
    var prop1 = unsafe(reinterpret<FProperty?> field1)
    //let flagsTest = *prop1 |> GetPropertyFlags
    //print("Flags: {flagsTest}\n") //crash
    //prop1.PropertyFlags |= propFlags
    *prop1 |> SetPropertyFlags(propFlags)

    var field2 = Construct(
        [[FStrProperty?]],
        FFieldVariant(fn),
        FName("Parameter2"),
        propertyObjFlags
    )
    var prop2 = unsafe(reinterpret<FProperty?> field2)
    *prop2 |> SetPropertyFlags(propFlags)
    

    var field3 = Construct(
        [[FFloatProperty?]],
        FFieldVariant(fn),
        FName("Parameter3"),
        propertyObjFlags
    )
    var prop3 = unsafe(reinterpret<FProperty?> field3)
    *prop3 |> SetPropertyFlags(propFlags)

    var field4 = Construct(
        [[FIntProperty?]],
        FFieldVariant(fn),
        FName("Parameter4"),
        propertyObjFlags
    )
    var prop4 = unsafe(reinterpret<FProperty?> field4)
    *prop4 |> SetPropertyFlags(propFlags)

    var field5 = Construct(
        [[FStrProperty?]],
        FFieldVariant(fn),
        FName("Parameter5"),
        propertyObjFlags
    )
    var prop5 = unsafe(reinterpret<FProperty?> field5)
    *prop5 |> SetPropertyFlags(propFlags)
    *prop5 |> SetPropertyFlags(propFlagsReturn)
    *prop5 |> SetPropertyFlags(propFlagsOut)

    *fn |> AddCppProperty(prop5)
    *fn |> AddCppProperty(prop4)
    *fn |> AddCppProperty(prop3)
    *fn |> AddCppProperty(prop2)
    *fn |> AddCppProperty(prop1)

    //fill class 
    *cls |> AddFunctionToFunctionMap(fn, name)
    //TODO: create new field in class
    //  fn.Next = cls.Children
    //  cls.Children = fn

    //create function body
    fn |> SetNativeFunc <| @(var context: UObject?; var stack: FFrame; param: void?)
        //init debug
        print("In generated function\n")

        /*
        let selfFunction = stack.Node
        let param4f = selfFunction |> GetChildFieldsFromStruct_FField
        print("  Generated function parameter4 name: {string(*param4f |> GetName)}\n")
        let param3f = param4f.Next //stack
        print("  Generated function parameter3 name: {string(*param3f |> GetName)}\n")
        let param2f = param3f.Next //stack
        print("  Generated function parameter2 name: {string(*param2f |> GetName)}\n")
        let param1f = param2f.Next //stack
        print("  Generated function parameter1 name: {string(*param1f |> GetName)}\n")*/

        print("  Generated function ParmsSize = {int(fn.ParmsSize)}\n")
        print("  Generated function ReturnValueOffset = {int(fn.ReturnValueOffset)}\n")

        print("stack.Node {stack.Node}\n")
        print("stack.Object {stack.Object}\n")
        print("stack.Code {stack.Code}\n")
        print("stack.Locals {stack.Locals}\n")
        print("stack.PropertyChainForCompiledIn {stack.PropertyChainForCompiledIn}\n")
        print("stack.CurrentNativeFunction {stack.CurrentNativeFunction}\n")

        //let obj = unsafe(reinterpret<UMyClassToTestPtr?> context)

        //prolog
        unsafe
            print("  Parameters values:\n")
            //is GetDefaultPropertyValue really need?

            var prop1Ptr = reinterpret<FProperty?> stack.PropertyChainForCompiledIn
            print("    PropertyChainForCompiledIn current name: {string(*prop1Ptr |> GetName)}\n")

            //read parameters by one
            var param1 = 999
            stack |> StepCompiledIn(addr(param1), [[FIntProperty?]] |> StaticClass)
            print("        StepCompiledIn : {param1}\n")

            var param2 : FString
            stack |> StepCompiledIn(addr(param2), [[FStrProperty?]] |> StaticClass)
            print("        StepCompiledIn : {string(param2)}\n")

            var param3 = 999.f
            stack |> StepCompiledIn(addr(param3), [[FFloatProperty?]] |> StaticClass)
            print("        StepCompiledIn : {param3}\n")

            var param4 = 999
            stack |> StepCompiledIn(addr(param4), [[FIntProperty?]] |> StaticClass)
            print("        StepCompiledIn : {param4}\n")

            var param5 : FString
            stack |> StepCompiledIn(addr(param5), [[FStrProperty?]] |> StaticClass)
            print("        StepCompiledIn : {string(param5)}\n")

            //other way - just reinterpret
            //TODO: assert sizeof(SaySomethingGeneratedParams1) == sizeof(fn.ParmsSize)
            var params = reinterpret<SaySomethingGeneratedParams1?>(stack.Locals)

            //TODO: out logic
            //var returnProperty = *fn |> GetReturnProperty
            //var returnPropertyStr = reinterpret<FStrProperty?> returnProperty

            print("    Stack locals: {params}\n")
            print("    Stack locals str: {string(params.res)}\n")

            if stack.Code != null //stack.Code += !!stack.Code
                stack.Code++

    *fn |> StaticLink(true)
    print("  Generated function ParmsSize = {fn.ParmsSize}\n")
    return fn
