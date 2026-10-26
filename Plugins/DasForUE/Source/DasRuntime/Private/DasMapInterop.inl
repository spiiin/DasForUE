// Map key/value composition. anyArgument is borrowed synchronously; no VM pointer or TypeInfo escapes.
const das::TypeAnnotation* MapNameType = nullptr;
const das::TypeAnnotation* MapEnumType = nullptr;

template <class T> T MapArg(vec4f V, const das::TypeInfo* Type, das::Context* C)
{
    if (Type->isRef() || Type->isRefType())
    {
        const auto* P = das::cast<const T*>::to(V);
        if (!P)
            C->throw_error("null map argument reference");
        return *P;
    }
    // Object handles use the managed reference ABI and have no by-value SIMD cast.
    if constexpr (std::is_same_v<T, FDasObjectHandle>)
    {
        C->throw_error("expected managed object map argument reference");
        return {};
    }
    else
        return das::cast<T>::to(V);
}
// Fixed formals use their declared ABI. The VM only emits TypeInfo for anyArgument slots.
template <class T> auto MapTypedArg(vec4f V, das::Context* C)
{
    using Value = std::remove_cv_t<std::remove_reference_t<T>>;
    if constexpr (std::is_reference_v<T>)
    {
        const auto* P = das::cast<const Value*>::to(V);
        if (!P)
            C->throw_error("null typed map argument reference");
        return *P;
    }
    else
        return das::cast<T>::to(V);
}
void CheckMapCall(das::Context& C, das::SimNode_CallBase* Node, vec4f* Args, int32 Count, int32 KeyIndex)
{
    if (!Node || !Args || Node->nArguments != Count)
        C.throw_error("invalid map interop call metadata");
    if (KeyIndex >= 0 && (!Node->types || !Node->types[KeyIndex] || Node->types[KeyIndex]->dimSize))
        C.throw_error("invalid map interop argument metadata");
}
struct FMapKey
{
    ESnapshotKind Kind = ESnapshotKind::Invalid;
    int32 Int = 0;
    uint8 Byte = 0;
    FString String;
    FName Name;
    TObjectPtr<UObject> Object;
    const void* Memory() const
    {
        switch (Kind)
        {
        case ESnapshotKind::Int:
            return &Int;
        case ESnapshotKind::Byte:
        case ESnapshotKind::Enum:
            return &Byte;
        case ESnapshotKind::String:
            return &String;
        case ESnapshotKind::Name:
            return &Name;
        case ESnapshotKind::Object:
            return &Object;
        default:
            return nullptr;
        }
    }
};
FMapKey ReadMapKey(vec4f V, const das::TypeInfo* T, FProperty* Expected, das::Context* C)
{
    FMapKey K;
    if (T->type == das::Type::tInt)
    {
        K.Kind = ESnapshotKind::Int;
        K.Int = MapArg<int32>(V, T, C);
    }
    else if (T->type == das::Type::tUInt8)
    {
        K.Kind = ESnapshotKind::Byte;
        K.Byte = MapArg<uint8>(V, T, C);
    }
    else if (T->type == das::Type::tString)
    {
        K.Kind = ESnapshotKind::String;
        K.String = FDasKindString::FromDas(MapArg<char*>(V, T, C), C);
    }
    else if (T->type == das::Type::tHandle && T->getAnnotation() == MapNameType)
    {
        K.Kind = ESnapshotKind::Name;
        K.Name = ResolveDSLName(MapArg<FDasNameValue>(V, T, C), C);
    }
    else if (T->type == das::Type::tHandle && T->getAnnotation() == MapEnumType)
    {
        const auto& E = ResolveEnumValue(MapArg<FDasEnumValue>(V, T, C), C);
        if (DasEnumPropertyType(Expected) != E.Type.Get() || Expected->GetElementSize() != 1 || E.Value < 0 ||
            E.Value > 255)
            C->throw_error("snapshot map enum key identity/storage mismatch");
        K.Kind = ESnapshotKind::Enum;
        K.Byte = uint8(E.Value);
    }
    else if (T->type == das::Type::tUInt64 && IsDasObjectHashProperty(Expected))
    {
        K.Kind = ESnapshotKind::Object;
        auto* O = ResolveObjectKey(MapArg<uint64>(V, T, C), C);
        if (O && !DasReferenceMatches(O, DasReferencePropertyClass(Expected)))
            C->throw_error("snapshot map object key class mismatch");
        K.Object = O;
    }
    else
        C->throw_error("unsupported map key argument type");
    if (DasPropertyKind(Expected) != K.Kind)
        C->throw_error("map key/property type mismatch");
    return K;
}

