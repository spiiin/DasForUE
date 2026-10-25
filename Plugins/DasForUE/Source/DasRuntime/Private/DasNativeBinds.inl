// Part of DasRuntime.cpp: native binds (DasNativeBinds.h) as daScript modules. One generic call node converts the
// VM arguments to engine values, calls the module's thunk and converts the result back. Classes bound with
// Class<T>(Name) get their own script types (an object handle under another name) with generated helpers.
// Included in place; declaration order matters.
// FName and FText arguments of native binds, for both paths: the value of the token in the active context's table,
// or the error text of ResolveDSLName / ResolveDSLText (the caller reports it with the function's label).
const TCHAR* DasNativeNameArgument(FDasNameValue Value, das::Context* Context, FName& Out)
{
    Out = NAME_None;
    if (!Value.Token)
        return nullptr;
    const FName* Name =
        ActiveContext && ActiveContext == Context ? ActiveContext->DSLNames.Find(Value.Token) : nullptr;
    if (!Name)
        return TEXT("name belongs to another context");
    Out = *Name;
    return nullptr;
}
const TCHAR* DasNativeTextArgument(FDasTextValue Value, das::Context* Context, FText& Out)
{
    Out = FText::GetEmpty();
    if (!Value.Token)
        return nullptr;
    const FText* Text =
        ActiveContext && ActiveContext == Context ? ActiveContext->TextValues.Find(Value.Token) : nullptr;
    if (!Text)
        return TEXT("invalid or foreign text value");
    Out = *Text;
    return nullptr;
}
TArray<FDasNativeClassType*>& DasNativeClassTypes()
{
    static TArray<FDasNativeClassType*> Types;
    return Types;
}
FDasNativeClassType* FindDasNativeClassType(const UClass* Class)
{
    for (FDasNativeClassType* Type : DasNativeClassTypes())
        if (Type->Class == Class)
            return Type;
    return nullptr;
}
// Class types of the ancestors of Class, nearest first.
TArray<FDasNativeClassType*> DasNativeAncestorTypes(const UClass* Class)
{
    TArray<FDasNativeClassType*> Result;
    for (const UClass* Super = Class->GetSuperClass(); Super; Super = Super->GetSuperClass())
        if (FDasNativeClassType* Type = FindDasNativeClassType(Super))
            Result.Add(Type);
    return Result;
}

// The script type of a class: the same layout as UObjectHandle, a different name for the compiler.
struct FDasNativeClassAnnotation : das::ManagedStructureAnnotation<FDasObjectHandle, false>
{
    FDasNativeClassAnnotation(const das::string& Name, das::ModuleLibrary& Library)
        : ManagedStructureAnnotation(Name, Library, "FDasObjectHandle")
    {
    }
    virtual bool isLocal() const override { return true; }
    virtual bool canCopy() const override { return true; }
    virtual bool canMove() const override { return true; }
    virtual bool hasNonTrivialCtor() const override { return false; }
    virtual bool isPod() const override { return true; }
    virtual bool canBePlacedInContainer() const override { return true; }
};

