module unreal_boost //shared

require unreal public
require daslib/templates
require daslib/contracts
require daslib/apply
require daslib/algorithm
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
    using(cls) <| $(var params: FStaticConstructObjectParameters#)
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

let propFlagsPublic = EPropertyFlags CPF_NativeAccessSpecifierPublic

let propFlagsField = propFlagsPublic | EPropertyFlags CPF_BlueprintVisible | EPropertyFlags CPF_Edit | EPropertyFlags CPF_ExposeOnSpawn

let propFlagsParam = EPropertyFlags CPF_Parm | propFlagsPublic
let propFlagsOut = propFlagsParam | EPropertyFlags CPF_OutParm
let propFlagsReturn = propFlagsOut | EPropertyFlags CPF_ReturnParm

def newFIntProperty(owner: UObject?; name: string; propFlags: EPropertyFlags) : FProperty?
    var field1 = Construct(
        [[FIntProperty?]],
        FFieldVariant(owner),
        FName(name),
        propertyObjFlags
    )
    var prop = unsafe(reinterpret<FProperty?> field1)
    *prop |> SetPropertyFlags(propFlags)
    return prop

def newFStringProperty(owner: UObject?; name: string; propFlags: EPropertyFlags) : FProperty?
    var field = Construct(
        [[FStrProperty?]],
        FFieldVariant(owner),
        FName(name),
        propertyObjFlags
    )
    var prop = unsafe(reinterpret<FProperty?> field)
    *prop |> SetPropertyFlags(propFlags)
    return prop

def newFFloatProperty(owner: UObject?; name: string; propFlags: EPropertyFlags) : FProperty?
    var field = Construct(
        [[FFloatProperty?]],
        FFieldVariant(owner),
        FName(name),
        propertyObjFlags
    )
    var prop = unsafe(reinterpret<FProperty?> field)
    *prop |> SetPropertyFlags(propFlags)
    return prop


def emit_properties(
    var cls: UStruct?;
    createPropertiesCb: lambda<(owner: UObject?): array<FProperty?>>
)
    var clsPtr = unsafe(reinterpret<UClass?> cls)
    if *clsPtr |> HasAnyClassFlags(EClassFlags CLASS_Intrinsic)
        //class already created
        return null
    var props <- invoke(createPropertiesCb, cls)
    props |> reverse
    for prop in props
        *cls |> AddCppProperty(prop)
    //*cls |> StaticLink(true)

    //
    *cls |> StaticLink(true)
    *clsPtr |> Bind
    //*clsPtr |> AssembleReferenceTokenStream
    return cls


def emit_ufunction(
    var cls: UClass?;
    var name: FName;
    createPropertiesCb: lambda<(owner: UObject?): array<FProperty?>>;
    nativeFuncPtr: lambda<(var context: UObject?; var stack: FFrame; param: void?): void>
)
    //TODO: check if function with name already exist in this class or it's superclasses

    var fn = new_uobject(cls, name, funcObjFlags, type<UFunction>)
    fn.FunctionFlags = funcFlags

    var props <- invoke(createPropertiesCb, fn)
    props |> reverse
    for prop in props
        *fn |> AddCppProperty(prop)

    *cls |> AddFunctionToFunctionMap(fn, name)
    //TODO: create new field in class
    //  fn.Next = cls.Children
    //  cls.Children = fn

    fn |> SetNativeFunc <| nativeFuncPtr
    *fn |> StaticLink(true)
    return fn