template <class K> struct TMapValue
{
    using In = typename K::In;
    using Out = typename K::Out;
    static constexpr ESnapshotKind Kind = SnapshotValueKind<typename K::Property>();
    static auto Prepare(In V, das::Context* C) { return K::FromDas(V, C); }
    static Out Read(FProperty* P, const void* M, das::Context* C, das::LineInfoArg*)
    {
        return K::ToDas(ReadPropertyScalarValue(CastFieldChecked<typename K::Property>(P), M, C), C);
    }
    template <class Native> static void Write(FProperty* P, void* M, const Native& V, das::Context* C)
    {
        WriteNativePropertyValue(P, M, PropertyScalarValue<typename K::Property>(V), C);
    }
};
struct FMapEnum
{
    using In = FDasEnumValue;
    using Out = FDasEnumValue;
    struct Native
    {
        UEnum* Type;
        int32 Value;
    };
    static constexpr ESnapshotKind Kind = ESnapshotKind::Enum;
    static Native Prepare(In V, das::Context* C)
    {
        const auto& E = ResolveEnumValue(V, C);
        return {E.Type.Get(), E.Value};
    }
    static Out Read(FProperty* P, const void* M, das::Context* C, das::LineInfoArg*)
    {
        return MakeEnumValue(DasEnumPropertyType(P)->GetPathName(), ReadNativeEnumNumber(P, M, C), C);
    }
    static void Write(FProperty* P, void* M, const Native& V, das::Context* C)
    {
        WriteNativeEnumNumber(P, M, V.Type, V.Value, C);
    }
};
struct FMapObject
{
    using In = const FDasObjectHandle&;
    using Out = uint64;
    static constexpr ESnapshotKind Kind = ESnapshotKind::Object;
    static UObject* Prepare(In V, das::Context* C) { return V.Token ? ResolveObject(V, C) : nullptr; }
    static Out Read(FProperty* P, const void* M, das::Context* C, das::LineInfoArg*)
    {
        return ReadNullablePropertyObject(P, M, C).Token;
    }
    static void Write(FProperty* P, void* M, UObject* V, das::Context* C)
    {
        WriteNativePropertyValue(P, M, SnapshotObjectValue(P, V, C), C);
    }
};
struct FMapStruct
{
    using In = const FDasStructValue&;
    using Out = uint64;
    static constexpr ESnapshotKind Kind = ESnapshotKind::Struct;
    static FDasStructValue Prepare(In V, das::Context*) { return V; }
    static Out Read(FProperty* P, const void* M, das::Context* C, das::LineInfoArg* At)
    {
        return ReadPropertySnapshot(P, M, C, At).Token;
    }
    static void Write(FProperty* P, void* M, FDasStructValue V, das::Context* C) { WritePropertySnapshot(P, M, V, C); }
};