das::TypeDeclPtr DasNativeTypeDecl(const FDasNativeParam& Param, const FDasNativeResolvedParam& Resolved,
                                   const das::ModuleLibrary& Lib, bool bArgument)
{
    // A reference parameter: `var x : T&`, the caller's variable.
    if (bArgument && Param.bRef)
    {
        FDasNativeParam Value = Param;
        Value.bRef = false;
        das::TypeDeclPtr Type = DasNativeTypeDecl(Value, Resolved, Lib, false);
        Type->ref = true;
        Type->constant = false;
        return Type;
    }
    switch (Param.Type)
    {
    case EDasNativeType::Void:
        return das::makeType<void>(Lib);
    case EDasNativeType::Bool:
        return das::makeType<bool>(Lib);
    case EDasNativeType::Int32:
        return das::makeType<int32_t>(Lib);
    case EDasNativeType::Int64:
        return das::makeType<int64_t>(Lib);
    case EDasNativeType::Float:
        return das::makeType<float>(Lib);
    case EDasNativeType::Double:
        return das::makeType<double>(Lib);
    case EDasNativeType::Byte:
        return das::makeType<uint8_t>(Lib);
    case EDasNativeType::String:
        return bArgument ? das::makeType<const char*>(Lib) : das::makeType<char*>(Lib);
    case EDasNativeType::Name:
        return das::makeType<FDasNameValue>(Lib);
    case EDasNativeType::Text:
        return das::makeType<FDasTextValue>(Lib);
    case EDasNativeType::DateTime:
        return das::makeType<FDasDateTimeValue>(Lib);
    case EDasNativeType::Timespan:
        return das::makeType<FDasTimespanValue>(Lib);
    case EDasNativeType::Class:
        return das::makeType<FDasClassHandle>(Lib);
    case EDasNativeType::ObjectArray:
    {
        // array<T>: the element is the class's script type (or UObjectHandle), held by value.
        FDasNativeParam Element = Param;
        Element.Type = EDasNativeType::Object;
        das::TypeDeclPtr Array = new das::TypeDecl(das::Type::tArray, das::LineInfo());
        Array->firstType = DasNativeTypeDecl(Element, Resolved, Lib, false);
        return Array;
    }
    case EDasNativeType::Object:
    {
        if (!Resolved.Script)
            return bArgument ? das::makeType<const FDasObjectHandle&>(Lib) : das::makeType<FDasObjectHandle>(Lib);
        das::TypeDeclPtr Type = new das::TypeDecl(das::Type::tHandle, das::LineInfo());
        Type->annotation = Resolved.Script->Annotation;
        Type->ref = bArgument;
        Type->constant = bArgument;
        return Type;
    }
#define DAS_NATIVE_MATH(Kind, CppType)                                                                                 \
    case EDasNativeType::Kind:                                                                                         \
        return bArgument ? das::makeType<const CppType&>(Lib) : das::makeType<CppType>(Lib);
        DAS_NATIVE_MATH(Vector, FVector)
        DAS_NATIVE_MATH(Vector2D, FVector2D)
        DAS_NATIVE_MATH(Rotator, FRotator)
        DAS_NATIVE_MATH(Quat, FQuat)
        DAS_NATIVE_MATH(Transform, FTransform)
        DAS_NATIVE_MATH(LinearColor, FLinearColor)
        DAS_NATIVE_MATH(Color, FColor)
#undef DAS_NATIVE_MATH
    }
    return das::makeType<void>(Lib);
}
// The script name of a parameter or result type (manifests).
FString DasNativeScriptTypeName(const FDasNativeParam& Param, const FDasNativeResolvedParam& Resolved)
{
    if (Param.bRef)
    {
        FDasNativeParam Value = Param;
        Value.bRef = false;
        return DasNativeScriptTypeName(Value, Resolved) + TEXT("&");
    }
    switch (Param.Type)
    {
    case EDasNativeType::Void:
        return TEXT("void");
    case EDasNativeType::Bool:
        return TEXT("bool");
    case EDasNativeType::Int32:
        return TEXT("int");
    case EDasNativeType::Int64:
        return TEXT("int64");
    case EDasNativeType::Float:
        return TEXT("float");
    case EDasNativeType::Double:
        return TEXT("double");
    case EDasNativeType::Byte:
        return TEXT("uint8");
    case EDasNativeType::String:
        return TEXT("string");
    case EDasNativeType::Name:
        return TEXT("UENameValue");
    case EDasNativeType::Text:
        return TEXT("UETextValue");
    case EDasNativeType::DateTime:
        return TEXT("UEDateTime");
    case EDasNativeType::Timespan:
        return TEXT("UETimespan");
    case EDasNativeType::Object:
        return Resolved.Script ? Resolved.Script->Name : FString(TEXT("UObjectHandle"));
    case EDasNativeType::Class:
        return TEXT("UEClassHandle");
    case EDasNativeType::ObjectArray:
        return TEXT("array<") + (Resolved.Script ? Resolved.Script->Name : FString(TEXT("UObjectHandle"))) +
               TEXT(">");
    default:
    {
        // Engine values: the name of the type annotation ue_foundation registered (math handle names live in
        // their adapters, Scripts/TestTypeAdapterGate.py).
        das::ModuleLibrary Lib;
        Lib.addBuiltInModule();
        Lib.addModule(das::Module::require("ue_foundation"));
        const das::TypeDeclPtr Type = DasNativeTypeDecl(Param, Resolved, Lib, false);
        return Type->annotation ? FString(UTF8_TO_TCHAR(Type->annotation->name.c_str())) : FString();
    }
    }
}
// AOT ABI of a parameter or result (DasUEAot.h): engine values by const reference, results by value, objects as
// their handles, a reference parameter (bRef) as a reference. nullptr when a type has none (FName, FText): such
// functions stay in the interpreter.
const char* DasNativeAbiName(EDasNativeType Type, bool bArgument, bool bRef = false)
{
    if (bRef)
        switch (Type)
        {
        case EDasNativeType::Bool:
            return "bool &";
        case EDasNativeType::Int32:
            return "int32_t &";
        case EDasNativeType::Int64:
            return "int64_t &";
        case EDasNativeType::Float:
            return "float &";
        case EDasNativeType::Double:
            return "double &";
        case EDasNativeType::Byte:
            return "uint8_t &";
        case EDasNativeType::String:
            return "char * &";
        case EDasNativeType::DateTime:
            return "FDasDateTimeValue &";
        case EDasNativeType::Timespan:
            return "FDasTimespanValue &";
        case EDasNativeType::Vector:
            return "FVector &";
        case EDasNativeType::Vector2D:
            return "FVector2D &";
        case EDasNativeType::Rotator:
            return "FRotator &";
        case EDasNativeType::Quat:
            return "FQuat &";
        case EDasNativeType::Transform:
            return "FTransform &";
        case EDasNativeType::LinearColor:
            return "FLinearColor &";
        case EDasNativeType::Color:
            return "FColor &";
        default:
            return nullptr;
        }
    switch (Type)
    {
    case EDasNativeType::Void:
        return bArgument ? nullptr : "void";
    case EDasNativeType::Bool:
        return "bool";
    case EDasNativeType::Int32:
        return "int32_t";
    case EDasNativeType::Int64:
        return "int64_t";
    case EDasNativeType::Float:
        return "float";
    case EDasNativeType::Double:
        return "double";
    case EDasNativeType::Byte:
        return "uint8_t";
    case EDasNativeType::String:
        return bArgument ? "const char *" : "char *";
    case EDasNativeType::Name:
        return "FDasNameValue";
    case EDasNativeType::Text:
        return "FDasTextValue";
    case EDasNativeType::DateTime:
        return "FDasDateTimeValue";
    case EDasNativeType::Timespan:
        return "FDasTimespanValue";
    case EDasNativeType::Object:
        return bArgument ? "const FDasObjectHandle &" : "FDasObjectHandle";
    case EDasNativeType::Class:
        return bArgument ? "FDasClassHandle" : nullptr; // class results are not bound
#define DAS_NATIVE_ABI(Kind, CppType)                                                                                  \
    case EDasNativeType::Kind:                                                                                         \
        return bArgument ? "const " CppType " &" : CppType;
        DAS_NATIVE_ABI(Vector, "FVector")
        DAS_NATIVE_ABI(Vector2D, "FVector2D")
        DAS_NATIVE_ABI(Rotator, "FRotator")
        DAS_NATIVE_ABI(Quat, "FQuat")
        DAS_NATIVE_ABI(Transform, "FTransform")
        DAS_NATIVE_ABI(LinearColor, "FLinearColor")
        DAS_NATIVE_ABI(Color, "FColor")
#undef DAS_NATIVE_ABI
    default:
        return nullptr;
    }
}
// The C++ types of a function's AOT entry ("R,A1,...,das::Context *"), or empty when it has none.
FString DasNativeAbiTypes(const FDasNativeFunctionDesc& F)
{
    const char* Result = DasNativeAbiName(F.Result.Type, false);
    if (!F.AotEntry || !Result)
        return FString();
    FString Types = UTF8_TO_TCHAR(Result);
    for (const FDasNativeParam& P : F.Arguments)
    {
        const char* Argument = DasNativeAbiName(P.Type, true, P.bRef);
        if (!Argument)
            return FString();
        Types += FString(TEXT(",")) + UTF8_TO_TCHAR(Argument);
    }
    return Types + TEXT(",das::Context *");
}
bool DasNativeIsMath(EDasNativeType Type)
{
    return Type >= EDasNativeType::Vector;
}
// Results returned through the VM's result slot (copy-on-return).
bool DasNativeIsCmres(EDasNativeType Type)
{
    return DasNativeIsMath(Type) || Type == EDasNativeType::Object || Type == EDasNativeType::ObjectArray;
}

// Engine values of one call: constructed in place, destroyed when the call ends (also on a script error).
struct FDasNativeValues
{
    alignas(16) uint8 Storage[FDasNativeModule::MaxArguments + 1][sizeof(FTransform)];
    EDasNativeType Types[FDasNativeModule::MaxArguments + 1];
    int32 Count = 0;
    void* Construct(EDasNativeType Type)
    {
        void* Slot = Storage[Count];
        switch (Type)
        {
        case EDasNativeType::String:
            new (Slot) FString();
            break;
        case EDasNativeType::Name:
            new (Slot) FName();
            break;
        case EDasNativeType::Text:
            new (Slot) FText();
            break;
        case EDasNativeType::ObjectArray:
            new (Slot) TArray<UObject*>();
            break;
        default:
            FMemory::Memzero(Slot, sizeof(Storage[0]));
            break;
        }
        Types[Count++] = Type;
        return Slot;
    }
    ~FDasNativeValues()
    {
        for (int32 I = 0; I < Count; ++I)
            if (Types[I] == EDasNativeType::String)
                static_cast<FString*>(static_cast<void*>(Storage[I]))->~FString();
            else if (Types[I] == EDasNativeType::Text)
                static_cast<FText*>(static_cast<void*>(Storage[I]))->~FText();
            else if (Types[I] == EDasNativeType::ObjectArray)
                static_cast<TArray<UObject*>*>(static_cast<void*>(Storage[I]))->~TArray<UObject*>();
    }
};
static_assert(sizeof(TArray<UObject*>) <= sizeof(FTransform) && sizeof(FString) <= sizeof(FTransform) &&
              sizeof(FText) <= sizeof(FTransform) &&
              sizeof(FLinearColor) <= sizeof(FTransform) && sizeof(FQuat) <= sizeof(FTransform));

