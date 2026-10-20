// Part of DasRuntime.cpp: scalar property accessors shared by self, authored-object and native-reference access.
// Included in place; declaration order matters.
//
// One kind row describes how a daScript scalar maps to an Unreal property: the
// property class, the daScript argument/result types and the conversions. The
// accessor families below are templates over a kind; DAS_SCALAR_ACCESSOR_KINDS
// registers every (family, kind) pair, so a new scalar kind is one row here.
// Enum and class (DAS_REFERENCE_ACCESSOR_KINDS) share the self/authored/native
// families through their own Read/Write.

struct FDasKindInt
{
    static constexpr ESnapshotKind SnapshotKind = ESnapshotKind::Int; // as a map/set key
    using Property = FIntProperty;
    using In = int32;
    using Out = int32;
    static constexpr const TCHAR* Name = TEXT("int");
    static int32 ToDas(int32 V, das::Context*) { return V; }
    static int32 FromDas(int32 V, das::Context*) { return V; }
    static void Check(Property*, das::Context*) {}
};
struct FDasKindFloat
{
    using Property = FFloatProperty;
    using In = float;
    using Out = float;
    static constexpr const TCHAR* Name = TEXT("float");
    static float ToDas(float V, das::Context*) { return V; }
    static float FromDas(float V, das::Context*) { return V; }
    static void Check(Property*, das::Context*) {}
};
struct FDasKindDouble
{
    using Property = FDoubleProperty;
    using In = double;
    using Out = double;
    static constexpr const TCHAR* Name = TEXT("double");
    static double ToDas(double V, das::Context*) { return V; }
    static double FromDas(double V, das::Context*) { return V; }
    static void Check(Property*, das::Context*) {}
};
struct FDasKindBool
{
    using Property = FBoolProperty;
    using In = bool;
    using Out = bool;
    static constexpr const TCHAR* Name = TEXT("bool");
    static bool ToDas(bool V, das::Context*) { return V; }
    static bool FromDas(bool V, das::Context*) { return V; }
    static void Check(Property*, das::Context*) {}
};
struct FDasKindString
{
    static constexpr ESnapshotKind SnapshotKind = ESnapshotKind::String; // as a map/set key
    using Property = FStrProperty;
    using In = const char*;
    using Out = char*;
    static constexpr const TCHAR* Name = TEXT("string");
    static char* ToDas(const FString& V, das::Context* C) { return CopyDSLString(V, C); }
    static FString FromDas(const char* V, das::Context*) { return V ? UTF8_TO_TCHAR(V) : TEXT(""); }
    static void Check(Property*, das::Context*) {}
};
struct FDasKindName
{
    static constexpr ESnapshotKind SnapshotKind = ESnapshotKind::Name; // as a map/set key
    using Property = FNameProperty;
    using In = FDasNameValue;
    using Out = FDasNameValue;
    static constexpr const TCHAR* Name = TEXT("name");
    static FDasNameValue ToDas(FName V, das::Context* C) { return MakeDSLName(V, C); }
    static FName FromDas(FDasNameValue V, das::Context* C) { return ResolveDSLName(V, C); }
    static void Check(Property*, das::Context*) {}
};
struct FDasKindText
{
    using Property = FTextProperty;
    using In = FDasTextValue;
    using Out = FDasTextValue;
    static constexpr const TCHAR* Name = TEXT("text");
    static FDasTextValue ToDas(const FText& V, das::Context* C) { return MakeDSLText(V, C); }
    static FText FromDas(FDasTextValue V, das::Context* C) { return ResolveDSLText(V, C); }
    static void Check(Property*, das::Context*) {}
};
// Raw (non-enum) byte as uint8: the self accessors.
struct FDasKindByte
{
    static constexpr ESnapshotKind SnapshotKind = ESnapshotKind::Byte; // as a map/set key
    using Property = FByteProperty;
    using In = uint8;
    using Out = uint8;
    static constexpr const TCHAR* Name = TEXT("byte");
    static uint8 ToDas(uint8 V, das::Context*) { return V; }
    static uint8 FromDas(uint8 V, das::Context*) { return V; }
    static void Check(Property* P, das::Context* C)
    {
        if (P->Enum)
            C->throw_error("expected raw byte");
    }
};
// Raw byte passed as int with a range check: authored and native references.
struct FDasKindByteInt : FDasKindByte
{
    using In = int32;
    using Out = int32;
    static int32 ToDas(uint8 V, das::Context*) { return V; }
    static uint8 FromDas(int32 V, das::Context* C)
    {
        if (V < 0 || V > 255)
            C->throw_error("byte must be 0..255");
        return uint8(V);
    }
};