template <class VK> vec4f MapGet(das::Context& C, das::SimNode_CallBase* Node, vec4f* A)
{
    CheckMapCall(C, Node, A, 3, 2);
    const auto H = MapTypedArg<const FDasStructValue&>(A[0], &C);
    const auto Path = MapTypedArg<const char*>(A[1], &C);
    const auto& S = ResolveStructValue(H, &C);
    auto Field = StructLeaf(S, Path, &C);
    auto* P = CastField<FMapProperty>(Field.Property);
    if (!P)
        C.throw_error("expected snapshot map");
    const auto Key = ReadMapKey(A[2], Node->types[2], P->KeyProp, &C);
    auto View = SnapshotMap(Field, Key.Kind, VK::Kind, &C);
    FConstStructLeaf Value{};
    FString Error;
    if (!View.Find(Key.Memory(), Value, Error))
        C.throw_error(TCHAR_TO_UTF8(*Error));
    return das::cast<typename VK::Out>::from(
        VK::Read(Value.Property, Value.Memory, &C, das::cast_arg<das::LineInfoArg*>::to(C, Node)));
}
template <class VK> vec4f MapWith(das::Context& C, das::SimNode_CallBase* Node, vec4f* A)
{
    CheckMapCall(C, Node, A, 4, 2);
    const auto H = MapTypedArg<const FDasStructValue&>(A[0], &C);
    const auto Path = MapTypedArg<const char*>(A[1], &C);
    auto Field = StructLeaf(ResolveStructValue(H, &C), Path, &C);
    auto* P = CastField<FMapProperty>(Field.Property);
    if (!P)
        C.throw_error("expected snapshot map");
    const auto Key = ReadMapKey(A[2], Node->types[2], P->KeyProp, &C);
    const auto Value = VK::Prepare(MapTypedArg<typename VK::In>(A[3], &C), &C);
    FStructMutation Mutation(H, &C);
    auto View = SnapshotMap(StructLeaf(Mutation.Value, Path, &C), Key.Kind, VK::Kind, &C);
    FString Error;
    if (!View.Upsert(
            Key.Memory(),
            [&](FProperty* Target, void* Memory, FString&)
            {
                VK::Write(Target, Memory, Value, &C);
                return true;
            },
            Error))
        C.throw_error(TCHAR_TO_UTF8(*Error));
    return das::cast<uint64>::from(Mutation.Finish(&C, das::cast_arg<das::LineInfoArg*>::to(C, Node)).Token);
}
template <class VK, bool Write, bool NameKey = false>
vec4f SelfMap(das::Context& C, das::SimNode_CallBase* Node, vec4f* A)
{
    CheckMapCall(C, Node, A, Write ? 3 : 2, NameKey ? -1 : 1);
    auto Name = MapTypedArg<const char*>(A[0], &C);
    UObject* Self = nullptr;
    auto* P = SelfContainer<FMapProperty>(Self, Name, Write, &C);
    auto* V = SelfContainerElement<VK>(P->ValueProp, &C);
    const auto Key = [&]
    {
        if constexpr (NameKey)
        {
            if (DasPropertyKind(P->KeyProp) != ESnapshotKind::Name)
                C.throw_error("map key/property type mismatch");
            FMapKey K;
            K.Kind = ESnapshotKind::Name;
            K.Name = ResolveDSLName(MapTypedArg<FDasNameValue>(A[1], &C), &C);
            return K;
        }
        else
            return ReadMapKey(A[1], Node->types[1], P->KeyProp, &C);
    }();
    FScriptMapHelper Map(P, P->ContainerPtrToValuePtr<void>(Self));
    if constexpr (Write)
    {
        const auto Value = VK::FromDas(MapTypedArg<typename VK::In>(A[2], &C), &C);
        if (!Map.FindValueFromHash(Key.Memory()))
            SelfContainerGrow(Map.Num(), false, &C);
        Map.AddPair(Key.Memory(), &Value);
        return v_zero();
    }
    else
    {
        if (const uint8* Value = Map.FindValueFromHash(Key.Memory()))
            return das::cast<typename VK::Out>::from(VK::ToDas(V->GetPropertyValue(Value), &C));
        return das::cast<typename VK::Out>::from(VK::ToDas(TDasKindNative<VK>(), &C));
    }
}
FDasStructValue MapStructToken(uint64 Token, das::Context* C)
{
    FDasStructValue V{Token};
    ResolveStructValue(V, C);
    return V;
}
FDasObjectHandle MapObjectToken(uint64 Token, das::Context* C)
{
    FDasObjectHandle V{Token};
    RequireObjectContext(C);
    if (Token)
        ResolveObject(V, C);
    return V;
}