// A reference argument (FDasNativeParam::bRef): the VM passes the address of the script's variable. The call works
// on a copy in its slot, written back only when the call succeeds.
int32 DasNativeValueSize(EDasNativeType Type)
{
    switch (Type)
    {
    case EDasNativeType::Bool:
        return sizeof(bool);
    case EDasNativeType::Int32:
    case EDasNativeType::Float:
        return 4;
    case EDasNativeType::Int64:
    case EDasNativeType::Double:
        return 8;
    case EDasNativeType::Byte:
        return 1;
    case EDasNativeType::Vector:
        return sizeof(FVector);
    case EDasNativeType::Vector2D:
        return sizeof(FVector2D);
    case EDasNativeType::Rotator:
        return sizeof(FRotator);
    case EDasNativeType::Quat:
        return sizeof(FQuat);
    case EDasNativeType::Transform:
        return sizeof(FTransform);
    case EDasNativeType::LinearColor:
        return sizeof(FLinearColor);
    case EDasNativeType::Color:
        return sizeof(FColor);
    default:
        return 0;
    }
}
void DasNativeReadRef(EDasNativeType Type, const void* Variable, void* Slot)
{
    switch (Type)
    {
    case EDasNativeType::String:
    {
        const char* Text = *static_cast<char* const*>(Variable);
        *static_cast<FString*>(Slot) = Text ? FString(UTF8_TO_TCHAR(Text)) : FString();
        break;
    }
    case EDasNativeType::DateTime:
        *static_cast<FDateTime*>(Slot) = FDateTime(static_cast<const FDasDateTimeValue*>(Variable)->Ticks);
        break;
    case EDasNativeType::Timespan:
        *static_cast<FTimespan*>(Slot) = FTimespan(static_cast<const FDasTimespanValue*>(Variable)->Ticks);
        break;
    default:
        FMemory::Memcpy(Slot, Variable, DasNativeValueSize(Type));
        break;
    }
}
void DasNativeWriteRef(EDasNativeType Type, const void* Slot, void* Variable, das::Context& Context)
{
    switch (Type)
    {
    case EDasNativeType::String:
        *static_cast<char**>(Variable) = CopyDSLString(*static_cast<const FString*>(Slot), &Context);
        break;
    case EDasNativeType::DateTime:
        static_cast<FDasDateTimeValue*>(Variable)->Ticks = static_cast<const FDateTime*>(Slot)->GetTicks();
        break;
    case EDasNativeType::Timespan:
        static_cast<FDasTimespanValue*>(Variable)->Ticks = static_cast<const FTimespan*>(Slot)->GetTicks();
        break;
    default:
        FMemory::Memcpy(Variable, Slot, DasNativeValueSize(Type));
        break;
    }
}