// Reference kinds: no FDasScalarValue slot, so the kind reads and writes the property itself.
struct FDasKindEnum
{
    using Property = FProperty;
    using In = FDasEnumValue;
    using Out = FDasEnumValue;
    static constexpr const TCHAR* Name = TEXT("enum");
    static constexpr const TCHAR* PropertyName = TEXT("EnumProperty"); // FEnumProperty or an enum FByteProperty
    static void Check(Property*, das::Context*) {}
    static FDasEnumValue Read(Property* P, UObject* O, das::Context* C) { return ReadPropertyEnum(P, O, C); }
    static void Write(Property* P, UObject* O, FDasEnumValue V, das::Context* C) { WritePropertyEnum(P, O, V, C); }
};
// Class values are checked against the property's MetaClass in both directions.
struct FDasKindClass
{
    using Property = FClassProperty;
    using In = FDasClassHandle;
    using Out = FDasClassHandle;
    static constexpr const TCHAR* Name = TEXT("class");
    static void Check(Property*, das::Context*) {}
    static FDasClassHandle Read(Property* P, UObject* O, das::Context* C)
    {
        auto* Class = ReadPropertyClass(P, O, C);
        if (!Class)
            return {};
        CheckedClass(Class, TCHAR_TO_UTF8(*P->MetaClass->GetPathName()), "", C);
        return {MakeObjectHandle(Class, UClass::StaticClass(), C).Token};
    }
    static void Write(Property* P, UObject* O, FDasClassHandle V, das::Context* C)
    {
        UClass* Class = nullptr;
        if (V.Token)
            Class = CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{V.Token}, C)),
                                 TCHAR_TO_UTF8(*P->MetaClass->GetPathName()), "", C);
        WritePropertyClass(P, O, Class, C);
    }
};

// Scalar kinds go through ReadPropertyScalar/WritePropertyScalar; a kind with Read/Write handles the property itself.
template <class K> typename K::Out DasKindRead(typename K::Property* P, UObject* O, das::Context* C)
{
    if constexpr (requires { &K::Read; })
        return K::Read(P, O, C);
    else
        return K::ToDas(ReadPropertyScalar(P, O, C), C);
}
template <class K> void DasKindWrite(typename K::Property* P, UObject* O, typename K::In V, das::Context* C)
{
    if constexpr (requires { &K::Write; })
        K::Write(P, O, V, C);
    else
        WritePropertyScalar(P, O, K::FromDas(V, C), C);
}

// Extra validation for writes through generated native references.
template <class T> void DasNativeScalarWritePolicy(const char*, const char*, const T&, das::Context*) {}
inline void DasNativeScalarWritePolicy(const char*, const char*, double V, das::Context* C)
{
    if (!FMath::IsFinite(V))
        C->throw_error("native numeric field must be finite");
}
inline void DasNativeScalarWritePolicy(const char* Path, const char* Name, float V, das::Context* C)
{
    if (!FMath::IsFinite(V))
        C->throw_error("native numeric field must be finite");
    if (V < 0 && Path && Name && FCStringAnsi::Strcmp(Path, "/Script/Engine.CharacterMovementComponent") == 0 &&
        (FCStringAnsi::Strcmp(Name, "MaxWalkSpeed") == 0 || FCStringAnsi::Strcmp(Name, "JumpZVelocity") == 0))
        C->throw_error("movement speed/jump velocity must be nonnegative");
}