struct FMapKeyAnnotation : das::FunctionAnnotation
{
    int Index;
    explicit FMapKeyAnnotation(int I)
        : FunctionAnnotation(I == 1 ? "__ue_self_map_key" : "__ue_snapshot_map_key"), Index(I)
    {
    }
    bool apply(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    bool finalize(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&,
                  const das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
    bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, const das::AnnotationArgumentList&,
                  das::string&) override
    {
        return false;
    }
    bool verifyCall(das::ExprCallFunc* Call, das::AnnotationArgumentList&, const das::AnnotationArgumentList&,
                    das::string& Error) override
    {
        const auto T = Call->arguments[Index]->type;
        if (T &&
            (T->baseType == das::Type::tInt || T->baseType == das::Type::tUInt8 || T->baseType == das::Type::tString ||
             (T->baseType == das::Type::tHandle &&
              (T->annotation == MapNameType || (Index == 2 && T->annotation == MapEnumType))) ||
             (Index == 2 && T->baseType == das::Type::tUInt64)))
            return true;
        Error = "unsupported map key argument type";
        return false;
    }
};
void AnnotateMap(das::Function* Fn, FMapKeyAnnotation* Annotation)
{
    Fn->needCallerStackFrame = Annotation->Index == 2;
    auto* A = new das::AnnotationDeclaration();
    A->flags = 0;
    A->annotation = Annotation;
    A->at = Fn->at;
    Fn->annotations.push_back(A);
}
struct FMapResultMacro : das::CallMacro
{
    das::string Target, Convert;
    FMapResultMacro(const das::string& N, const das::string& T, const char* C) : CallMacro(N), Target(T), Convert(C) {}
    das::ExpressionPtr visit(das::Program*, das::Module*, das::ExprCallMacro* E) override
    {
        auto* Call = new das::ExprCall(E->at, Target);
        Call->arguments = E->arguments;
        return DSLCall(E->at, Convert, {Call});
    }
};
void AddMapResultMacro(das::Module& M, const das::string& Name, const das::string& Target, const char* Convert)
{
    auto* Macro = new FMapResultMacro(Name, Target, Convert);
    Macro->module = &M;
    M.ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));
    M.addCallMacro(Name,
                   [Macro](const das::LineInfo& At) -> das::ExprLooksLikeCall*
                   {
                       auto* E = new das::ExprCallMacro(At, Macro->name);
                       E->macro = Macro;
                       return E;
                   });
}
template <class VK>
void AddSnapshotMap(das::Module& M, const das::ModuleLibrary& L, FMapKeyAnnotation* A, const char* Suffix,
                    const char* Convert = nullptr)
{
    const das::string Get = das::string("ue_struct_map_get_") + Suffix;
    const das::string With = das::string("ue_struct_map_with_") + Suffix;
    const das::string GetNative = Convert ? "__ue_map_get_token_" + das::string(Suffix) : Get;
    AnnotateMap(AddUEInterop<MapGet<VK>, typename VK::Out, const FDasStructValue&, const char*, vec4f>(
                    M, L, GetNative.c_str(), das::SideEffects::modifyExternal),
                A);
    const das::string WithNative = "__ue_map_with_token_" + das::string(Suffix);
    AnnotateMap(AddUEInterop<MapWith<VK>, uint64, const FDasStructValue&, const char*, vec4f, typename VK::In>(
                    M, L, WithNative.c_str(), das::SideEffects::modifyExternal),
                A);
    AddMapResultMacro(M, With, WithNative, "__ue_map_struct_token");
    if (Convert)
        AddMapResultMacro(M, Get, GetNative, Convert);
}
template <class VK>
void AddSelfMap(das::Module& M, const das::ModuleLibrary& L, FMapKeyAnnotation* A, const char* Suffix)
{
    const das::string Get = das::string("__ue_self_map_get_") + Suffix;
    const das::string Set = das::string("__ue_self_map_set_") + Suffix;
    AnnotateMap(AddUEInterop<SelfMap<VK, false>, typename VK::Out, const char*, vec4f>(
                    M, L, Get.c_str(), das::SideEffects::modifyExternal),
                A);
    AnnotateMap(AddUEInterop<SelfMap<VK, true>, void, const char*, vec4f, typename VK::In>(
                    M, L, Set.c_str(), das::SideEffects::modifyExternal),
                A);
    // Exact name-key overloads preserve the pre-existing closed AOT boundary of the opaque name ABI.
    // All overloads still share the same implementation, not key x value native instantiations.
    AddUEInterop<SelfMap<VK, false, true>, typename VK::Out, const char*, FDasNameValue>(
        M, L, Get.c_str(), das::SideEffects::modifyExternal);
    AddUEInterop<SelfMap<VK, true, true>, void, const char*, FDasNameValue, typename VK::In>(
        M, L, Set.c_str(), das::SideEffects::modifyExternal);
}
#if DAS_WITH_TEST_PROBES
#include "DasMapInteropReference.inl"
#endif