struct SimNode_DasNative : das::SimNode_ExtFuncCallBase
{
    const FDasNativeModuleDesc* Module = nullptr; // whose code runs: an unloaded module reports an error
    const FDasNativeFunctionDesc* Function = nullptr;
    const char* Label = nullptr; // "Type.method" or the function name, for errors
    SimNode_DasNative(const das::LineInfo& At, const char* Name, const FDasNativeModuleDesc* M,
                      const FDasNativeFunctionDesc* F, const char* L)
        : SimNode_ExtFuncCallBase(At, Name), Module(M), Function(F), Label(L)
    {
    }
    // Objects come through the context's handle table: null, expired and foreign handles read as null.
    UObject* ResolveArgument(das::Context& Context, vec4f Raw, int32 I)
    {
        const auto* Handle = static_cast<const FDasObjectHandle*>(das::cast<void*>::to(Raw));
        UObject* Object = Handle->Token ? TryObject(*Handle, &Context) : nullptr;
        if (Function->Helper != EDasNativeHelper::Call)
            return Object; // helpers take any object; a typed handle already has its class
        const int32 Number = Function->IsMethod() ? I : I + 1;
        UClass* Class = Function->Resolved[I].Class;
        if (Object && !Object->IsA(Class))
            Context.throw_error_at(debugInfo, "%s: argument %d must be %s, got %s", Label, Number,
                                   TCHAR_TO_UTF8(*Class->GetName()), TCHAR_TO_UTF8(*Object->GetClass()->GetName()));
        if (!Object && I == 0 && Function->IsMethod())
            Context.throw_error_at(debugInfo, "%s: receiver is null or expired", Label);
        if (!Object && EnumHasAnyFlags(Function->Flags, EDasBind::NonNullArgs))
            Context.throw_error_at(debugInfo, "%s: argument %d is null or expired", Label, Number);
        return Object;
    }
    // A class argument: null passes as nullptr, an expired handle and a class that is not the base fail the call.
    UClass* ResolveClassArgument(das::Context& Context, vec4f Raw, int32 I)
    {
        const FDasClassHandle Handle = das::cast<FDasClassHandle>::to(Raw);
        if (!Handle.Token)
            return nullptr;
        UClass* Class = Cast<UClass>(TryObject(FDasObjectHandle{Handle.Token}, &Context));
        if (!Class)
            Context.throw_error_at(debugInfo, "%s: object handle is null, expired, or belongs to another context",
                                   Label);
        UClass* Base = Function->Arguments[I].Class();
        if (!Class->IsChildOf(Base))
            Context.throw_error_at(debugInfo, "%s: argument %d must be a subclass of %s, got %s", Label,
                                   Function->IsMethod() ? I : I + 1, TCHAR_TO_UTF8(*Base->GetName()),
                                   TCHAR_TO_UTF8(*Class->GetName()));
        return Class;
    }
    // An array result into the caller's array (cmres): handles of the element class, null for other objects.
    vec4f ReturnObjects(das::Context& Context, const TArray<UObject*>& Values)
    {
        UClass* Class = Function->Resolved.Last().Class;
        auto* Array = reinterpret_cast<das::Array*>(cmresEval->evalPtr(Context));
        // The result slot is not constructed: the callee makes the array (an empty, unlocked one first).
        FMemory::Memzero(Array, sizeof(das::Array));
        das::builtin_array_resize(*Array, Values.Num(), sizeof(FDasObjectHandle), &Context, nullptr);
        auto* Handles = reinterpret_cast<FDasObjectHandle*>(Array->data);
        for (int32 I = 0; I < Values.Num(); ++I)
            Handles[I] = Values[I] && Values[I]->IsA(Class) ? MakeObjectHandle(Values[I], Class, &Context)
                                                            : FDasObjectHandle();
        return das::cast<void*>::from(Array);
    }
    vec4f ReturnObject(das::Context& Context, UObject* Object)
    {
        UClass* Class = Function->Resolved.Last().Class;
        auto* Out = reinterpret_cast<FDasObjectHandle*>(cmresEval->evalPtr(Context));
        *Out = Object && Object->IsA(Class) ? MakeObjectHandle(Object, Class, &Context) : FDasObjectHandle();
        return das::cast<void*>::from(Out);
    }
    DAS_EVAL_ABI virtual vec4f eval(das::Context& Context) override
    {
        GuardUEExtern();
        if (!Module->bLoaded)
            Context.throw_error_at(debugInfo, "%s", TCHAR_TO_UTF8(*Module->Error));
        vec4f Raw[FDasNativeModule::MaxArguments + 1]; // + the context argument of an AOT-eligible function
        evalArgs(Context, Raw);
        return Run(Context, Raw);
    }
    // The call with evaluated arguments.
    vec4f Run(das::Context& Context, vec4f* Raw)
    {
        FDasNativeValues Values;
        void* Args[FDasNativeModule::MaxArguments];
        UObject* Objects[FDasNativeModule::MaxArguments] = {};
        for (int32 I = 0; I < Function->Arguments.Num(); ++I)
        {
            const EDasNativeType Type = Function->Arguments[I].Type;
            if (Function->Arguments[I].bRef)
            {
                Args[I] = Values.Construct(Type);
                DasNativeReadRef(Type, das::cast<void*>::to(Raw[I]), Args[I]);
                continue;
            }
            if (DasNativeIsMath(Type))
            {
                Args[I] = das::cast<void*>::to(Raw[I]); // the VM's own value, passed by const reference
                continue;
            }
            void* Slot = Values.Construct(Type);
            switch (Type)
            {
            case EDasNativeType::Bool:
                *static_cast<bool*>(Slot) = das::cast<bool>::to(Raw[I]);
                break;
            case EDasNativeType::Int32:
                *static_cast<int32*>(Slot) = das::cast<int32_t>::to(Raw[I]);
                break;
            case EDasNativeType::Int64:
                *static_cast<int64*>(Slot) = das::cast<int64_t>::to(Raw[I]);
                break;
            case EDasNativeType::DateTime:
                *static_cast<FDateTime*>(Slot) = FDateTime(das::cast<FDasDateTimeValue>::to(Raw[I]).Ticks);
                break;
            case EDasNativeType::Timespan:
                *static_cast<FTimespan*>(Slot) = FTimespan(das::cast<FDasTimespanValue>::to(Raw[I]).Ticks);
                break;
            case EDasNativeType::Float:
                *static_cast<float*>(Slot) = das::cast<float>::to(Raw[I]);
                break;
            case EDasNativeType::Double:
                *static_cast<double*>(Slot) = das::cast<double>::to(Raw[I]);
                break;
            case EDasNativeType::Byte:
                *static_cast<uint8*>(Slot) = das::cast<uint8_t>::to(Raw[I]);
                break;
            case EDasNativeType::String:
            {
                const char* Text = das::cast<const char*>::to(Raw[I]);
                *static_cast<FString*>(Slot) = Text ? FString(UTF8_TO_TCHAR(Text)) : FString();
                break;
            }
            case EDasNativeType::Name:
                if (const TCHAR* Error = DasNativeNameArgument(das::cast<FDasNameValue>::to(Raw[I]), &Context,
                                                               *static_cast<FName*>(Slot)))
                    Context.throw_error_at(debugInfo, "%s: %s", Label, TCHAR_TO_UTF8(Error));
                break;
            case EDasNativeType::Text:
                if (const TCHAR* Error = DasNativeTextArgument(das::cast<FDasTextValue>::to(Raw[I]), &Context,
                                                               *static_cast<FText*>(Slot)))
                    Context.throw_error_at(debugInfo, "%s: %s", Label, TCHAR_TO_UTF8(Error));
                break;
            case EDasNativeType::Object:
                Objects[I] = ResolveArgument(Context, Raw[I], I);
                *static_cast<UObject**>(Slot) = Objects[I];
                break;
            case EDasNativeType::Class:
                *static_cast<UClass**>(Slot) = ResolveClassArgument(Context, Raw[I], I);
                break;
            default:
                break;
            }
            Args[I] = Slot;
        }
        switch (Function->Helper)
        {
        case EDasNativeHelper::Valid:
            return das::cast<bool>::from(Objects[0] != nullptr);
        case EDasNativeHelper::Equal:
            return das::cast<bool>::from(Objects[0] == Objects[1]);
        case EDasNativeHelper::NotEqual:
            return das::cast<bool>::from(Objects[0] != Objects[1]);
        case EDasNativeHelper::Handle:
        case EDasNativeHelper::Cast:
            return ReturnObject(Context, Objects[0]);
        case EDasNativeHelper::Call:
            break;
        }
        const EDasNativeType Result = Function->Result.Type;
        // Math results are written straight into the VM's result slot (copy-on-return, like other value externs).
        void* Out = nullptr;
        if (DasNativeIsMath(Result))
            Out = cmresEval->evalPtr(Context);
        else if (Result != EDasNativeType::Void)
            Out = Values.Construct(Result);
        // DasNativeError during the call fails it after the function returns.
        FString Error;
        FString* const Outer = DasNativeErrorSlot();
        DasNativeErrorSlot() = &Error;
        Function->Thunk(Args, Out);
        DasNativeErrorSlot() = Outer;
        if (!Error.IsEmpty())
            Context.throw_error_at(debugInfo, "%s: %s", Label, TCHAR_TO_UTF8(*Error));
        for (int32 I = 0; I < Function->Arguments.Num(); ++I)
            if (Function->Arguments[I].bRef)
                DasNativeWriteRef(Function->Arguments[I].Type, Args[I], das::cast<void*>::to(Raw[I]), Context);
        switch (Result)
        {
        case EDasNativeType::Void:
            return v_zero();
        case EDasNativeType::Bool:
            return das::cast<bool>::from(*static_cast<bool*>(Out));
        case EDasNativeType::Int32:
            return das::cast<int32_t>::from(*static_cast<int32*>(Out));
        case EDasNativeType::Int64:
            return das::cast<int64_t>::from(*static_cast<int64*>(Out));
        case EDasNativeType::DateTime:
            return das::cast<FDasDateTimeValue>::from({static_cast<FDateTime*>(Out)->GetTicks()});
        case EDasNativeType::Timespan:
            return das::cast<FDasTimespanValue>::from({static_cast<FTimespan*>(Out)->GetTicks()});
        case EDasNativeType::Float:
            return das::cast<float>::from(*static_cast<float*>(Out));
        case EDasNativeType::Double:
            return das::cast<double>::from(*static_cast<double*>(Out));
        case EDasNativeType::Byte:
            return das::cast<uint8_t>::from(*static_cast<uint8*>(Out));
        case EDasNativeType::String:
            return das::cast<char*>::from(CopyDSLString(*static_cast<FString*>(Out), &Context));
        case EDasNativeType::Name:
            return das::cast<FDasNameValue>::from(MakeDSLName(*static_cast<FName*>(Out), &Context));
        case EDasNativeType::Text:
            return das::cast<FDasTextValue>::from(MakeDSLText(*static_cast<FText*>(Out), &Context));
        case EDasNativeType::Object:
            return ReturnObject(Context, *static_cast<UObject**>(Out));
        case EDasNativeType::ObjectArray:
            return ReturnObjects(Context, *static_cast<TArray<UObject*>*>(Out));
        default:
            return das::cast<void*>::from(Out);
        }
    }
};

