#include "DasTypeContracts.h"
#include "DasContainerTypes.h"
#include "DasNativeDefaults.h"
#include "DasCallbackTypes.h"
#include "DasDelegateSignatures.h"
#include "UObject/StrongObjectPtr.h"
#include "DasValueKinds.h"
#include "Misc/SecureHash.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/UnrealType.h"

namespace
{
bool Fail(FString& Error, const FString& Message)
{
    Error = Message;
    return false;
}
bool Identifier(const FString& Text)
{
    if (Text.IsEmpty() || Text.Len() >= NAME_SIZE || FChar::IsDigit(Text[0]) ||
        Text.Equals(TEXT("None"), ESearchCase::IgnoreCase))
        return false;
    for (TCHAR Ch : Text)
        if (!FChar::IsAlnum(Ch) && Ch != '_')
            return false;
    return true;
}
bool StableId(const FString& Text)
{
    if (Text.IsEmpty() || Text.Len() > 256)
        return false;
    for (TCHAR Ch : Text)
        if (!((Ch >= 'a' && Ch <= 'z') || (Ch >= 'A' && Ch <= 'Z') || (Ch >= '0' && Ch <= '9') || Ch == '_' ||
              Ch == '.' || Ch == '-'))
            return false;
    return true;
}
bool NativePath(const FString& Text)
{
    if (!Text.StartsWith(TEXT("/")) || !Text.Contains(TEXT(".")) || Text.Len() >= NAME_SIZE)
        return false;
    for (TCHAR Ch : Text)
        if (!FChar::IsAlnum(Ch) && Ch != '_' && Ch != '/' && Ch != '.')
            return false;
    return true;
}
bool DeclarationType(const FDasDeclaredType& Type, const TSet<FString>& Classes, bool bVoid, FString& Error,
                     int32 Depth)
{
    if (Depth > 16)
        return Fail(Error, TEXT("Declaration type nesting limit exceeded"));
    const FString& K = Type.Kind;
    if (!Type.AuthoredId.IsEmpty() && (K != TEXT("object") || !StableId(Type.AuthoredId)))
        return Fail(Error, TEXT("Invalid authored object identity"));
    if (K == TEXT("array") || K == TEXT("set"))
    {
        if (!Type.Element || Type.Key || Type.Value || !Type.TypePath.IsEmpty())
            return Fail(Error, TEXT("Invalid element type shape"));
        return DeclarationType(*Type.Element, Classes, false, Error, Depth + 1);
    }
    if (K == TEXT("map"))
    {
        if (!Type.Key || !Type.Value || Type.Element || !Type.TypePath.IsEmpty())
            return Fail(Error, TEXT("Invalid map type shape"));
        if (Type.Key->Kind !=
            TEXT("byte") && Type.Key->Kind !=
                                TEXT("int") && Type.Key->Kind !=
                                                   TEXT("float") &&
                                                        Type.Key->Kind !=
                                                            TEXT("double") &&
                                                                 Type.Key->Kind !=
                                                                     TEXT("bool") &&
                                                                          Type.Key->Kind !=
                                                                              TEXT("string") &&
                                                                                   Type.Key->Kind !=
                                                                                       TEXT("name") &&
                                                                                            Type.Key->Kind !=
                                                                                                TEXT("enum") &&
                                                                                                     Type.Key->Kind !=
                                                                                                         TEXT("object"))
            return Fail(Error, TEXT("Map key requires a supported scalar identity"));
        return DeclarationType(*Type.Key, Classes, false, Error, Depth + 1) &&
               DeclarationType(*Type.Value, Classes, false, Error, Depth + 1);
    }
    if (!Type.DelegateWire.IsEmpty() &&
        (K != TEXT("delegate") || Type.DelegateWire.Len() > 131072 ||
                   Type.TypePath != TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*Type.DelegateWire) + TEXT(".Type")))
        return Fail(Error, TEXT("Delegate type/signature identity mismatch"));
    if (Type.Element || Type.Key || Type.Value)
        return Fail(Error, TEXT("Scalar/reference type has container members"));
    if (K == TEXT("object") || K == TEXT("enum") || K == TEXT("struct") ||
                                                              (K == TEXT("event") || K == TEXT("delegate")) ||
                                                               K == TEXT("class"))
        return (NativePath(Type.TypePath) || (K == TEXT("object") && Classes.Contains(Type.TypePath))) ||
                Fail(Error, TEXT("Missing type identity: ") + Type.TypePath);
    if (!Type.TypePath.IsEmpty())
        return Fail(Error, TEXT("Primitive type has a type_path"));
    if (K == TEXT("byte") ||
                  K == TEXT("text") ||
                            K == TEXT("int") ||
                                      K == TEXT("float") ||
                                                K == TEXT("double") ||
                                                          K == TEXT("bool") ||
                                                                    K == TEXT("string") ||
                                                                              K == TEXT("name") ||
                                                                                        (bVoid && K == TEXT("void")))
        return true;
    return Fail(Error, TEXT("Unsupported declaration type: ") + K);
}
bool PropertyDefault(const FDasDeclaredType& Type, const FDasDeclaredLiteral& D, FString& Error)
{
    if (auto* Carrier = DasContainerCarrier(Type))
    {
        if (D.Kind.IsEmpty())
            return true;
        FInstancedStruct Value;
        return (D.Kind == TEXT("string") && ParseDasStructDefault(Carrier->GetPathName(), D.String, Value, Error)) ||
                Fail(Error, TEXT("Invalid container default: ")+ Error);
    }
    if (D.Kind.IsEmpty())
        return true;
    const auto& K = Type.Kind;
    if (K == TEXT("text"))
    {
        FText Value;
        return D.Kind == TEXT("string") && ParseDasTextDefault(D.String, Value, Error);
    }
    if (K == TEXT("byte"))
        return D.Kind == TEXT("number") && FMath::IsFinite(D.Number) && D.Number >= 0 && D.Number <= 255 &&
                              FMath::FloorToDouble(D.Number) == D.Number;
    if (D.Kind == TEXT("number") && FMath::IsFinite(D.Number))
    {
        if (K == TEXT("int") && D.Number >= MIN_int32 && D.Number <= MAX_int32 &&
                      FMath::FloorToDouble(D.Number) == D.Number)
            return true;
        if (K == TEXT("double"))
            return true;
        if (K == TEXT("float") && FMath::IsFinite(static_cast<float>(D.Number)) &&
                      (D.Number == 0 || static_cast<float>(D.Number) != 0))
            return true;
    }
    if (K == TEXT("bool") && D.Kind == TEXT("bool"))
        return true;
    if ((K == TEXT("string") || K == TEXT("name") || K == TEXT("enum")) &&
                                          D.Kind == TEXT("string") &&
                                                         (K != TEXT("name") || D.String.Len() < NAME_SIZE) &&
                                                          (K != TEXT("enum") || Identifier(D.String)))
        return true;
    if (K == TEXT("struct") && D.Kind == TEXT("string"))
    {
        FInstancedStruct Value;
        return ParseDasStructDefault(Type.TypePath, D.String, Value, Error);
    }
    if ((K == TEXT("object") || K == TEXT("class")) && D.Kind == TEXT("null"))
        return true;
    if ((K == TEXT("object") || K == TEXT("class")) && D.Kind == TEXT("string") && !D.String.IsEmpty())
    {
        if (!Type.AuthoredId.IsEmpty())
            return Fail(Error, TEXT("Non-null authored object defaults require a separate identity contract"));
        UObject* Value = nullptr;
        return ResolveDasReferenceDefault(K == TEXT("class"), Type.TypePath, D.String, Value, Error);
    }
    return Fail(Error, TEXT("Invalid authored property default"));
}