// Self property sites: a small direct-mapped cache per (kind, read/write) keyed by the property name.
// An entry is filled only after the checked path succeeded for a plain property (validated contract,
// no accessor, writable for writes) and is used while the same self class, the same FProperty (field path
// still resolves to it) and the cache epoch hold; everything else, including every error, takes the
// checked path.
struct FDasSelfSite
{
    das::string Name;
    UClass* Class = nullptr;
    uint64 Epoch = 0;
    FProperty* Raw = nullptr;
    TFieldPath<FProperty> Property;
    bool bActor = false;
};
template <class K> constexpr bool DasPlainKind = !requires { &K::Read; };
template <class K, bool bWrite> FDasSelfSite& DasSelfSite(const char* Name)
{
    static FDasSelfSite Sites[256];
    const UPTRINT Key = UPTRINT(Name);
    return Sites[((Key >> 4) ^ (Key >> 12)) & 255];
}
template <class K, bool bWrite> typename K::Property* DasFindSelfSite(const char* Name, das::Context* C, UObject*& Self)
{
    if (!GDasSelfSitesEnabled || !Name || !ActiveContext || C != ActiveContext || !IsInGameThread() ||
        LibraryInvocationContract || ActiveContext->bLibraryScope ||
        (bWrite && ActiveContext->bCollectingSubscriptions))
        return nullptr;
    const FDasSelfSite& S = DasSelfSite<K, bWrite>(Name);
    // Names may be heap strings whose address is reused, so the content is compared too.
    if (S.Epoch != GDasSelfSiteEpoch || FCStringAnsi::Strncmp(Name, S.Name.c_str(), S.Name.size() + 1) != 0)
        return nullptr;
    // A weak pointer resolves to null for garbage objects, which matches IsValid.
    UObject* O = ActiveContext->Self.Get();
    if (!O || O->GetClass() != S.Class || (S.bActor && static_cast<AActor*>(O)->IsActorBeingDestroyed()))
        return nullptr;
    FProperty* P = S.Property.Get();
    if (!P || P != S.Raw)
        return nullptr;
    Self = O;
    return static_cast<typename K::Property*>(P);
}
template <class K, bool bWrite>
void DasStoreSelfSite(const char* Name, UObject* Self, typename K::Property* P, das::Context* C)
{
    if (!GDasSelfSitesEnabled || !DasPlainScalarAccess(P, Self, ObjectPropertyAccess(P, C), bWrite) ||
        (bWrite && P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))
        return;
    FDasSelfSite& S = DasSelfSite<K, bWrite>(Name);
    S.Name = Name;
    S.Class = Self->GetClass();
    S.Epoch = GDasSelfSiteEpoch;
    S.Raw = P;
    S.Property = TFieldPath<FProperty>(P);
    S.bActor = Self->IsA<AActor>();
}

// self: properties of the running script owner.
template <class K> typename K::Out GetSelfScalar(const char* Name, das::Context* C)
{
    if constexpr (DasPlainKind<K>)
    {
        UObject* Cached = nullptr;
        if (auto* P = DasFindSelfSite<K, false>(Name, C, Cached))
            return K::ToDas(P->GetPropertyValue_InContainer(Cached), C);
    }
    UObject* Self = ResolveSelf(C);
    auto* P = ResolveProperty<typename K::Property>(Self, Name, false, C);
    K::Check(P, C);
    if constexpr (DasPlainKind<K>)
        DasStoreSelfSite<K, false>(Name, Self, P, C);
    return DasKindRead<K>(P, Self, C);
}
template <class K> void SetSelfScalar(const char* Name, typename K::In Value, das::Context* C)
{
    if constexpr (DasPlainKind<K>)
    {
        UObject* Cached = nullptr;
        if (auto* P = DasFindSelfSite<K, true>(Name, C, Cached))
        {
            const auto Native = K::FromDas(Value, C);
            if (DasFiniteScalar(Native))
            {
                P->SetPropertyValue_InContainer(Cached, Native);
                return;
            }
        }
    }
    if (ActiveContext && ActiveContext->bCollectingSubscriptions)
        C->throw_error("self writes are forbidden in bind_events");
    UObject* Self = ResolveSelf(C);
    auto* P = ResolveProperty<typename K::Property>(Self, Name, true, C);
    K::Check(P, C);
    if constexpr (DasPlainKind<K>)
        DasStoreSelfSite<K, true>(Name, Self, P, C);
    DasKindWrite<K>(P, Self, Value, C);
}