class FDasNativeFunction final : public das::ExternalFnBase
{
public:
    const FDasNativeModuleDesc* Module;
    const FDasNativeFunctionDesc* Function;
    das::string Label;
    // bAot: the function has an AOT entry; generated code passes the context as the last argument.
    FDasNativeFunction(const FDasNativeModuleDesc& Origin, const FDasNativeFunctionDesc& F,
                       const das::ModuleLibrary& Lib, const das::string& Name, bool bAot)
        : ExternalFnBase(Name.c_str(), nullptr), Module(&Origin), Function(&F), Label(Name)
    {
        if (F.Helper == EDasNativeHelper::Call && F.IsMethod() && F.Resolved[0].Script)
            Label = das::string(TCHAR_TO_UTF8(*F.Resolved[0].Script->Name)) + "." + Name;
        das::vector<das::TypeDeclPtr> Types{DasNativeTypeDecl(F.Result, F.Resolved.Last(), Lib, false)};
        for (int32 I = 0; I < F.Arguments.Num(); ++I)
            Types.push_back(DasNativeTypeDecl(F.Arguments[I], F.Resolved[I], Lib, true));
        if (bAot)
            Types.push_back(das::makeType<das::Context*>(Lib));
        constructExternal(Types);
        // AOT calls go through the guarded bridge once the entry is registered (FDasNativeDasModule::Add).
        noAot = true;
    }
    virtual das::SimNode* makeSimNode(das::Context& Context, const das::vector<das::ExpressionPtr>&) override
    {
        const char* FnName = Context.code->allocateName(this->name);
        return Context.code->makeNode<TDasDiagnosticNode<SimNode_DasNative>>(at, FnName, Module, Function,
                                                                             Label.c_str());
    }
};

// A native gameplay method (`ref.K2_GetComponentLocation()`, DasAuthoredRefs.inl) answered by the native bind that
// reflects it (Reflects). The script module gets a wrapper with the method's name and parameters (DSLEmitRoutedMethod)
// that calls the bind's route: a builtin of ue_foundation, made when the bind's module is built, that takes the
// receiver's handle and class path before the bind's own arguments. It runs the receiver checks of the reflected
// call (BeginNativeGameplayCall: a live receiver in the owner's world, no calls in bind_events / EndPlay, its
// class), then the bind, without a call frame. Object parameters and results cross the route as UObjectHandle (the
// wrapper passes `arg.handle` and turns the result into the method's reference type); an object argument that is
// expired fails with the error of the reflected call (ResolveObject), null passes. The route is not in the script
// module: an executable program drops the builtins of its own module (removeUnusedSymbols marks only script
// functions used), and its calls would keep dangling pointers.
struct SimNode_DasNativeRoute : SimNode_DasNative
{
    using SimNode_DasNative::SimNode_DasNative;
    DAS_EVAL_ABI virtual vec4f eval(das::Context& Context) override
    {
        GuardUEExtern();
        if (!Module->bLoaded)
            Context.throw_error_at(debugInfo, "%s", TCHAR_TO_UTF8(*Module->Error));
        vec4f Raw[FDasNativeModule::MaxArguments + 2]; // the receiver, its class path, the other arguments
        evalArgs(Context, Raw);
        const auto* Handle = static_cast<const FDasObjectHandle*>(das::cast<void*>::to(Raw[0]));
        NativeGameplayObject(*Handle, das::cast<const char*>::to(Raw[1]), true, &Context);
        for (int32 I = 1; I < Function->Arguments.Num(); ++I)
        {
            Raw[I] = Raw[I + 1];
            if (Function->Arguments[I].Type == EDasNativeType::Object)
                if (const auto* Argument = static_cast<const FDasObjectHandle*>(das::cast<void*>::to(Raw[I]));
                    Argument->Token && !TryObject(*Argument, &Context))
                    Context.throw_error("object handle is null, expired, or belongs to another context");
        }
        ++DasNativeRoutedCallCount();
        return Run(Context, Raw);
    }
};
// A parameter or result type of a route: objects are UObjectHandle (the gameplay method's reference type carries
// the class), everything else is the bind's type.
das::TypeDeclPtr DasNativeRouteTypeDecl(const FDasNativeParam& Param, const FDasNativeResolvedParam& Resolved,
                                        const das::ModuleLibrary& Lib, bool bArgument)
{
    return DasNativeTypeDecl(Param, Param.Type == EDasNativeType::Object ? FDasNativeResolvedParam() : Resolved, Lib,
                             bArgument);
}
class FDasNativeRouteFunction final : public das::ExternalFnBase
{
public:
    const FDasNativeModuleDesc* Module;
    const FDasNativeFunctionDesc* Function;
    das::string Label;
    FDasNativeRouteFunction(const FDasNativeModuleDesc& Origin, const FDasNativeFunctionDesc& F,
                            const das::ModuleLibrary& Lib)
        : ExternalFnBase(TCHAR_TO_UTF8(*F.RouteName), nullptr), Module(&Origin), Function(&F),
          Label(TCHAR_TO_UTF8(*F.ReflectedName))
    {
        das::vector<das::TypeDeclPtr> Types{DasNativeRouteTypeDecl(F.Result, F.Resolved.Last(), Lib, false),
                                            das::makeType<const FDasObjectHandle&>(Lib),
                                            das::makeType<const char*>(Lib)};
        for (int32 I = 1; I < F.Arguments.Num(); ++I)
            Types.push_back(DasNativeRouteTypeDecl(F.Arguments[I], F.Resolved[I], Lib, true));
        constructExternal(Types);
        // Object receivers have no AOT ABI: callers stay in the interpreter (FDasClosedCallsLint).
        noAot = true;
    }
    virtual das::SimNode* makeSimNode(das::Context& Context, const das::vector<das::ExpressionPtr>&) override
    {
        const char* FnName = Context.code->allocateName(this->name);
        return Context.code->makeNode<TDasDiagnosticNode<SimNode_DasNativeRoute>>(at, FnName, Module, Function,
                                                                                 Label.c_str());
    }
};

// Why a gameplay call of UFUNCTION Function on a receiver of class Receiver keeps the reflected call although
// a bind reflects it, or empty. Policies that the reflected call checks per call stay there.
FString DasNativeRoutePolicy(const UFunction* Function, const UClass* Receiver)
{
    const auto* Row = DasNativeGameplayMethod(Receiver->GetPathName(), Function->GetName());
    if (!Row)
        return TEXT("not a generated native gameplay method");
    if (Row->ConstructionUnsafe)
        return TEXT("unsafe in construction scripts");
    if (Function->HasAnyFunctionFlags(FUNC_BlueprintAuthorityOnly))
        return TEXT("requires authority");
    return FString();
}
// Why the script types of a gameplay method (Types: the result, then the parameters) are not the types of the
// bind that reflects it, or empty. An object is a native reference type there: ObjectPath gives its class path (empty
// for any other type), which must be the bind's class.
FString DasNativeRouteTypes(const FDasNativeRoute& Route, const das::ModuleLibrary& Lib,
                            const das::vector<das::TypeDeclPtr>& Types,
                            TFunctionRef<FString(const das::TypeDeclPtr&)> ObjectPath)
{
    const FDasNativeFunctionDesc& F = *Route.Function;
    if (F.RouteName.IsEmpty())
        return TEXT("the bind has no route");
    if (Types.size() != size_t(F.Arguments.Num()))
        return TEXT("the number of parameters differs");
    auto Same = [&](const das::TypeDeclPtr& Script, const FDasNativeParam& P, const FDasNativeResolvedParam& R,
                    bool bArgument)
    {
        if (P.Type == EDasNativeType::Object)
            return R.Class && ObjectPath(Script) == R.Class->GetPathName();
        const auto Bind = DasNativeTypeDecl(P, R, Lib, bArgument);
        return Script->isSameType(*Bind, das::RefMatters::no, das::ConstMatters::no, das::TemporaryMatters::no);
    };
    if (!Same(Types[0], F.Result, F.Resolved.Last(), false))
        return TEXT("the result type differs");
    for (int32 I = 1; I < F.Arguments.Num(); ++I)
        if (!Same(Types[size_t(I)], F.Arguments[I], F.Resolved[I], true))
            return FString::Printf(TEXT("the type of parameter %d differs"), I);
    return FString();
}

