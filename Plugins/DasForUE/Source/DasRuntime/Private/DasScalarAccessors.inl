// Part of DasRuntime.cpp: scalar property accessors shared by self, authored-object and native-reference access.
// Included in place; declaration order matters.
//
// One kind row describes how a daScript scalar maps to an Unreal property: the
// property class, the daScript argument/result types and the conversions. The
// accessor families below are templates over a kind; DAS_SCALAR_ACCESSOR_KINDS
// registers every (family, kind) pair, so a new scalar kind is one row here.

struct FDasKindInt
{
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
    if (FString(Path) == TEXT("/Script/Engine.CharacterMovementComponent") &&
                              (FString(Name) == TEXT("MaxWalkSpeed") || FString(Name) == TEXT("JumpZVelocity")) &&
                                                     V < 0)
        C->throw_error("movement speed/jump velocity must be nonnegative");
}

// self: properties of the running script owner.
template <class K> typename K::Out GetSelfScalar(const char* Name, das::Context* C)
{
    UObject* Self = ResolveSelf(C);
    auto* P = ResolveProperty<typename K::Property>(Self, Name, false, C);
    K::Check(P, C);
    return K::ToDas(ReadPropertyScalar(P, Self, C), C);
}
template <class K> void SetSelfScalar(const char* Name, typename K::In Value, das::Context* C)
{
    if (ActiveContext && ActiveContext->bCollectingSubscriptions)
        C->throw_error("self writes are forbidden in bind_events");
    UObject* Self = ResolveSelf(C);
    auto* P = ResolveProperty<typename K::Property>(Self, Name, true, C);
    K::Check(P, C);
    WritePropertyScalar(P, Self, K::FromDas(Value, C), C);
}

// Authored object references: properties of DSL-declared classes by stable type id.
template <class K>
typename K::Out AuthoredGetScalar(const FDasObjectHandle& H, const char* Id, const char* Name, das::Context* C)
{
    auto* O = ResolveAuthoredObject(H, Id, C);
    auto* P = ResolveProperty<typename K::Property>(O, Name, false, C);
    K::Check(P, C);
    return K::ToDas(ReadPropertyScalar(P, O, C), C);
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
    WritePropertyScalar(P, O, K::FromDas(Value, C), C);
}

// Generated native references: properties listed in the native gameplay registry.
template <class K>
typename K::Out NativeReferenceGetScalar(const FDasObjectHandle& H, const char* Path, const char* Name, das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, false, C);
    auto* P = CastFieldChecked<typename K::Property>(NativeGameplayProperty(O, Path, Name, false, K::Name, C));
    K::Check(P, C);
    return K::ToDas(ReadPropertyScalar(P, O, C), C);
}
template <class K>
void NativeReferenceSetScalar(const FDasObjectHandle& H, const char* Path, const char* Name, typename K::In Value,
                              das::Context* C)
{
    auto* O = NativeGameplayObject(H, Path, true, C);
    auto* P = CastFieldChecked<typename K::Property>(NativeGameplayProperty(O, Path, Name, true, K::Name, C));
    K::Check(P, C);
    const auto Native = K::FromDas(Value, C);
    DasNativeScalarWritePolicy(Path, Name, Native, C);
    WritePropertyScalar(P, O, Native, C);
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