// Authored object references: properties of DSL-declared classes by stable type id.
template <class K>
typename K::Out AuthoredGetScalar(const FDasObjectHandle& H, const char* Id, const char* Name, das::Context* C)
{
    auto* O = ResolveAuthoredObject(H, Id, C);
    auto* P = ResolveProperty<typename K::Property>(O, Name, false, C);
    K::Check(P, C);
    return DasKindRead<K>(P, O, C);
}
template <class K>
void AuthoredSetScalar(const FDasObjectHandle& H, const char* Id, const char* Name, typename K::In Value,
                       das::Context* C)
{
    if (ActiveContext && ActiveContext->bCollectingSubscriptions)
        C->throw_error("object writes are forbidden in bind_events");
    auto* O = ResolveAuthoredObject(H, Id, C);
    auto* P = ResolveProperty<typename K::Property>(O, Name, true, C);
    K::Check(P, C);
    DasKindWrite<K>(P, O, Value, C);
}

// Generated native references: properties listed in the native gameplay registry.
template <class K>
typename K::Out NativeReferenceGetScalar(const FDasObjectHandle& H, const char* Path, const char* Name, das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, false, C);
    auto* P = CastFieldChecked<typename K::Property>(NativeGameplayProperty(O, Path, Name, false, K::Name, C));
    K::Check(P, C);
    return DasKindRead<K>(P, O, C);
}
template <class K>
void NativeReferenceSetScalar(const FDasObjectHandle& H, const char* Path, const char* Name, typename K::In Value,
                              das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, true, C);
    auto* P = CastFieldChecked<typename K::Property>(NativeGameplayProperty(O, Path, Name, true, K::Name, C));
    K::Check(P, C);
    if constexpr (requires { &K::Write; })
        K::Write(P, O, Value, C);
    else
    {
        const auto Native = K::FromDas(Value, C);
        DasNativeScalarWritePolicy(Path, Name, Native, C);
        WritePropertyScalar(P, O, Native, C);
    }
}

// Struct snapshots: scalar leaves addressed by field path.
template <class K> typename K::Out StructGetScalar(const FDasStructValue& H, const char* Path, das::Context* C)
{
    const auto& V = ResolveStructValue(H, C);
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<typename K::Property>(F.Property);
    if (!P)
        C->throw_error("struct field scalar type mismatch");
    K::Check(P, C);
    return K::ToDas(ReadPropertyScalarValue(P, F.Memory, C), C);
}
template <class K>
FDasStructValue StructWithScalar(const FDasStructValue& H, const char* Path, typename K::In X, das::Context* C,
                                 das::LineInfoArg* At)
{
    FStructMutation Mutation(H, C);
    auto& V = Mutation.Value;
    auto F = StructLeaf(V, Path, C);
    auto* P = CastField<typename K::Property>(F.Property);
    if (!P)
        C->throw_error("struct field scalar type mismatch");
    K::Check(P, C);
    WritePropertyScalarValue(P, F.Memory, K::FromDas(X, C), C);
    return Mutation.Finish(C, At);
}