// Object parameters of a module's own function: the class each object must have and its script type.
void ResolveDasNativeFunction(FDasNativeFunctionDesc& F)
{
    F.Resolved.SetNum(F.Arguments.Num() + 1);
    auto Resolve = [](const FDasNativeParam& P, FDasNativeResolvedParam& R)
    {
        if (P.Type != EDasNativeType::Object && P.Type != EDasNativeType::ObjectArray)
            return;
        R.Class = P.Class();
        R.Script = FindDasNativeClassType(R.Class);
    };
    for (int32 I = 0; I < F.Arguments.Num(); ++I)
        Resolve(F.Arguments[I], F.Resolved[I]);
    Resolve(F.Result, F.Resolved.Last());
}

// The daScript modules of native binds (the closed-calls lint below needs to recognize them).
TSet<const das::Module*>& DasNativeDasModules()
{
    static TSet<const das::Module*> Modules;
    return Modules;
}

// AOT: generated code calls a builtin by its C++ name, and a closed plugin builtin (noAot: no ABI in the bridge)
// has none. A script function that calls one stays in the interpreter; AOT code reaches it through the
// interpreter (das_invoke_function), the rest of the file is still compiled. Registered in ue_foundation.
struct FDasClosedCallsLint : das::PassMacro
{
    FDasClosedCallsLint() : PassMacro("ue_closed_builtin_calls") {}
    virtual bool apply(das::Program*, das::Module* M) override
    {
        struct FFind : das::Visitor
        {
            bool bClosed = false;
            void Check(const das::Function* F)
            {
                if (F && F->builtIn && F->noAot && F->module &&
                    (F->module->name == "ue_foundation" || DasNativeDasModules().Contains(F->module)))
                    bClosed = true;
            }
            virtual void preVisit(das::ExprCall* E) override { Check(E->func); }
            virtual void preVisit(das::ExprOp1* E) override { Check(E->func); }
            virtual void preVisit(das::ExprOp2* E) override { Check(E->func); }
        };
        M->functions.foreach (
            [&](das::Function* F)
            {
                if (!F->body || F->noAot)
                    return;
                FFind Find;
                F->body->visit(Find);
                if (Find.bClosed)
                    F->noAot = true;
            });
        return false;
    }
};
das::PassMacro* MakeDasClosedCallsLint()
{
    return new FDasClosedCallsLint();
}

class FDasNativeDasModule final : public das::Module
{
public:
    // Generated AOT code calls native binds through the guarded bridge; functions without an ABI stay closed.
    virtual das::ModuleAotType aotRequire(das::TextWriter& Writer) const override
    {
        Writer << "#include \"DasUEAot.h\"\n";
        return das::ModuleAotType::cpp;
    }
    explicit FDasNativeDasModule(FDasNativeModuleDesc& InDesc)
        : das::Module(TCHAR_TO_UTF8(*InDesc.Name)), Desc(InDesc), Library(this)
    {
        DasNativeDasModules().Add(this);
        Library.addBuiltInModule();
        auto* Foundation = das::Module::require("ue_foundation");
        Library.addModule(Foundation);
        // Scripts that require a native module also see the engine value types it uses.
        addDependency(Foundation, true);
        TArray<FDasNativeClassType*> Owned;
        for (FDasNativeClassType* Type : DasNativeClassTypes())
            if (Type->Owner == &Desc)
            {
                auto* Annotation = new FDasNativeClassAnnotation(TCHAR_TO_UTF8(*Type->Name), Library);
                addAnnotation(Annotation);
                Type->Annotation = Annotation;
                Owned.Add(Type);
            }
        for (FDasNativeFunctionDesc& F : Desc.Functions)
        {
            ResolveDasNativeFunction(F);
            Add(Desc, F);
            if (!F.ReflectedPath.IsEmpty())
                AddRoute(*Foundation, F);
        }
        for (FDasNativeClassType* Type : Owned)
            AddHelpers(*Type);
        // Methods of an ancestor class on descendant types: whichever of the two modules is built later adds them.
        for (const FDasNativeFunctionDesc& F : Desc.Functions)
            if (F.IsMethod())
                for (FDasNativeClassType* Type : DasNativeClassTypes())
                    if (Type != F.Resolved[0].Script && (Type->Owner == &Desc || Type->Owner->bBuilt) &&
                        Type->Class->IsChildOf(F.Resolved[0].Class))
                        Inherit(Desc, F, *Type);
        for (FDasNativeClassType* Type : Owned)
            for (FDasNativeModuleDesc* Other : DasNativeModuleDescs())
                if (Other != &Desc && Other->bBuilt)
                    for (const FDasNativeFunctionDesc& F : Other->Functions)
                        if (F.IsMethod() && Type != F.Resolved[0].Script && Type->Class->IsChildOf(F.Resolved[0].Class))
                            Inherit(*Other, F, *Type);
    }

private:
    FDasNativeModuleDesc& Desc;
    das::ModuleLibrary Library;
    TSet<const FDasNativeModuleDesc*> TypeModules;
    TSet<FString> Signatures; // name and parameter types of the functions added so far