void AddMapInterops(das::Module& M, const das::ModuleLibrary& L)
{
#if DAS_WITH_TEST_PROBES
    AddUEExtern<DAS_BIND_FUN(MapProbeTypedGet)>(M, L, "__ue_probe_typed_map_get_int", das::SideEffects::modifyExternal);
    AddUEExtern<DAS_BIND_FUN(MapProbeTypedSet)>(M, L, "__ue_probe_typed_map_set_int", das::SideEffects::modifyExternal);
#endif
#if DAS_WITH_TEST_PROBES
    AddUEExtern<DAS_BIND_FUN(MapProbeBindingCount)>(M, L, "__ue_probe_map_binding_count",
                                                    das::SideEffects::accessExternal);
#endif
    MapNameType = das::makeType<FDasNameValue>(L)->annotation;
    MapEnumType = das::makeType<FDasEnumValue>(L)->annotation;
    auto* SelfKeys = new FMapKeyAnnotation(1);
    auto* SnapshotKeys = new FMapKeyAnnotation(2);
    M.addAnnotation(SelfKeys);
    M.addAnnotation(SnapshotKeys);
    AddUEExtern<DAS_BIND_FUN(MapStructToken), das::SimNode_ExtFuncCallAndCopyOrMove>(M, L, "__ue_map_struct_token",
                                                                                     das::SideEffects::modifyExternal);
    AddUEExtern<DAS_BIND_FUN(MapObjectToken), das::SimNode_ExtFuncCallAndCopyOrMove>(M, L, "__ue_map_object_token",
                                                                                     das::SideEffects::modifyExternal);
#define ADD_MAP_KIND(Suffix, Kind, RefKind)                                                                            \
    AddSelfMap<Kind>(M, L, SelfKeys, #Suffix);                                                                         \
    AddSnapshotMap<TMapValue<Kind>>(M, L, SnapshotKeys, #Suffix);
    DAS_SCALAR_ACCESSOR_KINDS(ADD_MAP_KIND)
#undef ADD_MAP_KIND
    AddSnapshotMap<FMapEnum>(M, L, SnapshotKeys, "enum");
    AddSnapshotMap<FMapObject>(M, L, SnapshotKeys, "object", "__ue_map_object_token");
    AddSnapshotMap<FMapStruct>(M, L, SnapshotKeys, "struct", "__ue_map_struct_token");
}