FDasTypeUseResult Decision(bool Good, const FString& Why)
{
    if (Good)
        return {EDasTypeSupport::Supported, {}};
    return {EDasTypeSupport::Unsupported, Why.IsEmpty() ? TEXT("Unsupported type in this context") : Why};
}
bool ScalarDefault(EDasTypeKind K, bool Parameter)
{
    return K == EDasTypeKind::Int || K == EDasTypeKind::Float || K == EDasTypeKind::Double || K == EDasTypeKind::Bool ||
           K == EDasTypeKind::String || (Parameter && (K == EDasTypeKind::Byte || K == EDasTypeKind::Text));
}
}
TConstArrayView<FDasTypeUseInfo> DasTypeUses()
{
    static const FDasTypeUseInfo Uses[] = {
        {EDasTypeUse::AuthoredInput, TEXT("authored_in"), TEXT("authored method lowering ABI")},
         {EDasTypeUse::AuthoredOut, TEXT("authored_out"), TEXT("authored method lowering ABI")},
          {EDasTypeUse::AuthoredRef, TEXT("authored_ref"), TEXT("authored method lowering ABI")},
           {EDasTypeUse::AuthoredReturn, TEXT("authored_return"), TEXT("authored method lowering ABI")},
            {EDasTypeUse::InterfaceInput,
             TEXT("interface_in"), TEXT("interface method lowering ABI; nominal signature matching remains required")},
             {EDasTypeUse::InterfaceOut,
              TEXT("interface_out"),
                   TEXT("interface method lowering ABI; nominal signature matching remains required")},
              {EDasTypeUse::InterfaceRef,
               TEXT("interface_ref"),
                    TEXT("interface method lowering ABI; nominal signature matching remains required")},
               {EDasTypeUse::InterfaceReturn,
                TEXT("interface_return"),
                     TEXT("interface method lowering ABI; nominal signature matching remains required")},
                {EDasTypeUse::DelegateInput, TEXT("delegate_in"), TEXT("delegate reflected signature factory")},
                 {EDasTypeUse::DelegateOut, TEXT("delegate_out"), TEXT("delegate reflected signature factory")},
                  {EDasTypeUse::DelegateRef, TEXT("delegate_ref"), TEXT("delegate reflected signature factory")},
                   {EDasTypeUse::DelegateReturn, TEXT("delegate_return"), TEXT("delegate reflected signature factory")},
                    {EDasTypeUse::MulticastInput,
                     TEXT("multicast_in"), TEXT("multicast signature factory; no return or out-only parameters")},
                     {EDasTypeUse::MulticastRef,
                      TEXT("multicast_ref"),
                           TEXT("synchronous multicast signature; queued subscribers cannot mutate refs")},
                      {EDasTypeUse::QueuedEvent,
                       TEXT("queued_event"),
                           TEXT("owned queued payload; native receiver/signature and delivery value checks remain required")},
                       {EDasTypeUse::Timer,
                        TEXT("timer"),
                             TEXT("timer payload; scheduling, direction and delivery value checks remain required")},
                        {EDasTypeUse::PropertyDefault,
                         TEXT("property_default"),
                              TEXT("explicit property initializer type gate; literal/path validation follows")},
                         {EDasTypeUse::StructMemberDefault,
                          TEXT("struct_member_default"),
                               TEXT("struct member type gate; implicit/explicit and symbol-table checks follow")},
                          {EDasTypeUse::ParameterDefault,
                           TEXT("parameter_default"),
                                TEXT("scalar input literal; ordering/world-context and literal validation follow")},
                           {EDasTypeUse::ComputedDefaultLeaf,
                            TEXT("computed_default_leaf"),
                                TEXT("scalar evaluator leaf; constructors/containers compose leaves; expression audit follows")},
                            {EDasTypeUse::SynchronousCallback,
                             TEXT("synchronous_callback"),
                                 TEXT("specialized synchronous callback role; concrete frame and lifetime scope required")},
                             {EDasTypeUse::NativeCall,
                              TEXT("native_call"),
                                  TEXT("registered native transport; exact FProperty/function/direction/depth required")},
                             };
    return MakeArrayView(Uses);
}
EDasMethodSignature DasAuthoredTypeSignature(const FDasDeclaredType& T)
{
    const auto K = T.GetKind();
    if (K == EDasTypeKind::Array || K == EDasTypeKind::Set || K == EDasTypeKind::Map)
        return DasContainerCarrier(T) ? EDasMethodSignature::Struct : EDasMethodSignature::Frame;
    if (K == EDasTypeKind::Struct)
    {
        const auto* A = FDasTypeAdapters::Get().FindStructPath(T.TypePath);
        return A ? A->GetSignature().Get(EDasMethodSignature::Struct) : EDasMethodSignature::Struct;
    }
    return DasSignatureKind(K).Get(EDasMethodSignature::Frame);
}
FDasTypeUseResult QueryDasTypeUse(const FDasDeclaredType& T, EDasTypeUse Use)
{
    FString Error;
    const auto K = T.GetKind();
    switch (Use)
    {
    case EDasTypeUse::AuthoredInput:
    case EDasTypeUse::AuthoredOut:
    case EDasTypeUse::AuthoredRef:
    case EDasTypeUse::AuthoredReturn:
    case EDasTypeUse::InterfaceInput:
    case EDasTypeUse::InterfaceOut:
    case EDasTypeUse::InterfaceRef:
    case EDasTypeUse::InterfaceReturn:
    {
        if (!ValidateDasDeclarationType(
                T, {}, Use == EDasTypeUse::AuthoredReturn || Use == EDasTypeUse::InterfaceReturn, Error))
            return Decision(false, Error);
        const auto S = DasAuthoredTypeSignature(T);
        if (K == EDasTypeKind::Object)
        {
            auto* C = FindObject<UClass>(nullptr, *T.TypePath);
            if (!C || (!T.TypePath.StartsWith(TEXT("/Script/")) && !C->HasAnyClassFlags(CLASS_Interface)))
                return Decision(false, TEXT("Object signature requires loaded native class or interface"));
        }
        if (K == EDasTypeKind::Class)
        {
            auto* C = FindObject<UClass>(nullptr, *T.TypePath);
            if (!T.TypePath.StartsWith(
                    TEXT("/Script/")) || !IsValid(C) ||
                    C->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists))
                return Decision(false, TEXT("Class signature requires a loaded native base class"));
        }
        if (S == EDasMethodSignature::Struct)
        {
            const auto* Carrier = DasContainerCarrier(T);
            const FString Path = Carrier ? Carrier->GetPathName() : T.TypePath;
            if (!ValidateDasValueStruct(Path, Error))
                return Decision(false, Error);
        }
        if (K == EDasTypeKind::Enum && !ResolveDasEnum(T.TypePath, Error))
            return Decision(false, Error);
        if (K == EDasTypeKind::Delegate)
        {
            if (Use == EDasTypeUse::AuthoredOut || Use == EDasTypeUse::AuthoredRef ||
                Use == EDasTypeUse::InterfaceOut || Use == EDasTypeUse::InterfaceRef)
                return Decision(false, TEXT("Delegate-valued ref/out parameters are unsupported"));
            TStrongObjectPtr<UFunction> Signature(MakeDasSingleDelegateSignature(T.DelegateWire, Error));
            if (!Signature)
                return Decision(false, Error);
        }
        return Decision(S != EDasMethodSignature::Frame &&
                            (S != EDasMethodSignature::Void || Use == EDasTypeUse::AuthoredReturn ||
                             Use == EDasTypeUse::InterfaceReturn),
                        TEXT("No authored lowering ABI for this type/use"));
    }
    case EDasTypeUse::DelegateInput:
    case EDasTypeUse::DelegateOut:
    case EDasTypeUse::DelegateRef:
    case EDasTypeUse::DelegateReturn:
    case EDasTypeUse::MulticastInput:
    case EDasTypeUse::MulticastRef:
        if (K == EDasTypeKind::Void && Use == EDasTypeUse::DelegateReturn)
            return Decision(ValidateDasDeclarationType(T, {}, true, Error), Error);
        return Decision(ValidateDasPropertyType(T, EDasPropertyUsage::DelegateParameter, Error), Error);
    case EDasTypeUse::QueuedEvent:
    case EDasTypeUse::Timer:
        return Decision(ValidateDasCallbackType(
                            T, Use == EDasTypeUse::Timer ? EDasCallbackUsage::Timer : EDasCallbackUsage::Event, Error),
                        Error);
    case EDasTypeUse::ParameterDefault:
    case EDasTypeUse::ComputedDefaultLeaf:
        if (!ValidateDasDeclarationType(T, {}, false, Error))
            return Decision(false, Error);
        return Decision(ScalarDefault(K, Use == EDasTypeUse::ParameterDefault),
                        Use == EDasTypeUse::ParameterDefault ? TEXT("Parameter defaults require scalar input literals")
                                                             : TEXT("Computed evaluator requires a scalar leaf"));
    case EDasTypeUse::PropertyDefault:
    case EDasTypeUse::StructMemberDefault:
    {
        TSet<FString> ClassReferences;
        if (K == EDasTypeKind::Object && StableId(T.TypePath))
            ClassReferences.Add(T.TypePath);
        if (!ValidateDasDeclarationType(T, ClassReferences, false, Error))
            return Decision(false, Error);
        if (K == EDasTypeKind::Event || K == EDasTypeKind::Delegate)
            return Decision(false, TEXT("Delegates/events use implicit empty initialization"));
        const auto* A = FDasTypeAdapters::Get().Find(T);
        if (const auto* N = A ? A->GetNativePolicy() : nullptr; N && N->StorageError)
            return Decision(false, N->StorageError);
        if (Use == EDasTypeUse::StructMemberDefault)
        {
            auto Member = [](EDasTypeKind Kind)
            {
                return ScalarDefault(Kind, true) || Kind == EDasTypeKind::Name || Kind == EDasTypeKind::Object ||
                       Kind == EDasTypeKind::Enum || Kind == EDasTypeKind::Struct;
            };
            if (!Member(K) && K != EDasTypeKind::Array && K != EDasTypeKind::Set && K != EDasTypeKind::Map)
                return Decision(false, TEXT("Unsupported authored struct member type"));
            if (K == EDasTypeKind::Array && (!T.Element || !Member(T.Element->GetKind())))
                return Decision(false, TEXT("Unsupported authored array element"));
            if (K == EDasTypeKind::Map && (!T.Value || !Member(T.Value->GetKind())))
                return Decision(false, TEXT("Unsupported authored map value"));
            if (K == EDasTypeKind::Set &&
                (!T.Element || (T.Element->Kind !=
                                TEXT("byte") && T.Element->Kind != TEXT("int") && T.Element->Kind != TEXT("string"))))
                return Decision(false, TEXT("Authored sets require int/string elements and empty defaults"));
            if (K == EDasTypeKind::Map &&
                (!T.Key || !T.Value ||
                 (T.Key->Kind != TEXT("byte") && T.Key->Kind != TEXT("int") && T.Key->Kind != TEXT("string"))))
                return Decision(false, TEXT("Authored maps require int/string keys and empty defaults"));
        }
        return Decision(true, {});
    }
    case EDasTypeUse::SynchronousCallback:
    {
        const auto* A = FDasTypeAdapters::Get().Find(T);
        const auto* N = A ? A->GetNativePolicy() : nullptr;
        if (!N || !EnumHasAnyFlags(N->Roles, EDasNativeTypeRole::Geometry | EDasNativeTypeRole::InputEvent |
                                                 EDasNativeTypeRole::Paint | EDasNativeTypeRole::EventReply |
                                                 EDasNativeTypeRole::InputActionValue))
            return Decision(false,
                            TEXT("No specialized synchronous callback transport; ordinary signatures are separate"));
        return {EDasTypeSupport::RequiresContext,
                TEXT("Concrete callback role, complete frame and transport lifetime scope required")};
    }
    case EDasTypeUse::NativeCall:
    {
        const auto* A = FDasTypeAdapters::Get().Find(T);
        const auto* N = A ? A->GetNativePolicy() : nullptr;
        if (!N || !N->AllowCall)
            return Decision(false, TEXT("No registered native transport gate; generic API validation is separate"));
        if (EnumHasAnyFlags(N->Roles, EDasNativeTypeRole::Rejected))
            return Decision(false, UTF8_TO_TCHAR(N->CallError));
        return {EDasTypeSupport::RequiresContext,
                N->CallError ? UTF8_TO_TCHAR(N->CallError) : TEXT("Concrete native property required")};
    }
    }
    return Decision(false, TEXT("Unknown type use"));
}
bool ValidateDasTypeUse(const FDasDeclaredType& Type, EDasTypeUse Use, FString& Error)
{
    auto Result = QueryDasTypeUse(Type, Use);
    Error = MoveTemp(Result.Reason);
    return Result.IsSupported();
}
bool ValidateDasDeclarationType(const FDasDeclaredType& T, const TSet<FString>& Classes, bool Void, FString& Error)
{
    Error.Reset();
    return DeclarationType(T, Classes, Void, Error, 0);
}
bool ValidateDasSignatureContract(const FDasDeclaredMethod& M, EDasSignatureUse Use, FString& Error)
{
    Error.Reset();
    const bool Multi = Use == EDasSignatureUse::Multicast;
    if (M.Parameters.Num() > 16 || (Multi && M.ReturnType.GetKind() != EDasTypeKind::Void))
        return Fail(Error, TEXT("Signatures support up to 16 inputs; multicast events cannot return a value"));
    // Delegate factories also project existing methods for signature matching. Defaults are source
    // declaration metadata, not part of that projection; delegate DSL parsers reject them separately.
    if ((Use == EDasSignatureUse::Method || Use == EDasSignatureUse::Interface) &&
        !ValidateDasParameterDefaults(M, Use == EDasSignatureUse::Method, Error))
        return false;
    const bool Delegate = Use == EDasSignatureUse::Delegate || Multi;
    auto Context = [&](const FString& Direction)
    {
        if (Multi)
            return Direction == TEXT("in") ? EDasTypeUse::MulticastInput : EDasTypeUse::MulticastRef;
        if (Delegate)
            return Direction == TEXT("in")
                                     ? EDasTypeUse::DelegateInput
                                     : Direction == TEXT("out") ? EDasTypeUse::DelegateOut : EDasTypeUse::DelegateRef;
        if (Use == EDasSignatureUse::Interface)
            return Direction == TEXT("in")
                                     ? EDasTypeUse::InterfaceInput
                                     : Direction == TEXT("out") ? EDasTypeUse::InterfaceOut : EDasTypeUse::InterfaceRef;
        return Direction == TEXT("in") ? EDasTypeUse::AuthoredInput
                                       : Direction == TEXT("out") ? EDasTypeUse::AuthoredOut : EDasTypeUse::AuthoredRef;
    };
    if (!Multi && !ValidateDasTypeUse(M.ReturnType,
                                      Delegate                             ? EDasTypeUse::DelegateReturn
                                      : Use == EDasSignatureUse::Interface ? EDasTypeUse::InterfaceReturn
                                                                           : EDasTypeUse::AuthoredReturn,
                                      Error))
        return false;
    for (const auto& P : M.Parameters)
    {
        if ((P.Direction != TEXT("in") && P.Direction != TEXT("out") && P.Direction != TEXT("inout")) ||
                                              (Multi && P.Direction == TEXT("out")))
            return Fail(Error, TEXT("Invalid signature direction; multicast supports in/ref, not out-only parameters"));
        if (!ValidateDasTypeUse(P.Type, Context(P.Direction), Error))
            return false;
    }
    return true;
}
bool ValidateDasAuthoredDefault(const FDasDeclaredType& T, const FDasDeclaredLiteral& V, EDasTypeUse Use,
                                FString& Error)
{
    Error.Reset();
    if (Use != EDasTypeUse::PropertyDefault && Use != EDasTypeUse::StructMemberDefault &&
        Use != EDasTypeUse::ParameterDefault)
        return Fail(Error, TEXT("Expected literal default context"));
    if (Use == EDasTypeUse::StructMemberDefault)
    {
        if (!ValidateDasTypeUse(T, Use, Error))
            return false;
        const auto K = T.GetKind();
        if (K == EDasTypeKind::Array || K == EDasTypeKind::Set || K == EDasTypeKind::Map)
            return V.Kind.IsEmpty() || Fail(Error, TEXT("Authored struct containers require empty defaults"));
        if (K == EDasTypeKind::Struct)
        {
            const auto* Leaf = DasLeafValueAdapter(T.TypePath);
            if ((Leaf && Leaf->GetDefaultPolicy() == EDasAdapterDefault::ImplicitEmpty) ||
                (!Leaf && !DasMathAdapter(T.TypePath)))
                return V.Kind.IsEmpty() || Fail(Error, TEXT("This struct member uses implicit child/empty defaults"));
            if (DasMathAdapter(T.TypePath) && V.Kind != TEXT("string"))
                return Fail(Error, TEXT("Math struct members require literal defaults"));
        }
    }
    if (V.Kind.IsEmpty())
        return true;
    if (!ValidateDasTypeUse(T, Use, Error))
        return false;
    if (Use == EDasTypeUse::ParameterDefault && V.Kind == TEXT("string"))
    {
        if (V.String.Len() > 4096)
            return Fail(Error, TEXT("Parameter string default exceeds supported limits"));
        for (int32 I = 0; I < V.String.Len(); ++I)
            if (V.String[I] == 0)
                return Fail(Error, TEXT("Parameter string default contains NUL"));
    }
    const bool Good = PropertyDefault(T, V, Error);
    if (!Good && Error.IsEmpty())
        Error = TEXT("Invalid authored default for ") + T.Kind;
    return Good;
}
bool ValidateDasNativeCallContract(const FProperty* P, bool Input, bool Output, int32 Depth, FString& Error)
{
    Error.Reset();
    const auto* S = CastField<FStructProperty>(P);
    const auto* N = S ? DasNativeTypePolicy(S->Struct) : nullptr;
    if (!N || !N->AllowCall)
        return Fail(Error, TEXT("No registered native transport gate"));
    return N->AllowCall(P, Input, Output, Depth) ||
           Fail(Error, N->CallError ? UTF8_TO_TCHAR(N->CallError) : TEXT("Native call context rejected"));
}