    // The module that owns a script type must be visible to this one and to the scripts that require this one.
    void UseTypes(const FDasNativeFunctionDesc& F)
    {
        for (const FDasNativeResolvedParam& R : F.Resolved)
            if (R.Script && R.Script->Owner != &Desc && !TypeModules.Contains(R.Script->Owner))
            {
                TypeModules.Add(R.Script->Owner);
                auto* Owner = das::Module::require(TCHAR_TO_UTF8(*R.Script->Owner->Name));
                Library.addModule(Owner);
                addDependency(Owner, true);
            }
    }
    static FString Signature(const FDasNativeFunctionDesc& F)
    {
        FString Key = F.Name;
        for (int32 I = 0; I < F.Arguments.Num(); ++I)
            Key += FString::Printf(TEXT("|%d:%s"), int32(F.Arguments[I].Type),
                                        F.Resolved[I].Script ? *F.Resolved[I].Script->Name : TEXT(""));
        return Key;
    }
    void Add(const FDasNativeModuleDesc& Origin, const FDasNativeFunctionDesc& F)
    {
        UseTypes(F);
        Signatures.Add(Signature(F));
        const FString AbiTypes = DasNativeAbiTypes(F);
        auto* Fn = new FDasNativeFunction(Origin, F, Library, das::string(TCHAR_TO_UTF8(*F.Name)), !AbiTypes.IsEmpty());
        if (!AbiTypes.IsEmpty())
            RegisterAot(Origin, F, *Fn, AbiTypes);
        das::SideEffects Effects = EnumHasAnyFlags(F.Flags, EDasBind::Pure)
                                       ? das::SideEffects::none
                                       : das::SideEffects::modifyExternal | das::SideEffects::accessExternal;
        // A reference parameter changes the caller's variable: without this the compiler would keep using the
        // value it had before the call (constant propagation).
        if (F.Arguments.ContainsByPredicate([](const FDasNativeParam& P) { return P.bRef; }))
            Effects = Effects | das::SideEffects::modifyArgument;
        das::addExternFunc(*this, Fn, DasNativeIsCmres(F.Result.Type), Effects);
    }
    // The route of a bind that reflects a UFUNCTION (SimNode_DasNativeRoute): a builtin of ue_foundation, which every
    // gameplay script sees, with a name of its own (a rebuilt module adds a new one).
    void AddRoute(das::Module& Foundation, FDasNativeFunctionDesc& F)
    {
        static int32 Routes = 0;
        F.RouteName = FString::Printf(TEXT("__ue_native_route_%d"), ++Routes);
        das::addExternFunc(Foundation, new FDasNativeRouteFunction(Desc, F, Library), DasNativeIsCmres(F.Result.Type),
                           das::SideEffects::modifyExternal | das::SideEffects::accessExternal);
    }
    // The AOT entry goes into the bridge registry under a key of module, name and C++ types; the function's
    // cppName is the bridge call. Resolve refuses an entry of an unloaded module.
    static void RegisterAot(const FDasNativeModuleDesc& Origin, const FDasNativeFunctionDesc& F, FDasNativeFunction& Fn,
                            const FString& Types)
    {
        // The label names the receiver's type: a method inherited by a descendant type reports its own name.
        const FString Name = FString::Printf(TEXT("native:%s::%s"), *Origin.Name, UTF8_TO_TCHAR(Fn.Label.c_str()));
        const uint64 Key = dasue::aot::Key(TCHAR_TO_UTF8(*Name), TCHAR_TO_UTF8(*Types));
        if (const auto* Existing = UEAotEntries.Find(Key);
            Existing && Existing->Function != F.AotEntry && (!Existing->Loaded || *Existing->Loaded))
        {
            UE_LOG(LogDasRuntime, Warning,
                   TEXT("Native bind %s: AOT key collision, it stays in the interpreter"), *Name);
            return;
        }
        AddUEAotEntry(Key, {F.AotEntry, Name + TEXT(":") + Types, UTF8_TO_TCHAR(Fn.Label.c_str()), &Origin.bLoaded});
        // Generated code calls an operator with its operands only: CallOp adds the context (the entry's last).
        const bool bOperator = !F.Name.IsEmpty() && !FChar::IsAlpha(F.Name[0]) && F.Name[0] != TEXT('_');
        const FString Operands = Types.LeftChop(FCString::Strlen(TEXT(",das::Context *")));
        Fn.cppName = TCHAR_TO_UTF8(*(bOperator ? FString::Printf(TEXT("dasue::aot::CallOp<%lluULL,%s>"), Key, *Operands)
                                               : FString::Printf(TEXT("dasue::aot::Call<%lluULL,%s>"), Key, *Types)));
        Fn.noAot = false;
    }
    // Helper functions: object parameters and results of a given script type (nullptr: UObjectHandle).
    FDasNativeFunctionDesc& Generate(const FString& Name, EDasNativeHelper Helper)
    {
        auto& F = *Desc.Generated.Add_GetRef(MakeUnique<FDasNativeFunctionDesc>());
        F.Name = Name;
        F.Helper = Helper;
        return F;
    }
    static void Param(FDasNativeFunctionDesc& F, const FDasNativeClassType* Type)
    {
        F.Arguments.Add({EDasNativeType::Object, nullptr});
        F.Resolved.Add({Type ? Type->Class : UObject::StaticClass(), Type});
    }
    static void Result(FDasNativeFunctionDesc& F, EDasNativeType Kind, const FDasNativeClassType* Type)
    {
        F.Result = {Kind, nullptr};
        F.Resolved.Add(Kind == EDasNativeType::Object
                           ? FDasNativeResolvedParam{Type ? Type->Class : UObject::StaticClass(), Type}
                           : FDasNativeResolvedParam());
    }
    // AOT entries of valid, handle, == and != (the casts' are made per class, FDasNativeModule::AotCast).
    static bool AotValid(const FDasObjectHandle& Value, das::Context* Context)
    {
        return DasNativeAotObject(Value, UObject::StaticClass(), Context, 0, EDasNativeAotObject::Argument) != nullptr;
    }
    static FDasObjectHandle AotHandle(const FDasObjectHandle& Value, das::Context* Context)
    {
        const UObject* Object =
            DasNativeAotObject(Value, UObject::StaticClass(), Context, 0, EDasNativeAotObject::Argument);
        return DasNativeAotHandle(Object, UObject::StaticClass(), Context);
    }
    // == and != compare the objects, as the interpreter does (null and expired handles are equal).
    static bool AotEqual(const FDasObjectHandle& A, const FDasObjectHandle& B, das::Context* Context)
    {
        return DasNativeAotObject(A, UObject::StaticClass(), Context, 1, EDasNativeAotObject::Argument) ==
               DasNativeAotObject(B, UObject::StaticClass(), Context, 2, EDasNativeAotObject::Argument);
    }
    static bool AotNotEqual(const FDasObjectHandle& A, const FDasObjectHandle& B, das::Context* Context)
    {
        return !AotEqual(A, B, Context);
    }
    // valid, handle, ==, != and the casts: from UObjectHandle, to and from the ancestors' types.
    void AddHelpers(const FDasNativeClassType& Type)
    {
        auto Unary = [&](const FString& Name, EDasNativeHelper Helper, EDasNativeType Kind,
                         const FDasNativeClassType* From, const FDasNativeClassType* To)
        {
            auto& F = Generate(Name, Helper);
            Param(F, From);
            Result(F, Kind, To);
            F.AotEntry = Helper == EDasNativeHelper::Valid    ? reinterpret_cast<void*>(&AotValid)
                         : Helper == EDasNativeHelper::Handle ? reinterpret_cast<void*>(&AotHandle)
                                                              : To->AotCast;
            Add(Desc, F);
        };
        auto Binary = [&](const FString& Name, EDasNativeHelper Helper)
        {
            auto& F = Generate(Name, Helper);
            Param(F, &Type);
            Param(F, &Type);
            Result(F, EDasNativeType::Bool, nullptr);
            F.AotEntry = Helper == EDasNativeHelper::Equal ? reinterpret_cast<void*>(&AotEqual)
                                                           : reinterpret_cast<void*>(&AotNotEqual);
            Add(Desc, F);
        };
        Unary(TEXT("valid"), EDasNativeHelper::Valid, EDasNativeType::Bool, &Type, nullptr);
        Unary(TEXT("handle"), EDasNativeHelper::Handle, EDasNativeType::Object, &Type, nullptr);
        Binary(TEXT("=="), EDasNativeHelper::Equal);
        Binary(TEXT("!="), EDasNativeHelper::NotEqual);
        const FString As = TEXT("as_") + Type.Name;
        Unary(As, EDasNativeHelper::Cast, EDasNativeType::Object, nullptr, &Type);
        for (FDasNativeClassType* Ancestor : DasNativeAncestorTypes(Type.Class))
        {
            Unary(As, EDasNativeHelper::Cast, EDasNativeType::Object, Ancestor, &Type);
            Unary(TEXT("as_") + Ancestor->Name, EDasNativeHelper::Cast, EDasNativeType::Object, &Type, Ancestor);
        }
    }
    // A method of an ancestor class bound again with a descendant type as the receiver; it still runs the
    // ancestor's code (Origin). Skipped when this module already has the name with these parameters.
    void Inherit(const FDasNativeModuleDesc& Origin, const FDasNativeFunctionDesc& F, const FDasNativeClassType& Type)
    {
        auto Copy = MakeUnique<FDasNativeFunctionDesc>(F);
        Copy->Resolved[0] = {Type.Class, &Type};
        if (Signatures.Contains(Signature(*Copy)))
            return;
        Add(Origin, *Desc.Generated.Add_GetRef(MoveTemp(Copy)));
    }
};

