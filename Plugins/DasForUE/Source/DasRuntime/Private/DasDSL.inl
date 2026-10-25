const char* DSLModelKey = "_ue_model_json";
FString DSLText(const das::string& S)
{
    return UTF8_TO_TCHAR(S.c_str());
}
das::string DSLUtf8(const FString& S)
{
    return TCHAR_TO_UTF8(*S);
}
void DSLFail(const FString& S)
{
    throw std::runtime_error(TCHAR_TO_UTF8(*S));
}
FString DSLString(const das::AnnotationArgumentList& Args, const char* Key, const FString& Default = FString())
{
    if (const auto* A = Args.find(Key, das::Type::tString))
        return DSLText(A->sValue);
    for (const auto& A : Args)
        if (A.name == Key)
            DSLFail(TEXT("Expected string annotation: ")+ DSLText(A.name));
    return Default;
}
bool DSLBool(const das::AnnotationArgument& A)
{
    if (A.type != das::Type::tBool)
        DSLFail(TEXT("Expected boolean specifier: ")+ DSLText(A.name));
    return A.bValue;
}

#include "DasDSLTypeAdapters.inl"
das::TypeDeclPtr DSLHandleType(das::ModuleGroup&, das::Module*, const char*);
#include "DasTextNameTypesDSL.inl"
FDasDeclaredLiteral DSLByteDefault(das::ExpressionPtr E)
{
    FDasDeclaredLiteral D;
    if (!E)
        return D;
    if (E->rtti_isCall())
    {
        auto* Call = static_cast<das::ExprCall*>(E);
        if (Call->name != "uint8" || Call->arguments.size() != 1)
            DSLFail(TEXT("uint8 default requires a bounded literal"));
        E = Call->arguments[0];
    }
    double Number = -1;
    if (E->__rtti == das::string("ExprConstUInt8"))
        Number = static_cast<das::ExprConstUInt8*>(E)->getValue();
    else if (E->__rtti == das::string("ExprConstInt"))
        Number = static_cast<das::ExprConstInt*>(E)->getValue();
    if (Number < 0 || Number > 255)
        DSLFail(TEXT("uint8 default must be in range 0..255"));
    D.Kind = TEXT("number");
    D.Number = Number;
    return D;
}
FDasDeclaredLiteral DSLParameterDefault(das::ExpressionPtr Init)
{
    FDasDeclaredLiteral Result;
    if (!Init)
        return Result;
    auto E = Init;
    bool Negative = false;
    if (E->__rtti == das::string("ExprOp1"))
    {
        auto* Unary = static_cast<das::ExprOp1*>(E);
        if (Unary->op != "-")
            DSLFail(TEXT("Parameter defaults require scalar literals"));
        Negative = true;
        E = Unary->subexpr;
    }
    if (E->__rtti == das::string("ExprConstInt"))
    {
        Result.Kind = TEXT("number");
        Result.Number = static_cast<das::ExprConstInt*>(E)->getValue();
    }
    else if (E->__rtti == das::string("ExprConstFloat"))
    {
        Result.Kind = TEXT("number");
        Result.Number = static_cast<das::ExprConstFloat*>(E)->getValue();
    }
    else if (E->__rtti == das::string("ExprConstDouble"))
    {
        Result.Kind = TEXT("number");
        Result.Number = static_cast<das::ExprConstDouble*>(E)->getValue();
    }
    else if (E->__rtti == das::string("ExprConstBool"))
    {
        Result.Kind = TEXT("bool");
        Result.Boolean = static_cast<das::ExprConstBool*>(E)->getValue();
    }
    else if (E->__rtti == das::string("ExprConstString"))
    {
        Result.Kind = TEXT("string");
        Result.String = DSLText(static_cast<das::ExprConstString*>(E)->text);
    }
    else
        DSLFail(TEXT("Parameter defaults require scalar literals"));
    if (Negative)
    {
        if (Result.Kind != TEXT("number"))
            DSLFail(TEXT("Only numeric defaults can be negated"));
        Result.Number = -Result.Number;
    }

    return Result;
}
das::ExpressionPtr DSLParameterDefaultExpression(const FDasDeclaredParameter& P, const das::LineInfo& At)
{
    if (P.Default.Kind.IsEmpty())
        return nullptr;
    if (const auto* Facet = DSLKindFacet(P.Type.GetKind()))
        if (auto Expression = Facet->DefaultExpression(P.Default, At))
            return Expression;
    if (P.Type.GetKind() == EDasTypeKind::Byte)
        return new das::ExprConstUInt8(At, uint8(P.Default.Number));
    if (P.Type.GetKind() == EDasTypeKind::Int)
        return new das::ExprConstInt(At, int32(P.Default.Number));
    if (P.Type.GetKind() == EDasTypeKind::Float)
        return new das::ExprConstFloat(At, float(P.Default.Number));
    if (P.Type.GetKind() == EDasTypeKind::Double)
        return new das::ExprConstDouble(At, P.Default.Number);
    if (P.Type.GetKind() == EDasTypeKind::Bool)
        return new das::ExprConstBool(At, P.Default.Boolean);
    if (P.Type.GetKind() == EDasTypeKind::String)
        return new das::ExprConstString(At, DSLUtf8(P.Default.String));
    DSLFail(TEXT("Unsupported parameter default type"));
    return nullptr;
}
bool DSLIsMetadata(const das::string& N)
{
    return N == "Category" || N == "DisplayName" || N == "ToolTip";
}
void DSLMetadata(const das::AnnotationArgument& A, TMap<FString, FString>& Out)
{
    if (A.type != das::Type::tString)
        DSLFail(TEXT("Metadata must be string: ")+ DSLText(A.name));
    Out.Add(DSLText(A.name), DSLText(A.sValue));
}
#include "DasCommentTooltips.inl"
const das::AnnotationDeclaration* DSLAnnotation(const das::AnnotationList& L, const char* N)
{
    const das::AnnotationDeclaration* Found = nullptr;
    for (auto* A : L)
        if (A->annotation->name == N)
        {
            if (Found)
                DSLFail(TEXT("Repeated singleton annotation: ")+ DSLText(N));
            Found = A;
        }
    return Found;
}
void DSLAnnotationArguments(const das::AnnotationArgumentList& Args, const TSet<FString>& Repeatable = {})
{
    TSet<FString> Seen;
    for (const auto& A : Args)
    {
        const FString Name = DSLText(A.name);
        if (Seen.Contains(Name) && !Repeatable.Contains(Name))
            DSLFail(TEXT("Repeated annotation option: ")+ Name);
        Seen.Add(Name);
    }
}
void DSLDeclarationKind(const das::AnnotationList& Args)
{
    TArray<FString> Kinds;
    for (const auto* A : Args)
    {
        const auto& Name = A->annotation->name;
        if (Name == "ue_library" || Name == "ue_native_base" || Name == "ue_object_ref" || Name == "ue_class" ||
            Name == "ue_interface" || Name == "ue_struct" || Name == "ue_native_struct" || Name == "ue_class_ref" ||
            Name == "ue_native_object" || Name == "ue_native_interface" || Name == "ue_interface_ref" ||
            Name == "ue_delegate" || Name == "ue_multicast" || Name == "ue_attribute_set" ||
            Name == "ue_gameplay_cue" || Name == "ue_gameplay_effect" || Name == "ue_gameplay_ability" ||
            Name == "ue_gas_calculation")
            Kinds.Add(DSLText(Name));
    }
    if (Kinds.Num() > 1)
        DSLFail(TEXT("Repeated/conflicting declaration annotations: ")+ FString::Join(Kinds, TEXT(",")));
}
void DSLAddDirections(const das::AnnotationArgument& A, TMap<FString, FString>& Directions)
{
    if (A.type != das::Type::tString)
        DSLFail(TEXT("Out/ref requires parameter names"));
    TArray<FString> Names;
    DSLText(A.sValue).ParseIntoArray(Names, TEXT(","), false);
    if (Names.IsEmpty())
        DSLFail(TEXT("Empty out/ref parameter list"));
    for (auto Name : Names)
    {
        Name = Name.TrimStartAndEnd();
        if (Name.IsEmpty() || Directions.Contains(Name))
            DSLFail(TEXT("Duplicate/overlapping out/ref parameter name"));
        Directions.Add(Name, A.name == "out" ? TEXT("out"): TEXT("inout"));
    }
}
FString DSLParameterDirection(const das::VariablePtr& Parameter, const FString& Legacy)
{
    FString Marker;
    TSet<FString> Seen;
    for (const auto& A : Parameter->annotation)
    {
        const FString Name = DSLText(A.name);
        if (Name != TEXT("ue_ref") && Name != TEXT("ue_out"))
        {
            if (Name.StartsWith(TEXT("ue_")))
                DSLFail(TEXT("Unknown UE parameter metadata: ")+ Name);
            continue;
        }
        if (Seen.Contains(Name))
            DSLFail(TEXT("Repeated parameter direction metadata: ")+ Name);
        Seen.Add(Name);
        if (A.type != das::Type::tBool || !A.bValue)
            DSLFail(TEXT("Parameter direction marker must be true: ")+ Name);
        const FString Direction = Name == TEXT("ue_out")? TEXT("out"): TEXT("inout");
        if (!Marker.IsEmpty())
            DSLFail(TEXT("Conflicting parameter ue_ref/ue_out metadata"));
        Marker = Direction;
    }
    if (!Marker.IsEmpty() && !Legacy.IsEmpty() && Marker != Legacy)
        DSLFail(TEXT("Conflicting parameter and function direction: ")+ DSLText(Parameter->name));
    const FString Explicit = Marker.IsEmpty() ? Legacy : Marker;
    const auto& Type = Parameter->type;
    if (!Type)
        DSLFail(TEXT("Missing parameter type"));
    if (Type->ref)
    {
        if (Type->constant)
            DSLFail(TEXT("Out/ref parameters require var mutable reference: ")+ DSLText(Parameter->name));
        return Explicit.IsEmpty() ? TEXT("inout"): Explicit;
    }
    if (!Explicit.IsEmpty())
        DSLFail(TEXT("Out/ref direction requires a mutable reference: ")+ DSLText(Parameter->name));
    return TEXT("in");
}
#include "DasTypeResolution.inl"
// Source-only type tokens lower to the existing checked runtime ABI.
struct FDSLTypeCallVisitor : das::Visitor
{
    FDSLTypeScope Scope;
    bool Resolve;
    explicit FDSLTypeCallVisitor(const FDSLTypeScope& InScope, bool InResolve = true)
        : Scope(InScope), Resolve(InResolve)
    {
    }
    virtual void preVisit(das::ExprCall* Call) override
    {
        das::string Name = Call->name;
        const das::string Prefix = "ue_foundation::";
        if (Name.find(Prefix) == 0)
            Name = Name.substr(Prefix.size());
        int32 Index = -1;
        EDSLTypeKind Kind = EDSLTypeKind::Class;
        if (Name == "ue_object_value")
            Index = 0;
        else if (Name == "ue_cast_object" || Name == "ue_cast_nullable_object" || Name == "ue_object_is_a")
            Index = 1;
        else if (Name == "ue_cast_interface" || Name == "ue_begin_interface_call" ||
                 Name == "ue_implements_interface" || Name == "ue_interface_call_int" ||
                 Name == "ue_interface_call_string")
        {
            Index = 1;
            Kind = EDSLTypeKind::Interface;
        }
        else if (Name == "ue_enum_value" || Name == "ue_enum_from_number")
        {
            Index = 0;
            Kind = EDSLTypeKind::Enum;
        }
        else if (Name == "ue_struct_value" || Name == "ue_struct_default")
        {
            Index = 0;
            Kind = EDSLTypeKind::Struct;
        }
        if (Index < 0 || Call->arguments.size() <= size_t(Index))
            return;
        auto* Argument = Call->arguments[size_t(Index)];
        bool Token = false;
        if (Argument->rtti_isCall())
        {
            auto* Marker = static_cast<das::ExprCall*>(Argument);
            if (Marker->name != "ue_type" && Marker->name != "ue_dsl::ue_type")
                return;
            if (Marker->arguments.size() != 1)
                DSLFail(TEXT("ue_type requires exactly one type identity"));
            Argument = Marker->arguments[0];
            Token = true;
        }
        FString Identity;
        if (Argument->__rtti == das::string("ExprConstString"))
            Identity = DSLText(static_cast<das::ExprConstString*>(Argument)->text);
        else if (Token && Argument->rtti_isVar())
            Identity = DSLText(static_cast<das::ExprVar*>(Argument)->name);
        else
        {
            if (Token)
                DSLFail(TEXT("ue_type requires a type name or literal identity"));
            return;
        }
        // Explicit full paths retain low-level runtime validation compatibility.
        if (!Token && Identity.StartsWith(TEXT("/")))
            return;
        if (!Resolve)
        {
            if (Token)
                Call->arguments[size_t(Index)] = new das::ExprConstString(Argument->at, DSLUtf8(Identity));
            return;
        }
        const FString Canonical = DSLResolveType(Kind, Identity, Scope);
        Call->arguments[size_t(Index)] = new das::ExprConstString(Argument->at, DSLUtf8(Canonical));
    }
};
#include "DasObjectTypes.inl"
#include "DasClassTypes.inl"
#include "DasSoftTypes.inl"
#include "DasGameplayTagsDSL.inl"
das::ExpressionPtr DSLDirectPack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr, const FString&);
das::ExpressionPtr DSLDirectUnpack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr, const FString&);
#include "DasObjectKeyTypes.inl"
FString DSLTypeName(const das::TypeDeclPtr& T)
{
    if (!T)
        return {};
    if (const auto* Entry = DSLValueAdapter(T))
        return DasTypeKindName(Entry->Adapter->GetKind());
    if (DSLObjectKey(T))
        return TEXT("object_key");
    if (DSLTypedClass(T))
        return TEXT("class");
    if (T->baseType == das::Type::tStructure && T->structType &&
        DSLAnnotation(T->structType->annotations, "ue_delegate"))
        return TEXT("delegate");
    if (DSLTypedReference(T))
        return TEXT("object");
    if (T->baseType == das::Type::tStructure && T->structType &&
        (DSLAnnotation(T->structType->annotations, "ue_struct") ||
         DSLAnnotation(T->structType->annotations, "ue_native_struct")))
        return TEXT("struct");
    switch (T->baseType)
    {
    case das::Type::tUInt8:
        return TEXT("byte");
    case das::Type::tArray:
        return TEXT("array");
    case das::Type::tTable:
        return T->secondType && T->secondType->baseType == das::Type::tVoid ? TEXT("set"): TEXT("map");
    case das::Type::tInt:
        return TEXT("int");
    case das::Type::tFloat:
        return TEXT("float");
    case das::Type::tBool:
        return TEXT("bool");
    case das::Type::tDouble:
        return TEXT("double");
    case das::Type::tString:
        return TEXT("string");
    case das::Type::tVoid:
        return TEXT("void");
    case das::Type::tHandle:
        if (T->annotation && T->annotation->name == "UEClassHandle")
            return TEXT("class");
        if (T->annotation && T->annotation->name == "UObjectHandle")
            return TEXT("object");
        if (T->annotation && (DSLAdaptedHandle(T) || T->annotation->name == "UEStructValue"))
            return TEXT("struct");
    default:
        break;
    }
    return {};
}
// Struct path of an adapted handle view (math structs, InputActionValue), or empty.
FString DSLMathPath(const das::TypeDeclPtr& T)
{
    const auto* Entry = DSLAdaptedHandle(T);
    return Entry ? Entry->Adapter->GetStruct()->GetPathName() : FString();
}
bool DSLPropertyMatches(FProperty* P, const FString& K)
{
    const auto Kind = DasTypeKind(K);
    return Kind != EDasTypeKind::Invalid && DasPropertyKind(P) == Kind;
}
bool DSLSet(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tTable && T->secondType && T->secondType->baseType == das::Type::tVoid;
}
bool DSLContainer(const das::TypeDeclPtr& T)
{
    return T && (T->baseType == das::Type::tArray || T->baseType == das::Type::tTable);
}
FString DSLDirectPath(const das::TypeDeclPtr&, const FString&, const FString&);
FDasDeclaredClass DSLReferenceDeclaration(const das::TypeDeclPtr&);
FDasDeclaredType DSLContainerType(const das::TypeDeclPtr& T, const FString& Folder = FString(),
                                  const FString& Registry = FString())
{
    FDasDeclaredType D;
    D.Kind = DSLTypeName(T);
    auto First = MakeShared<FDasDeclaredType>();
    First->Kind = DSLTypeName(T->firstType);
    if (DSLObjectKey(T->firstType))
    {
        if (D.GetKind() != EDasTypeKind::Set && D.GetKind() != EDasTypeKind::Map)
            DSLFail(TEXT("Object keys are only supported as map keys or set elements"));
        const auto* Key = DSLKeyAnnotation(T->firstType);
        First->Kind = TEXT("object");
        First->TypePath = Key->ClassPath;
        First->AuthoredId = Key->AuthoredId;
    }
    else if ((D.GetKind() == EDasTypeKind::Set || D.GetKind() == EDasTypeKind::Map) &&
             First->GetKind() == EDasTypeKind::Object)
        DSLFail(TEXT("Use UEObjectKey<Reference> for UObject table keys"));
    if (D.GetKind() == EDasTypeKind::Array && DSLTypedReference(T->firstType))
    {
        if (DSLAuthoredView(T->firstType))
            First->AuthoredId = DSLReferenceDeclaration(T->firstType).Id;
        First->TypePath = DSLTypedReferencePath(T->firstType);
        if (!First->TypePath.StartsWith(TEXT("/Script/")))
            DSLFail(TEXT("Reference arrays require native object/interface types"));
    }
    if ((D.GetKind() == EDasTypeKind::Array || D.GetKind() == EDasTypeKind::Set || D.GetKind() == EDasTypeKind::Map) &&
        DSLTypedValue(T->firstType, EDasTypeKind::Enum))
        First->TypePath = DSLDirectPath(T->firstType, Folder, Registry);
    if (D.GetKind() == EDasTypeKind::Array && First->GetKind() == EDasTypeKind::Struct)
    {
        if (DSLAdapted(T->firstType))
            First->TypePath = DSLAdaptedPath(T->firstType);
        else if (!DSLMathPath(T->firstType).IsEmpty())
            First->TypePath = DSLMathPath(T->firstType);
        else
        {
            if (!T->firstType->structType ||
                (!DSLAnnotation(T->firstType->structType->annotations, "ue_struct") &&
                 !DSLAnnotation(T->firstType->structType->annotations, "ue_native_struct")))
                DSLFail(TEXT("Struct arrays require authored ue_struct or supported math elements"));
            First->TypePath = DSLDirectPath(T->firstType, Folder, Registry);
        }
    }
    if (D.GetKind() == EDasTypeKind::Map)
    {
        D.Key = First;
        auto Value = MakeShared<FDasDeclaredType>();
        Value->Kind = DSLTypeName(T->secondType);
        if (DSLTypedValue(T->secondType, EDasTypeKind::Enum))
            Value->TypePath = DSLDirectPath(T->secondType, Folder, Registry);
        if (DSLTypedReference(T->secondType))
        {
            if (DSLAuthoredView(T->secondType))
                Value->AuthoredId = DSLReferenceDeclaration(T->secondType).Id;
            Value->TypePath = DSLTypedReferencePath(T->secondType);
            if (!Value->TypePath.StartsWith(TEXT("/Script/")))
                DSLFail(TEXT("Reference maps require native object/interface hosts"));
        }
        if (Value->GetKind() == EDasTypeKind::Struct)
        {
            if (DSLAdapted(T->secondType))
                Value->TypePath = DSLAdaptedPath(T->secondType);
            else if (!DSLMathPath(T->secondType).IsEmpty())
                Value->TypePath = DSLMathPath(T->secondType);
            else
            {
                if (!T->secondType->structType ||
                    (!DSLAnnotation(T->secondType->structType->annotations, "ue_struct") &&
                     !DSLAnnotation(T->secondType->structType->annotations, "ue_native_struct")))
                    DSLFail(TEXT("Struct maps require authored ue_struct or supported math values"));
                Value->TypePath = DSLDirectPath(T->secondType, Folder, Registry);
            }
        }
        D.Value = Value;
    }
    else
        D.Element = First;
    if (!DasContainerCarrier(D))
        DSLFail(TEXT("Standalone containers require primitive/authored-struct arrays, int/string/name/enum/object-key sets or int/string/name/enum/object-key scalar/name/struct/enum/reference maps"));
    return D;
}
bool DSLDelegate(const das::TypeDeclPtr&);
das::ExpressionPtr DSLDelegateSnapshot(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr);
#include "DasAdapterValueTypes.inl"
#include "DasEnumTypesDSL.inl"
#include "DasDirectTypes.inl"
#include "DasMathTypesDSL.inl"
struct FUESourceValueBinding : das::FunctionAnnotation
{
    FUESourceValueBinding() : FunctionAnnotation("ue_source_value_binding") {}
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
    virtual bool apply(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool finalize(const das::FunctionPtr& Function, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                          const das::AnnotationArgumentList&, das::string& Error) override
    {
        try
        {
            DSLAnnotationArguments(Args);
            if (Args.size() != 4)
                DSLFail(TEXT("Source value binding requires kind, id, path and registry"));
            const FString Kind = DSLString(Args, "kind"), Id = DSLString(Args, "id"), Path = DSLString(Args, "path"),
                          Registry = DSLString(Args, "registry");
            das::TypeDeclPtr Source = Function->result;
            if (!DSLTypedValue(Source, EDasTypeKind::Enum) && !DSLDirectStruct(Source))
            {
                if (Function->arguments.size() != 1)
                    DSLFail(TEXT("Source value converter requires one parameter"));
                Source = Function->arguments[0]->type;
            }
            if (DSLNativeValue(Source, EDasTypeKind::Enum) ||
                (DasTypeKind(Kind) == EDasTypeKind::Enum     ? !DSLTypedValue(Source, EDasTypeKind::Enum)
                 : DasTypeKind(Kind) == EDasTypeKind::Struct ? !DSLDirectStruct(Source)
                                                             : true) ||
                Registry.IsEmpty())
                DSLFail(TEXT("Source value binding type/kind mismatch"));
            const auto* A = DSLTypedValue(Source, EDasTypeKind::Enum)
                                ? DSLAnnotation(Source->enumType->annotations, "ue_enum")
                                : DSLAnnotation(Source->structType->annotations, "ue_struct");
            const auto* Wire = A->arguments.find("_ue_types_json", das::Type::tString);
            FDasReflectedTypes Declared;
            FString Issue;
            if (!Wire || !FDasReflectedTypes::FromJson(DSLText(Wire->sValue), Declared, Issue, true))
                DSLFail(TEXT("Source binding has no declaration identity"));
            const FString ActualId =
                DasTypeKind(Kind) == EDasTypeKind::Enum ? Declared.Enums[0].Id : Declared.Structs[0].Id;
            if (ActualId != Id || DSLDirectPath(Source, FString(), Registry) != Path)
                DSLFail(TEXT("Source value binding identity/layout mismatch"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
FString DSLMathPropertyDefault(const das::Structure::FieldDeclaration& Field);
bool DSLDelegate(const das::TypeDeclPtr&);
das::ExpressionPtr DSLDelegateView(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr, const FString&);
das::ExpressionPtr DSLDelegateFieldRead(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr,
                                        const FString&);
das::ExpressionPtr DSLDelegateFieldWrite(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr,
                                         const FString&, das::ExpressionPtr);
#include "DasComputedDefaultsState.inl"
#include "DasAuthoredRefs.inl"
#include "DasHandlerABI.inl"
#include "DasDelegateDSL.inl"
#include "DasComponentFields.inl"
#include "DasDeclarationGroups.inl"
#include "DasComputedDefaults.inl"
#include "DasIdentityDSL.inl"
#include "DasComponentDefaults.inl"
#include "DasContainerDefaultsDSL.inl"
bool DSLVirtualMethod(const FDasDeclaredClass& C, const FDasDeclaredMethod& M)
{
    if (M.bBlueprintEvent)
        return true;
    for (const auto& Path : C.Interfaces)
        if (auto* Interface = FindObject<UClass>(nullptr, *Path); Interface && Interface->FindFunctionByName(*M.Name))
            return true;
    return false;
}
// Native descriptors are compilation-local. Never serialize them as authored fields.
FProperty* DSLNativeSelfField(const FDasDeclaredClass& C, const das::string& Name)
{
    auto* Base = DasNativeDefaultBase(C.NativeParent);
    if (Base)
    {
        if (auto* P = Base->FindPropertyByName(*DSLText(Name)))
            return P;
    }

    return nullptr;
}
bool DSLNativeSelfKnown(const FDasDeclaredClass& C, const das::string& N)
{
    return DSLNativeSelfField(C, N) ||
           (DasActorNativeParent(C.NativeParent) && DSLText(N).Equals(TEXT("ActorTransform"), ESearchCase::IgnoreCase));
}
FDasDeclaredProperty DSLNativeSelfProperty(const FDasDeclaredClass& C, const das::string& Name)
{
    const FString N = DSLText(Name);
    if (DasActorNativeParent(C.NativeParent) && N.Equals(TEXT("ActorTransform"), ESearchCase::CaseSensitive))
    {
        FDasDeclaredProperty D;
        D.Name = N;
        D.Type.Kind = TEXT("struct");
        D.Type.TypePath = TEXT("/Script/CoreUObject.Transform");
        return D;
    }
    auto* P = DSLNativeSelfField(C, Name);
    // Owner is an explicit readonly facade over the public GetOwner(), not
    // permission to access arbitrary non-Blueprint-visible native storage.
    const bool OwnerFacade =
        DasActorNativeParent(C.NativeParent) && N.Equals(TEXT("Owner"), ESearchCase::CaseSensitive);
    if (!P || !P->GetName().Equals(N, ESearchCase::CaseSensitive) || P->ArrayDim != 1 ||
        P->HasAnyPropertyFlags(CPF_EditorOnly | CPF_Deprecated) ||
        (!OwnerFacade && !P->HasAnyPropertyFlags(CPF_BlueprintVisible)))
        DSLFail(TEXT("Unknown or inaccessible native self property: ")+ N);
    // Accessor-backed fields must go through their reflected getter/setter.
    // UE strips BlueprintGetter/Setter metadata when cooking. Keep the small
    // Actor adapter's accessor policy explicit so editor and game agree.
    const bool ActorAccessor =
        DasActorNativeParent(C.NativeParent) &&
        (N == TEXT("bAutoDestroyWhenFinished") || N == TEXT("NetCullDistanceSquared") ||
                                                            N == TEXT("NetUpdateFrequency") ||
                                                                      N == TEXT("MinNetUpdateFrequency"));
    if (P->HasSetterOrGetter() || ActorAccessor)
        DSLFail(TEXT("Native self property requires its accessor: ")+ N);
    FDasDeclaredProperty D;
    D.Name = N;
    D.bReadOnly = OwnerFacade || P->HasAnyPropertyFlags(CPF_BlueprintReadOnly);
    D.Type.Kind = CastField<FIntProperty>(P) ? TEXT("int")
                                             : CastField<FFloatProperty>(P)
        ? TEXT("float")
        : CastField<FDoubleProperty>(P)
        ? TEXT("double")
        : CastField<FBoolProperty>(P)
        ? TEXT("bool")
        : CastField<FStrProperty>(P)
        ? TEXT("string")
        : CastField<FNameProperty>(P)
        ? TEXT("name")
        : CastField<FClassProperty>(P)
        ? TEXT("class")
        : CastField<FObjectProperty>(P)
        ? TEXT("object")
        : TEXT("");
    auto NativeValue = [&](FProperty* Value)
    {
        auto Type = MakeShared<FDasDeclaredType>();
        if (!Value || Value->ArrayDim != 1)
            return Type;
        Type->Kind = CastField<FIntProperty>(Value) ? TEXT("int")
                                                    : CastField<FFloatProperty>(Value)
            ? TEXT("float")
            : CastField<FDoubleProperty>(Value)
            ? TEXT("double")
            : CastField<FBoolProperty>(Value)
            ? TEXT("bool")
            : CastField<FStrProperty>(Value)
            ? TEXT("string")
            : CastField<FNameProperty>(Value)
            ? TEXT("name")
            : TEXT("");
        if (auto* Enum = DasEnumPropertyType(Value))
        {
            Type->Kind = TEXT("enum");
            Type->TypePath = Enum->GetPathName();
        }
        if (auto* Struct = CastField<FStructProperty>(Value))
        {
            FString Issue;
            if (!ValidateDasValueStruct(Struct->Struct->GetPathName(), Issue))
                DSLFail(Issue);
            Type->Kind = TEXT("struct");
            Type->TypePath = Struct->Struct->GetPathName();
        }
        if (!CastField<FClassProperty>(Value) && DasReferencePropertyClass(Value))
        {
            Type->Kind = TEXT("object");
            Type->TypePath = DasReferencePropertyClass(Value)->GetPathName();
        }
        return Type;
    };
    if (auto* Array = CastField<FArrayProperty>(P))
    {
        D.Type.Kind = TEXT("array");
        D.Type.Element = NativeValue(Array->Inner);
        if (D.Type.Element->Kind.IsEmpty())
            DSLFail(TEXT("Unsupported native array element"));
    }
    if (auto* Map = CastField<FMapProperty>(P))
    {
        D.Type.Kind = TEXT("map");
        D.Type.Key = NativeValue(Map->KeyProp);
        D.Type.Value = NativeValue(Map->ValueProp);
        if ((D.Type.Key->GetKind() != EDasTypeKind::Int && D.Type.Key->GetKind() != EDasTypeKind::String &&
             D.Type.Key->GetKind() != EDasTypeKind::Name && D.Type.Key->GetKind() != EDasTypeKind::Enum &&
             D.Type.Key->GetKind() != EDasTypeKind::Object) ||
            D.Type.Value->Kind.IsEmpty())
            DSLFail(TEXT("Native maps require int/string/name/enum/object keys and supported values"));
    }
    if (auto* Set = CastField<FSetProperty>(P))
    {
        D.Type.Kind = TEXT("set");
        D.Type.Element = NativeValue(Set->ElementProp);
        if (D.Type.Element->GetKind() != EDasTypeKind::Int && D.Type.Element->GetKind() != EDasTypeKind::String &&
            D.Type.Element->GetKind() != EDasTypeKind::Name && D.Type.Element->GetKind() != EDasTypeKind::Enum &&
            D.Type.Element->GetKind() != EDasTypeKind::Object)
            DSLFail(TEXT("Native sets require int/string/name/enum/object elements"));
    }
    if (D.Type.GetKind() == EDasTypeKind::Array || D.Type.GetKind() == EDasTypeKind::Map ||
        D.Type.GetKind() == EDasTypeKind::Set)
    {
        auto* Carrier = DasContainerCarrierForProperty(P);
        auto* Values = Carrier ? Carrier->FindPropertyByName(TEXT("Values")) : nullptr;
        if (!Values || !Values->SameType(P))
            DSLFail(TEXT("Native container carrier differs from property: ")+ N);
    }
    if (auto* Enum = DasEnumPropertyType(P))
    {
        FString Issue;
        if (!ResolveDasEnum(Enum->GetPathName(), Issue))
            DSLFail(Issue);
        D.Type.Kind = TEXT("enum");
        D.Type.TypePath = Enum->GetPathName();
    }
    if (auto* Struct = CastField<FStructProperty>(P))
    {
        FString Issue;
        if (!ValidateDasValueStruct(Struct->Struct->GetPathName(), Issue))
            DSLFail(Issue);
        D.Type.Kind = TEXT("struct");
        D.Type.TypePath = Struct->Struct->GetPathName();
    }
    if (D.Type.Kind.IsEmpty())
        DSLFail(TEXT("Unsupported native self property type: ")+ N);
    if (auto* Class = CastField<FClassProperty>(P))
        D.Type.TypePath = Class->MetaClass->GetPathName();
    else if (auto* Object = CastField<FObjectProperty>(P))
        D.Type.TypePath = Object->PropertyClass->GetPathName();
    return D;
}
FString DSLCallbackKind(const das::TypeDeclPtr& T)
{
    const FString K = DSLTypeName(T), Math = DSLMathPath(T);
    if (K == TEXT("array"))
    {
        const FString Wire = TEXT("array_")+ DSLTypeName(T->firstType);
        return FindDasCallbackType(Wire) ? Wire : TEXT("struct");
    }
    if (DSLContainer(T))
        return TEXT("struct");
    if (const auto* Adapter = DasMathAdapter(Math))
        return Adapter->GetDefaultKind();
    return K;
}
#include "DasHandlerLookupDSL.inl"
#include "DasCallbackFramesDSL.inl"
#include "DasReflectionMacros.inl"
#include "DasWidgetLayoutsDSL.inl"
#include "DasWidgetLayoutUsesDSL.inl"
#include "DasDSLBodyVisitor.inl"
#include "DasOperationKeysDSL.inl"
#include "DasInputNamesDSL.inl"
#include "DasDSLInlineCallbacks.inl"
#include "DasDSLMemberAnnotations.inl"
#include "DasHandlerReferencesDSL.inl"
#include "DasEventChecksDSL.inl"
#include "DasDSLClassAnnotation.inl"
#include "DasDSLInterfaceAnnotation.inl"
#include "DasDSLTypeAnnotations.inl"
#include "DasGASDSL.inl"
#include "DasGASAbilityDSL.inl"
#include "DasGASCueDSL.inl"
#include "DasGASCalculationDSL.inl"
#include "DasStaticLibraryDSL.inl"

bool DSLValidateOwner(const das::ProgramPtr& Program, const FString& Entry, UObject* Self, FString& Wire,
                      FString& Error)
{
    if (Entry.StartsWith(TEXT("__ue_dispatch_")))
        if (auto* F = Program->getThisModule()->findUniqueFunction(DSLUtf8(Entry)))
            for (const auto* A : F->annotations)
                if (F->generated && A->annotation->name == "__ue_handler_contract")
                    if (const auto* Owner = A->arguments.find("owner_entry", das::Type::tString))
                        if (Owner->sValue.find("ue_") == 0)
                            return DSLValidateOwner(Program, DSLText(Owner->sValue), Self, Wire, Error);
    if (GASCalculationContract)
    {
        bool Match = false;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& S)
            {
                const auto* A = DSLAnnotation(S->annotations, "ue_gas_calculation");
                const auto* Data = A ? A->arguments.find(DSLGASKey, das::Type::tString) : nullptr;
                TSharedPtr<FJsonObject> O;
                FString Contract, Point;
                if (Data && FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(DSLText(Data->sValue)), O) &&
                    O->TryGetStringField(TEXT("contract"), Contract) &&
                                         O->TryGetStringField(TEXT("entry"), Point) &&
                                                              Contract == *GASCalculationContract && Point == Entry)
                    Match = true;
            });
        Wire.Reset();
        if (!Match)
            Error = TEXT("GAS calculation source contract differs from saved asset");
        return Match;
    }
    if (const auto* Session = Cast<UDasGASCueSession>(Self))
    {
        bool Match = false;
        const auto* Cue = Session->Definition.Get();
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& S)
            {
                const auto* A = DSLAnnotation(S->annotations, "ue_gameplay_cue");
                const auto* Data = A ? A->arguments.find(DSLGASKey, das::Type::tString) : nullptr;
                TSharedPtr<FJsonObject> O;
                FString Contract, Id;
                if (Cue && Data &&
                    FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(DSLText(Data->sValue)), O) &&
                    O->TryGetStringField(TEXT("contract"), Contract) &&
                                         O->TryGetStringField(TEXT("id"), Id) && Id == Cue->DefinitionId &&
                                                              Contract == Cue->ScriptContract)
                    for (const auto& H : Cue->Hooks)
                        Match |= H.Value == Entry;
            });
        Wire.Reset();
        if (!Match)
            Error = TEXT("Cue source contract differs from saved notify asset");
        return Match;
    }
    if (const auto* Ability = Cast<UDasAuthoredGameplayAbility>(Self); Ability && !Ability->ActivateEntry.IsEmpty())
    {
        bool Match = false;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& S)
            {
                const auto* A = DSLAnnotation(S->annotations, "ue_gameplay_ability");
                const auto* Data = A ? A->arguments.find(DSLGASKey, das::Type::tString) : nullptr;
                TSharedPtr<FJsonObject> Object;
                FString Contract, Id;
                if (Data && FJsonSerializer::Deserialize(TJsonReaderFactory<>::Create(DSLText(Data->sValue)), Object) &&
                    Object->TryGetStringField(
                        TEXT("contract"), Contract) &&
                        Object->TryGetStringField(TEXT("id"), Id) && Id == Ability->DefinitionId &&
                                                  Contract == Ability->ScriptContract &&
                                                  (Entry == Ability->ActivateEntry || Entry == Ability->EndEntry ||
                                                   Entry == Ability->CancelEntry || Entry == Ability->TaskEntry))
                {
                    // The native CDO validates this contract; this is not an Actor/Library schema.
                    Wire.Reset();
                    Match = true;
                }
            });
        if (!Match)
            Error = TEXT("Ability script contract differs from saved asset; reimport into a new folder");
        return Match;
    }
    if (LibraryInvocationContract && Self)
    {
        bool Match = false;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& St)
            {
                if (const auto* A = DSLAnnotation(St->annotations, "ue_library"))
                    if (const auto* J = A->arguments.find(DSLModelKey, das::Type::tString))
                    {
                        FDasAuthoringModule M;
                        FString Issue;
                        if (FDasAuthoringModule::FromJson(DSLText(J->sValue), M, Issue) &&
                            DSLText(J->sValue) == *LibraryInvocationContract &&
                            M.Classes.ContainsByPredicate(
                                [&](const auto& C)
                                {
                                    return C.Methods.ContainsByPredicate([&](const auto& Method)
                                                                         { return Method.EntryPoint == Entry; });
                                }))
                        {
                            Wire = DSLText(J->sValue);
                            Match = true;
                        }
                    }
            });
        if (!Match)
            Error = TEXT("Static library source contract differs from saved Blueprint; reimport library");
        return Match;
    }
    bool OK = true;
    Program->getThisModule()->structures.foreach (
        [&](const das::StructurePtr& St)
        {
            const auto* A = DSLAnnotation(St->annotations, "ue_class");
            if (!A || !OK)
                return;
            const auto* J = A->arguments.find(DSLModelKey, das::Type::tString);
            if (!J)
                return;
            FDasAuthoringModule Model;
            if (!FDasAuthoringModule::FromJson(DSLText(J->sValue), Model, Error))
            {
                OK = false;
                return;
            }
            for (const auto& C : Model.Classes)
                if (C.Methods.ContainsByPredicate([&](const auto& M) { return M.EntryPoint == Entry; }) ||
                    ((!C.Callbacks.IsEmpty() || C.AsyncPhysics.IsSet()) &&
                     (Entry ==
                      TEXT("main") ||
                           Entry == TEXT("begin_play") ||
                                         Entry == TEXT("tick") ||
                                                       Entry == TEXT("end_play") ||
                                                                     C.Callbacks.ContainsByPredicate(
                                                                         [&](const auto& Callback)
                                                                         {
                                                                             return Callback.Kind ==
                                                                                    TEXT("event") &&
                                                                                         Entry ==
                                                                                             TEXT("ue_")+ C.Name +
                                                                                                  TEXT("_")+
                                                                                                       Callback.Name;
                                                                         }))))
                {
                    Wire = DSLText(J->sValue);
                    if (!IsValid(Self))
                    {
                        Error = TEXT("DSL entry requires live self");
                        OK = false;
                        return;
                    }
                    const bool ActorAdapter = Self->IsA<ADasActor>() || Self->IsA<UDasScriptComponent>() ||
                                              Self->IsA<UDasSaveGame>() || Self->IsA<UDasDataAsset>() ||
                                              Self->IsA<UDasPrimaryDataAsset>();
                    FDasDeclaredClass EffectiveDefaults;
                    if (!Model.ResolveClass(C.Id, EffectiveDefaults, Error))
                    {
                        OK = false;
                        return;
                    }
                    if (!DasCheckNativeDefaultOwner(Model, C.Id, Self, Error))
                    {
                        OK = false;
                        return;
                    }
                    for (const auto& P : EffectiveDefaults.Properties)
                    {
                        FProperty* Actual = Self->GetClass()->FindPropertyByName(*P.Name);
                        auto* DeclaringCDO =
                            Actual && Actual->GetOwnerClass() ? Actual->GetOwnerClass()->GetDefaultObject() : nullptr;
                        for (const auto* Owner = &C; Owner;)
                        {
                            if (Owner->PropertyOverrides.ContainsByPredicate([&](const auto& O)
                                                                             { return O.Id == P.Id; }))
                            {
                                DeclaringCDO = nullptr;
                                for (auto* Class = Self->GetClass(); Class; Class = Class->GetSuperClass())
                                {
                                    auto* Candidate = Class->GetDefaultObject();
                                    const FString Identity = DasHostIdentity(Candidate);
                                    if (Identity == Owner->Id)
                                        DeclaringCDO = Candidate;
                                }
                                if (!DeclaringCDO)
                                {
                                    Error = TEXT("Missing inherited default owner: ")+ Owner->Id;
                                    OK = false;
                                    return;
                                }
                                break;
                            }
                            Owner = Owner->ParentId.IsEmpty()
                                        ? nullptr
                                        : Model.Classes.FindByPredicate([&](const auto& V)
                                                                        { return V.Id == Owner->ParentId; });
                        }
                        const auto* Contracts = DasHostContracts(DeclaringCDO);
                        const bool CheckCDODefault =
                            ActorAdapter && (!Contracts || Contracts->FindRef(P.Id) != DasDefaultSourceContract(P));
                        if (auto* Carrier = DasContainerCarrier(P.Type))
                        {
                            FInstancedStruct Expected;
                            if (!ParseDasStructDefault(Carrier->GetPathName(), P.Default.String, Expected, Error))
                            {
                                OK = false;
                                return;
                            }
                            auto* Values = Carrier->FindPropertyByName(TEXT("Values"));
                            if (!Actual || !Values || !Values->SameType(Actual) ||
                                P.bReadOnly != Actual->HasAnyPropertyFlags(CPF_BlueprintReadOnly) ||
                                (CheckCDODefault && !MatchesDasPropertyDefault(
                                                        Actual, Actual->ContainerPtrToValuePtr<void>(DeclaringCDO),
                                                        Values->ContainerPtrToValuePtr<void>(Expected.GetMemory()))))
                            {
                                Error = TEXT("DSL container identity/default differs from owner: ")+ P.Name;
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Delegate)
                        {
                            auto* D = CastField<FDelegateProperty>(Actual);
                            FDasAuthoringModule Definition;
                            FString Issue;
                            if (!FDasAuthoringModule::FromJson(P.DelegateWire, Definition, Issue))
                            {
                                Error = Issue;
                                OK = false;
                                return;
                            }
                            TStrongObjectPtr<UFunction> Sig(MakeDasDelegateSignature(
                                Definition.Classes[0].Methods[0], false, GetTransientPackage(), NAME_None, Issue));
                            if (!D || !Sig || !Sig->IsSignatureCompatibleWith(D->SignatureFunction) ||
                                P.bReadOnly != D->HasAnyPropertyFlags(CPF_BlueprintReadOnly))
                            {
                                Error = TEXT("Single delegate field differs from owner: ")+ P.Name;
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Event)
                        {
                            auto* Event = CastField<FMulticastDelegateProperty>(Actual);
                            FDasAuthoringModule Definition;
                            FString Issue;
                            if (!FDasAuthoringModule::FromJson(P.DelegateWire, Definition, Issue))
                            {
                                Error = Issue;
                                OK = false;
                                return;
                            }
                            TStrongObjectPtr<UFunction> Signature(MakeDasDelegateSignature(
                                Definition.Classes[0].Methods[0], true, GetTransientPackage(), NAME_None, Issue));
                            if (!Event || !Signature ||
                                !Signature->IsSignatureCompatibleWith(Event->SignatureFunction) ||
                                !Event->HasAllPropertyFlags(CPF_BlueprintAssignable | CPF_BlueprintCallable))
                            {
                                Error = TEXT("Authored event differs from owner; reimport Blueprint: ")+ P.Name;
                                auto Summary = [](UFunction* F)
                                {
                                    FString S;
                                    if (F)
                                        for (TFieldIterator<FProperty> I(F); I; ++I)
                                            if (I->HasAnyPropertyFlags(CPF_Parm))
                                            {
                                                FDasDeclaredType T;
                                                FString E;
                                                DescribeDasProperty(*I, T, E);
                                                S += T.StorageIdentity() +
                                                     FString::Printf(TEXT(" offset=%d size=%d flags=%llx;"),
                                                                          I->GetOffset_ForUFunction(), I->GetSize(),
                                                                          uint64(I->PropertyFlags));
                                            }
                                    return S;
                                };
                                Error +=
                                    TEXT(" expected ")+ Summary(Signature.Get()) +
                                         TEXT(" actual ")+ Summary(Event ? Event->SignatureFunction.Get() : nullptr);
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (!DSLPropertyMatches(Actual, P.Type.Kind) ||
                            P.bReadOnly != Actual->HasAnyPropertyFlags(CPF_BlueprintReadOnly))
                        {
                            Error = TEXT("DSL property differs from owner; apply/reimport Blueprint: ")+ P.Name;
                            OK = false;
                            return;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Class)
                        {
                            auto* Class = CastField<FClassProperty>(Actual);
                            if (!Class || !Class->MetaClass || Class->MetaClass->GetPathName() != P.Type.TypePath ||
                                (CheckCDODefault && !P.Default.String.IsEmpty() &&
                                 (Class->GetObjectPropertyValue_InContainer(DeclaringCDO)
                                      ? Class->GetObjectPropertyValue_InContainer(DeclaringCDO)->GetPathName()
                                      : FString()) != P.Default.String))
                            {
                                Error = TEXT("DSL class base differs from owner: ")+ P.Name;
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Enum)
                        {
                            auto* Type = ResolveDasEnum(P.Type.TypePath, Error);
                            if (!Type || DasEnumPropertyType(Actual) != Type ||
                                (CheckCDODefault &&
                                 DasEnumPropertyNumber(Actual, Actual->ContainerPtrToValuePtr<void>(DeclaringCDO)) !=
                                     Type->GetValueByNameString(P.Default.String)))
                            {
                                Error = TEXT("DSL enum identity/default differs from owner");
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Object)
                        {
                            auto* ReferenceClass = DasReferencePropertyClass(Actual);
                            if (ReferenceClass->GetPathName() != P.Type.TypePath ||
                                (CheckCDODefault &&
                                 (DasReferenceGet(Actual, Actual->ContainerPtrToValuePtr<void>(DeclaringCDO))
                                      ? DasReferenceGet(Actual, Actual->ContainerPtrToValuePtr<void>(DeclaringCDO))
                                            ->GetPathName()
                                      : FString()) != P.Default.String))
                            {
                                Error = TEXT("DSL object type/default differs from owner; apply/reimport Blueprint: ")+
                                             P.Name;
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Struct)
                        {
                            auto* Struct = CastField<FStructProperty>(Actual);
                            FInstancedStruct Default;
                            if (Struct->Struct->GetPathName() != P.Type.TypePath ||
                                !ParseDasStructDefault(P.Type.TypePath, P.Default.String, Default, Error) ||
                                (CheckCDODefault &&
                                 !MatchesDasStructDefault(Struct->Struct,
                                                          Struct->ContainerPtrToValuePtr<void>(DeclaringCDO),
                                                          Default.GetMemory())))
                            {
                                Error = TEXT("DSL struct type/default differs from owner; apply/reimport Blueprint: ")+
                                             P.Name;
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Byte)
                        {
                            if (CheckCDODefault &&
                                CastFieldChecked<FByteProperty>(Actual)->GetPropertyValue_InContainer(DeclaringCDO) !=
                                    uint8(P.Default.Number))
                            {
                                Error = TEXT("DSL byte default differs from owner");
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (P.Type.GetKind() == EDasTypeKind::Text)
                        {
                            FText Expected;
                            if (!ParseDasTextDefault(P.Default.String, Expected, Error) ||
                                (CheckCDODefault &&
                                 !DasTextDefaultsEqual(CastFieldChecked<FTextProperty>(Actual)
                                                           ->GetPropertyValue_InContainer(DeclaringCDO),
                                                       Expected)))
                            {
                                Error = TEXT("DSL text default differs from owner");
                                OK = false;
                                return;
                            }
                            continue;
                        }
                        if (CheckCDODefault)
                        {
                            UObject* CDO = DeclaringCDO;
                            const bool DefaultMatches =
                                P.Type.GetKind() == EDasTypeKind::Int
                                    ? CastField<FIntProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          static_cast<int32>(P.Default.Number)
                                : P.Type.GetKind() == EDasTypeKind::Float
                                    ? CastField<FFloatProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          static_cast<float>(P.Default.Number)
                                : P.Type.GetKind() == EDasTypeKind::Bool
                                    ? CastField<FBoolProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          P.Default.Boolean
                                : P.Type.GetKind() == EDasTypeKind::Double
                                    ? CastField<FDoubleProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          P.Default.Number
                                : P.Type.GetKind() == EDasTypeKind::String
                                    ? CastField<FStrProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          P.Default.String
                                    : CastField<FNameProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          FName(*P.Default.String);
                            if (!DefaultMatches)
                            {
                                Error = TEXT("DSL default differs from owner; apply/reimport Blueprint: ")+ P.Name;
                                OK = false;
                                return;
                            }
                        }
                    }
                    if (ActorAdapter)
                        for (const auto& I : C.Interfaces)
                        {
                            auto* Interface = FindObject<UClass>(nullptr, *I);
                            if (!Interface || !Self->GetClass()->ImplementsInterface(Interface))
                            {
                                Error = TEXT("DSL interface differs from owner; apply/reimport Blueprint");
                                OK = false;
                                return;
                            }
                        }
                    if (ActorAdapter)
                        for (const auto& M : C.Methods)
                        {
                            UFunction* Method = Self->FindFunction(*M.Name);
                            auto Matches = [](FProperty* P, const FDasDeclaredType& T)
                            {
                                if (T.GetKind() == EDasTypeKind::Delegate)
                                {
                                    FString Error;
                                    TStrongObjectPtr<UFunction> Sig(
                                        MakeDasSingleDelegateSignature(T.DelegateWire, Error));
                                    auto* D = CastField<FDelegateProperty>(P);
                                    return D && Sig && D->SignatureFunction &&
                                           Sig->IsSignatureCompatibleWith(D->SignatureFunction);
                                }
                                if (auto* Carrier = DasContainerCarrier(T))
                                    return P && Carrier->FindPropertyByName(TEXT("Values"))->SameType(P);
                                return DSLPropertyMatches(P, T.Kind);
                            };
                            bool Fits = Method && Method->HasAnyFunctionFlags(FUNC_BlueprintPure) == M.bPure &&
                                        Method->HasAnyFunctionFlags(FUNC_Const) == M.bConst &&
                                        Method->NumParms ==
                                            M.Parameters.Num() + (M.ReturnType.GetKind() == EDasTypeKind::Void ? 0 : 1);
                            bool Directed = false;
                            for (const auto& P : M.Parameters)
                                Directed |= P.Direction != TEXT("in");
                            if (Fits && !Directed)
                            {
                                int32 Index = 0;
                                for (TFieldIterator<FProperty> It(Method); It; ++It)
                                    if (It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))
                                    {
                                        Fits &= M.Parameters.IsValidIndex(Index) &&
                                                It->GetFName() == FName(*M.Parameters[Index].Name);
                                        ++Index;
                                    }
                                Fits &= Index == M.Parameters.Num();
                            }
                            if (Fits)
                                for (const auto& P : M.Parameters)
                                {
                                    auto* Actual = Method->FindPropertyByName(*P.Name);
                                    Fits &= Matches(Actual, P.Type) &&
                                            (P.Direction ==
                                             TEXT("out")
                                                  ? Actual->HasAnyPropertyFlags(CPF_OutParm) &&
                                                        !Actual->HasAnyPropertyFlags(CPF_ReferenceParm)
                                                  : P.Direction == TEXT("inout")
                                                                        ? Actual->HasAllPropertyFlags(CPF_OutParm |
                                                                                                      CPF_ReferenceParm)
                                                                        : (Actual->HasAnyPropertyFlags(CPF_ConstParm) ||
                                                                           P.Type.GetKind() == EDasTypeKind::Array ||
                                                                           !Actual->HasAnyPropertyFlags(
                                                                               CPF_OutParm | CPF_ReferenceParm)));
                                    if (Fits && P.Type.GetKind() == EDasTypeKind::Enum)
                                        Fits = DasEnumPropertyType(Actual)->GetPathName() == P.Type.TypePath;
                                    if (Fits && P.Type.GetKind() == EDasTypeKind::Struct &&
                                        !DasContainerCarrier(P.Type))
                                    {
                                        auto* NativeStruct = CastField<FStructProperty>(Actual);
                                        Fits = NativeStruct && NativeStruct->Struct->GetPathName() == P.Type.TypePath;
                                    }
                                    if (Fits && P.Type.GetKind() == EDasTypeKind::Class)
                                        Fits = CastFieldChecked<FClassProperty>(Actual)->MetaClass->GetPathName() ==
                                               P.Type.TypePath;
                                    if (Fits && P.Type.GetKind() == EDasTypeKind::Object)
                                        Fits = DasReferencePropertyClass(Actual)->GetPathName() == P.Type.TypePath;
                                }
                            if (Fits)
                                Fits = M.ReturnType.GetKind() == EDasTypeKind::Void
                                           ? !Method->GetReturnProperty()
                                           : Matches(Method->GetReturnProperty(), M.ReturnType);
                            if (Fits && M.ReturnType.GetKind() == EDasTypeKind::Enum)
                                Fits = DasEnumPropertyType(Method->GetReturnProperty())->GetPathName() ==
                                       M.ReturnType.TypePath;
                            if (Fits && M.ReturnType.GetKind() == EDasTypeKind::Struct &&
                                !DasContainerCarrier(M.ReturnType))
                            {
                                auto* NativeStruct = CastField<FStructProperty>(Method->GetReturnProperty());
                                Fits = NativeStruct && NativeStruct->Struct->GetPathName() == M.ReturnType.TypePath;
                            }
                            if (Fits && M.ReturnType.GetKind() == EDasTypeKind::Class)
                                Fits = CastFieldChecked<FClassProperty>(Method->GetReturnProperty())
                                           ->MetaClass->GetPathName() == M.ReturnType.TypePath;
                            if (Fits && M.ReturnType.GetKind() == EDasTypeKind::Object)
                                Fits = DasReferencePropertyClass(Method->GetReturnProperty())->GetPathName() ==
                                       M.ReturnType.TypePath;
                            if (!Fits)
                            {
                                Error = TEXT("DSL method differs from owner; apply/reimport Blueprint: ")+ M.Name;
                                OK = false;
                                return;
                            }
                        }
                }
        });
    return OK;
}