bool ValidateDasLoweredSignatureType(EDasMethodSignature Kind, const FString& Path, EDasTypeUse Use, FString& Error)
{
    FDasDeclaredType T;
    T.Kind = DasTypeKindName(DasTypeKindOf(Kind));
    T.TypePath = Path;
    if (const auto* A = FDasTypeAdapters::Get().FindSignature(Kind); A && A->GetStruct())
        T.TypePath = A->GetStruct()->GetPathName();
    if (Kind == EDasMethodSignature::Delegate)
    {
        T.DelegateWire = Path;
        T.TypePath = TEXT("/DasDelegates/") + FMD5::HashAnsiString(*Path) + TEXT(".Type");
    }
    return ValidateDasTypeUse(T, Use, Error);
}

bool DasSynchronousCallbackRole(const FString& Path, EDasNativeTypeRole Role)
{
    FDasDeclaredType T;
    T.Kind = TEXT("struct");
    T.TypePath = Path;
    return QueryDasTypeUse(T, EDasTypeUse::SynchronousCallback).State == EDasTypeSupport::RequiresContext &&
           DasHasNativeRolePath(Path, Role);
}

bool ValidateDasParameterDefaults(const FDasDeclaredMethod& M, bool AllowDefaults, FString& Error)
{
    Error.Reset();
    bool SawDefault = false;
    for (const auto& P : M.Parameters)
    {
        if (!P.Default.Kind.IsEmpty())
        {
            if (!AllowDefaults)
                return Fail(Error, TEXT("This signature surface does not support parameter defaults"));
            if (P.Direction != TEXT("in") || P.Name == M.WorldContext)
                return Fail(Error, TEXT("Parameter default requires a scalar input: ") + P.Name);
            if (!ValidateDasAuthoredDefault(P.Type, P.Default, EDasTypeUse::ParameterDefault, Error))
                return false;
            SawDefault = true;
        }
        else if (SawDefault)
            return Fail(Error, TEXT("Required parameter follows a default parameter: ") + P.Name);
    }
    return true;
}