// Modules a native module needs first: the owners of the script types its functions and classes use.
TSet<FDasNativeModuleDesc*> DasNativeDependencies(const FDasNativeModuleDesc& Desc)
{
    TSet<FDasNativeModuleDesc*> Result;
    auto Use = [&](const UClass* Class)
    {
        if (FDasNativeClassType* Type = FindDasNativeClassType(Class); Type && Type->Owner != &Desc)
            Result.Add(Type->Owner);
    };
    for (const FDasNativeFunctionDesc& F : Desc.Functions)
    {
        for (const FDasNativeParam& P : F.Arguments)
            if (P.Type == EDasNativeType::Object)
                Use(P.Class());
        if (F.Result.Type == EDasNativeType::Object || F.Result.Type == EDasNativeType::ObjectArray)
            Use(F.Result.Class());
    }
    for (FDasNativeClassType* Type : DasNativeClassTypes())
        if (Type->Owner == &Desc)
            for (FDasNativeClassType* Ancestor : DasNativeAncestorTypes(Type->Class))
                Use(Ancestor->Class);
    return Result;
}
// Why a described module cannot be built, or empty.
FString DasNativeModuleProblem(const FDasNativeModuleDesc& Desc)
{
    for (const FDasNativeFunctionDesc& F : Desc.Functions)
        if (F.IsMethod() && !FindDasNativeClassType(F.Receiver()))
            return FString::Printf(TEXT("Native module '%s': method '%s' of class %s, which has no script type "
                                        "(declare it with Class<T>(Name))"),
                                        *Desc.Name, *F.Name, *F.Receiver()->GetName());
    for (FDasNativeModuleDesc* Owner : DasNativeDependencies(Desc))
        if (!Owner->Error.IsEmpty() && !Owner->bBuilt)
            return FString::Printf(TEXT("Native module '%s' uses classes of module '%s', which failed: %s"), *Desc.Name,
                                        *Owner->Name, *Owner->Error);
    return FString();
}

// Builds the daScript modules of newly registered native modules. Runs before every compilation (game thread).
void EnsureDasNativeModules()
{
    if (!IsInGameThread())
        return;
    TArray<FDasNativeModuleDesc*> Pending;
    for (FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
        if (!Desc->bAttempted && Desc->bLoaded)
        {
            FDasNativeModuleBuilder::Describe(*Desc);
            Pending.Add(Desc);
        }
    if (Pending.IsEmpty())
        return;
    auto Fail = [](FDasNativeModuleDesc* Desc, const FString& Why)
    {
        if (Desc->Error.IsEmpty())
            Desc->Error = Why;
    };
    // Script types: one per class and per name. Types left by modules that failed are dropped.
    DasNativeClassTypes().RemoveAll([](FDasNativeClassType* Type)
                                    { return !Type->Owner->bBuilt && !Type->Owner->Error.IsEmpty(); });
    for (FDasNativeModuleDesc* Desc : Pending)
    {
        if (Desc->Error.IsEmpty() && das::Module::require(TCHAR_TO_UTF8(*Desc->Name)))
            Fail(Desc,
                 FString::Printf(TEXT("Native module name '%s' is taken by another daScript module"), *Desc->Name));
        TArray<FDasNativeClassType*> Added;
        for (const FDasNativeClassDecl& Decl : Desc->Classes)
        {
            if (!Desc->Error.IsEmpty())
                break;
            UClass* Class = Decl.Class();
            const FDasNativeClassType* Same = FindDasNativeClassType(Class);
            const FDasNativeClassType* Named = nullptr;
            for (FDasNativeClassType* Type : DasNativeClassTypes())
                if (Type->Name == Decl.ScriptName)
                    Named = Type;
            if (Same)
                Fail(Desc, FString::Printf(TEXT("Native module '%s': class %s already has the script type '%s' "
                                                "(module '%s')"),
                                                *Desc->Name, *Class->GetName(), *Same->Name, *Same->Owner->Name));
            else if (Named)
                Fail(Desc, FString::Printf(TEXT("Native module '%s': script type name '%s' is taken by class %s"),
                                                *Desc->Name, *Decl.ScriptName, *Named->Class->GetName()));
            else
                Added.Add(DasNativeClassTypes().Add_GetRef(
                    new FDasNativeClassType{Decl.ScriptName, Class, Desc, nullptr, Decl.AotCast}));
        }
        if (!Desc->Error.IsEmpty())
            DasNativeClassTypes().RemoveAll([&](FDasNativeClassType* Type) { return Added.Contains(Type); });
    }
    // A failed module fails the modules that use its classes; repeat until nothing changes.
    for (bool bChanged = true; bChanged;)
    {
        bChanged = false;
        for (FDasNativeModuleDesc* Desc : Pending)
            if (Desc->Error.IsEmpty())
                if (const FString Why = DasNativeModuleProblem(*Desc); !Why.IsEmpty())
                {
                    Fail(Desc, Why);
                    bChanged = true;
                }
    }
    // Owners of the types first.
    for (bool bProgress = true; bProgress;)
    {
        bProgress = false;
        for (FDasNativeModuleDesc* Desc : Pending)
        {
            if (!Desc->Error.IsEmpty() || Desc->bBuilt)
                continue;
            bool bReady = true;
            for (FDasNativeModuleDesc* Owner : DasNativeDependencies(*Desc))
                bReady &= Owner->bBuilt;
            if (!bReady)
                continue;
            new FDasNativeDasModule(*Desc);
            Desc->bBuilt = true;
            bProgress = true;
        }
    }
    for (FDasNativeModuleDesc* Desc : Pending)
    {
        if (Desc->Error.IsEmpty() && !Desc->bBuilt)
            Fail(Desc,
                 FString::Printf(TEXT("Native module '%s' depends on native modules that depend on it"), *Desc->Name));
        if (!Desc->Error.IsEmpty())
            UE_LOG(LogDasRuntime, Error, TEXT("%s"), *Desc->Error);
    }
}