// Struct snapshot maps with scalar keys and values.
template <class KK, class VK>
typename VK::Out StructMapGetScalar(const FDasStructValue& H, const char* Path, typename KK::In Key, das::Context* C)
{
    return VK::ToDas(StructScalarMapGet<typename VK::Property>(H, Path, KK::FromDas(Key, C), KK::SnapshotKind, C), C);
}
template <class KK, class VK>
FDasStructValue StructMapWithScalar(const FDasStructValue& H, const char* Path, typename KK::In Key,
                                    typename VK::In Value, das::Context* C, das::LineInfoArg* At)
{
    return StructScalarMapWith<typename VK::Property>(H, Path, KK::FromDas(Key, C), VK::FromDas(Value, C),
                                                      KK::SnapshotKind, C, At);
}

// Scalar members of self struct properties, read and written in place. The former lowering copied the
// whole struct into a snapshot, changed one leaf and wrote the snapshot back.
FStructProperty* SelfStructProperty(UObject* Self, const char* Property, bool bWrite, das::Context* C)
{
    auto* P = CastField<FStructProperty>(ResolveProperty<FProperty>(Self, Property, bWrite, C));
    if (!P)
        C->throw_error("self property is not a struct");
    return P;
}
template <class K> typename K::Out GetSelfStructMember(const char* Property, const char* Member, das::Context* C)
{
    UObject* Self = ResolveSelf(C);
    auto* S = SelfStructProperty(Self, Property, false, C);
    auto F = StructLeafMemory(S->Struct.Get(), S->ContainerPtrToValuePtr<const uint8>(Self), Member, C);
    auto* P = CastField<typename K::Property>(F.Property);
    if (!P)
        C->throw_error("struct field scalar type mismatch");
    K::Check(P, C);
    return K::ToDas(ReadPropertyScalarValue(P, F.Memory, C), C);
}
template <class K>
void SetSelfStructMember(const char* Property, const char* Member, typename K::In Value, das::Context* C,
                         das::LineInfoArg* At)
{
    if (ActiveContext && ActiveContext->bCollectingSubscriptions)
        C->throw_error("self writes are forbidden in bind_events");
    const auto Native = K::FromDas(Value, C);
    if (!DasFiniteScalar(Native))
    {
        // Non-finite numbers take the snapshot path, which validates math members and reports the error.
        SetSelfStruct(Property, StructWithScalar<K>(GetSelfStruct(Property, C, At), Member, Value, C, At), C);
        return;
    }
    UObject* Self = ResolveSelf(C);
    auto* S = SelfStructProperty(Self, Property, true, C);
    auto F = StructLeafMemory(S->Struct.Get(), S->ContainerPtrToValuePtr<uint8>(Self), Member, C);
    auto* P = CastField<typename K::Property>(F.Property);
    if (!P)
        C->throw_error("struct field scalar type mismatch");
    K::Check(P, C);
    WritePropertyScalarValue(P, F.Memory, Native, C);
}

// Timer payloads: one scalar argument delivered to the handler.
void ScriptTimerStartImpl(const char* Name, float Seconds, const char* Handler, bool Repeat, das::Context* C,
                          FDasScalarValue* Argument, bool NextTick);
template <class K> FDasScalarValue DasTimerPayload(typename K::In Value, das::Context* C)
{
    using FSlot = TPropertyScalarSlot<typename K::Property>;
    FDasScalarValue V;
    V.Kind = int32(FSlot::Kind);
    V.*FSlot::Member = K::FromDas(Value, C);
    return V;
}
template <class K>
void ScriptTimerStartKind(const char* N, float Seconds, const char* H, bool Repeat, typename K::In Value,
                          das::Context* C)
{
    auto V = DasTimerPayload<K>(Value, C);
    ScriptTimerStartImpl(N, Seconds, H, Repeat, C, &V, false);
}
template <class K> void ScriptTimerNextTickKind(const char* N, const char* H, typename K::In Value, das::Context* C)
{
    auto V = DasTimerPayload<K>(Value, C);
    ScriptTimerStartImpl(N, 0.f, H, false, C, &V, true);
}

