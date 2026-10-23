// Part of DasRuntime.cpp: native binds (DasNativeBinds.h) as daScript modules. One generic call node converts the
// VM arguments to engine values, calls the module's thunk and converts the result back.
// Included in place; declaration order matters.
das::TypeDeclPtr DasNativeTypeDecl(EDasNativeType Type, const das::ModuleLibrary& Lib, bool bArgument)
{
    switch (Type)
    {
    case EDasNativeType::Void:
        return das::makeType<void>(Lib);
    case EDasNativeType::Bool:
        return das::makeType<bool>(Lib);
    case EDasNativeType::Int32:
        return das::makeType<int32_t>(Lib);
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
bool DasNativeIsMath(EDasNativeType Type)
{
    return Type >= EDasNativeType::Vector;
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
    }
};
static_assert(sizeof(FString) <= sizeof(FTransform) && sizeof(FText) <= sizeof(FTransform) &&
              sizeof(FLinearColor) <= sizeof(FTransform) && sizeof(FQuat) <= sizeof(FTransform));

struct SimNode_DasNative : das::SimNode_ExtFuncCallBase
{
    const FDasNativeModuleDesc* Module = nullptr;
    const FDasNativeFunctionDesc* Function = nullptr;
    SimNode_DasNative(const das::LineInfo& At, const char* Name, const FDasNativeModuleDesc* M,
                      const FDasNativeFunctionDesc* F)
        : SimNode_ExtFuncCallBase(At, Name), Module(M), Function(F)
    {
    }
    DAS_EVAL_ABI virtual vec4f eval(das::Context& Context) override
    {
        GuardUEExtern();
        if (!Module->bLoaded)
            Context.throw_error_at(debugInfo, "%s", TCHAR_TO_UTF8(*Module->Error));
        vec4f Raw[FDasNativeModule::MaxArguments];
        evalArgs(Context, Raw);
        FDasNativeValues Values;
        void* Args[FDasNativeModule::MaxArguments];
        for (int32 I = 0; I < Function->Arguments.Num(); ++I)
        {
            const EDasNativeType Type = Function->Arguments[I];
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
                *static_cast<FName*>(Slot) = ResolveDSLName(das::cast<FDasNameValue>::to(Raw[I]), &Context);
                break;
            case EDasNativeType::Text:
                *static_cast<FText*>(Slot) = ResolveDSLText(das::cast<FDasTextValue>::to(Raw[I]), &Context);
                break;
            default:
                break;
            }
            Args[I] = Slot;
        }
        const EDasNativeType Result = Function->Result;
        // Math results are written straight into the VM's result slot (copy-on-return, like other value externs).
        void* Out = DasNativeIsMath(Result)          ? cmresEval->evalPtr(Context)
                    : Result == EDasNativeType::Void ? nullptr
                                                     : Values.Construct(Result);
        Function->Thunk(Args, Out);
        switch (Result)
        {
        case EDasNativeType::Void:
            return v_zero();
        case EDasNativeType::Bool:
            return das::cast<bool>::from(*static_cast<bool*>(Out));
        case EDasNativeType::Int32:
            return das::cast<int32_t>::from(*static_cast<int32*>(Out));
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
    FDasNativeFunction(const FDasNativeModuleDesc& M, const FDasNativeFunctionDesc& F, const das::ModuleLibrary& Lib,
                       const das::string& Name)
        : ExternalFnBase(Name.c_str(), nullptr), Module(&M), Function(&F)
    {
        das::vector<das::TypeDeclPtr> Types{DasNativeTypeDecl(F.Result, Lib, false)};
        for (EDasNativeType Type : F.Arguments)
            Types.push_back(DasNativeTypeDecl(Type, Lib, true));
        constructExternal(Types);
        // Calls go through the generic node; AOT would need a registered entry (dasue::aot) — a later step.
        noAot = true;
    }
    virtual das::SimNode* makeSimNode(das::Context& Context, const das::vector<das::ExpressionPtr>&) override
    {
        const char* FnName = Context.code->allocateName(this->name);
        return Context.code->makeNode<TDasDiagnosticNode<SimNode_DasNative>>(at, FnName, Module, Function);
    }
};

class FDasNativeDasModule final : public das::Module
{
public:
    explicit FDasNativeDasModule(FDasNativeModuleDesc& Desc) : das::Module(TCHAR_TO_UTF8(*Desc.Name))
    {
        das::ModuleLibrary Library(this);
        Library.addBuiltInModule();
        auto* Foundation = das::Module::require("ue_foundation");
        Library.addModule(Foundation);
        // Scripts that require a native module also see the engine value types it uses.
        addDependency(Foundation, true);
        for (const FDasNativeFunctionDesc& F : Desc.Functions)
        {
            const das::string Name = TCHAR_TO_UTF8(*F.Name);
            auto* Fn = new FDasNativeFunction(Desc, F, Library, Name);
            const das::SideEffects Effects = EnumHasAnyFlags(F.Flags, EDasBind::Pure)
                                                 ? das::SideEffects::none
                                                 : das::SideEffects::modifyExternal | das::SideEffects::accessExternal;
            das::addExternFunc(*this, Fn, DasNativeIsMath(F.Result), Effects);
        }
    }
};

// Builds the daScript modules of newly registered native modules. Runs before every compilation (game thread).
void EnsureDasNativeModules()
{
    if (!IsInGameThread())
        return;
    for (FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
    {
        if (Desc->bAttempted || !Desc->bLoaded)
            continue;
        FDasNativeModuleBuilder::Describe(*Desc);
        if (Desc->Error.IsEmpty() && das::Module::require(TCHAR_TO_UTF8(*Desc->Name)))
            Desc->Error =
                FString::Printf(TEXT("Native module name '%s' is taken by another daScript module"), *Desc->Name);
        if (!Desc->Error.IsEmpty())
        {
            UE_LOG(LogDasRuntime, Error, TEXT("%s"), *Desc->Error);
            continue;
        }
        new FDasNativeDasModule(*Desc);
        Desc->bBuilt = true;
    }
}
