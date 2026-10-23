// Part of DasRuntime.cpp: native binds (DasNativeBinds.h) as daScript modules. One generic call node converts the
// VM arguments to engine values, calls the module's thunk and converts the result back. Classes bound with
// Class<T>(Name) get their own script types (an object handle under another name) with generated helpers.
// Included in place; declaration order matters.
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
bool DasNativeIsMath(EDasNativeType Type)
{
    return Type >= EDasNativeType::Vector;
}
// Results returned through the VM's result slot (copy-on-return).
bool DasNativeIsCmres(EDasNativeType Type)
{
    return DasNativeIsMath(Type) || Type == EDasNativeType::Object;
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
        vec4f Raw[FDasNativeModule::MaxArguments];
        evalArgs(Context, Raw);
        FDasNativeValues Values;
        void* Args[FDasNativeModule::MaxArguments];
        UObject* Objects[FDasNativeModule::MaxArguments] = {};
        for (int32 I = 0; I < Function->Arguments.Num(); ++I)
        {
            const EDasNativeType Type = Function->Arguments[I].Type;
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
                *static_cast<FName*>(Slot) = ResolveDSLName(das::cast<FDasNameValue>::to(Raw[I]), &Context);
                break;
            case EDasNativeType::Text:
                *static_cast<FText*>(Slot) = ResolveDSLText(das::cast<FDasTextValue>::to(Raw[I]), &Context);
                break;
            case EDasNativeType::Object:
                Objects[I] = ResolveArgument(Context, Raw[I], I);
                *static_cast<UObject**>(Slot) = Objects[I];
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
    FDasNativeFunction(const FDasNativeModuleDesc& Origin, const FDasNativeFunctionDesc& F,
                       const das::ModuleLibrary& Lib, const das::string& Name)
        : ExternalFnBase(Name.c_str(), nullptr), Module(&Origin), Function(&F), Label(Name)
    {
        if (F.Helper == EDasNativeHelper::Call && F.IsMethod() && F.Resolved[0].Script)
            Label = das::string(TCHAR_TO_UTF8(*F.Resolved[0].Script->Name)) + "." + Name;
        das::vector<das::TypeDeclPtr> Types{DasNativeTypeDecl(F.Result, F.Resolved.Last(), Lib, false)};
        for (int32 I = 0; I < F.Arguments.Num(); ++I)
            Types.push_back(DasNativeTypeDecl(F.Arguments[I], F.Resolved[I], Lib, true));
        constructExternal(Types);
        // Calls go through the generic node; AOT would need a registered entry (dasue::aot) — a later step.
        noAot = true;
    }
    virtual das::SimNode* makeSimNode(das::Context& Context, const das::vector<das::ExpressionPtr>&) override
    {
        const char* FnName = Context.code->allocateName(this->name);
        return Context.code->makeNode<TDasDiagnosticNode<SimNode_DasNative>>(at, FnName, Module, Function,
                                                                             Label.c_str());
    }
};

// Object parameters of a module's own function: the class each object must have and its script type.
void ResolveDasNativeFunction(FDasNativeFunctionDesc& F)
{
    F.Resolved.SetNum(F.Arguments.Num() + 1);
    auto Resolve = [](const FDasNativeParam& P, FDasNativeResolvedParam& R)
    {
        if (P.Type != EDasNativeType::Object)
            return;
        R.Class = P.Class();
        R.Script = FindDasNativeClassType(R.Class);
    };
    for (int32 I = 0; I < F.Arguments.Num(); ++I)
        Resolve(F.Arguments[I], F.Resolved[I]);
    Resolve(F.Result, F.Resolved.Last());
}

class FDasNativeDasModule final : public das::Module
{
public:
    explicit FDasNativeDasModule(FDasNativeModuleDesc& InDesc)
        : das::Module(TCHAR_TO_UTF8(*InDesc.Name)), Desc(InDesc), Library(this)
    {
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
        auto* Fn = new FDasNativeFunction(Origin, F, Library, das::string(TCHAR_TO_UTF8(*F.Name)));
        const das::SideEffects Effects = EnumHasAnyFlags(F.Flags, EDasBind::Pure)
                                             ? das::SideEffects::none
                                             : das::SideEffects::modifyExternal | das::SideEffects::accessExternal;
        das::addExternFunc(*this, Fn, DasNativeIsCmres(F.Result.Type), Effects);
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
    // valid, handle, ==, != and the casts: from UObjectHandle, to and from the ancestors' types.
    void AddHelpers(const FDasNativeClassType& Type)
    {
        auto Unary = [&](const FString& Name, EDasNativeHelper Helper, EDasNativeType Kind,
                         const FDasNativeClassType* From, const FDasNativeClassType* To)
        {
            auto& F = Generate(Name, Helper);
            Param(F, From);
            Result(F, Kind, To);
            Add(Desc, F);
        };
        auto Binary = [&](const FString& Name, EDasNativeHelper Helper)
        {
            auto& F = Generate(Name, Helper);
            Param(F, &Type);
            Param(F, &Type);
            Result(F, EDasNativeType::Bool, nullptr);
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
        if (F.Result.Type == EDasNativeType::Object)
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
                Added.Add(DasNativeClassTypes().Add_GetRef(new FDasNativeClassType{Decl.ScriptName, Class, Desc}));
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