// Reflected call frames: named arguments and results.
template <class K> void SetCallKind(const FDasCallHandle& H, const char* Path, typename K::In V, das::Context* C)
{
    SetCallScalar<typename K::Property>(H, Path, K::FromDas(V, C), C);
}
template <class K> typename K::Out GetCallKind(const FDasCallHandle& H, const char* Path, das::Context* C)
{
    return K::ToDas(GetCallScalar<typename K::Property>(H, Path, C), C);
}
// Raw byte for call frames: range-checked int, no enum restriction (enum bytes use ue_arg_enum).
struct FDasKindCallByte : FDasKindByteInt
{
    static void Check(Property*, das::Context*) {}
    static uint8 FromDas(int32 V, das::Context* C)
    {
        if (V < 0 || V > 255)
            C->throw_error("byte input must be a raw uint8 in range 0..255");
        return uint8(V);
    }
};

// (argument extern, result extern, kind)
#define DAS_CALL_SCALAR_KINDS(X)                                                                                       \
    X("ue_arg_int", "ue_result_int", FDasKindInt)                                                                      \
    X("ue_arg_float", "ue_result_float", FDasKindFloat)                                                                \
    X("ue_arg_double", "ue_result_double", FDasKindDouble)                                                             \
    X("ue_arg_bool", "ue_result_bool", FDasKindBool)                                                                   \
    X("ue_arg_string", "ue_result_string", FDasKindString)                                                             \
    X("ue_arg_byte", "ue_result_byte", FDasKindCallByte)                                                               \
    X("__ue_method_arg_name", "__ue_method_result_name", FDasKindName)                                                 \
    X("__ue_method_arg_text", "__ue_method_result_text", FDasKindText)

// (daScript suffix, kind): self, authored and native accessors only (no struct leaves).
#define DAS_REFERENCE_ACCESSOR_KINDS(X)                                                                                \
    X(enum, FDasKindEnum, FDasKindEnum)                                                                                \
    X(class, FDasKindClass, FDasKindClass)

// (daScript suffix, self and struct kind, reference kind)
#define DAS_SCALAR_ACCESSOR_KINDS(X)                                                                                   \
    X(int, FDasKindInt, FDasKindInt)                                                                                   \
    X(float, FDasKindFloat, FDasKindFloat)                                                                             \
    X(double, FDasKindDouble, FDasKindDouble)                                                                          \
    X(bool, FDasKindBool, FDasKindBool)                                                                                \
    X(string, FDasKindString, FDasKindString)                                                                          \
    X(name, FDasKindName, FDasKindName)                                                                                \
    X(byte, FDasKindByte, FDasKindByteInt)                                                                             \
    X(text, FDasKindText, FDasKindText)

// DSL lowering looks kinds up by declared type name ("int", "text", ...).
// Suffix of a scalar kind with struct-leaf accessors, or nullptr.
inline const char* DasScalarKindSuffix(const FString& Kind)
{
#define DAS_SCALAR_KIND_SUFFIX(Suffix, SelfKind, RefKind)                                                              \
    if (Kind == TEXT(#Suffix))                                                                                         \
        return #Suffix;
    DAS_SCALAR_ACCESSOR_KINDS(DAS_SCALAR_KIND_SUFFIX)
#undef DAS_SCALAR_KIND_SUFFIX
    return nullptr;
}
// Suffix of any kind with self/authored/native accessors (scalars, enum, class), or nullptr.
inline const char* DasAccessorKindSuffix(const FString& Kind)
{
    if (const char* Suffix = DasScalarKindSuffix(Kind))
        return Suffix;
#define DAS_REFERENCE_KIND_SUFFIX(Suffix, SelfKind, RefKind)                                                           \
    if (Kind == TEXT(#Suffix))                                                                                         \
        return #Suffix;
    DAS_REFERENCE_ACCESSOR_KINDS(DAS_REFERENCE_KIND_SUFFIX)
#undef DAS_REFERENCE_KIND_SUFFIX
    return nullptr;
}
// Reflected property class of a kind as named in generator inputs ("IntProperty", ...).
template <class K> FString DasKindPropertyName()
{
    if constexpr (requires { K::PropertyName; })
        return K::PropertyName;
    else
        return K::Property::StaticClass()->GetName();
}
