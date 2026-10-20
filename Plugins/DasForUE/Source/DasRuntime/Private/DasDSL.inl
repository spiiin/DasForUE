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

FDasDeclaredLiteral DSLTextDefault(das::ExpressionPtr E)
{
    FDasDeclaredLiteral D;
    if (!E)
        return D;
    if (!E->rtti_isCall())
        DSLFail(TEXT("FText defaults require ue_text or ue_text_invariant literals"));
    auto* Call = static_cast<das::ExprCall*>(E);
    const bool Invariant = Call->name == "ue_text_invariant";
    if (!Invariant && Call->name != "ue_text")
        DSLFail(TEXT("FText defaults require ue_text or ue_text_invariant"));
    if (Call->arguments.size() != (Invariant ? 1 : 3))
        DSLFail(TEXT("Invalid text default arguments"));
    TArray<FString> A;
    for (auto* X : Call->arguments)
    {
        if (X->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Text default arguments must be string literals"));
        A.Add(DSLText(static_cast<das::ExprConstString*>(X)->text));
    }
    if ((!Invariant && (A[0].IsEmpty() || A[1].IsEmpty() || A[0].Len() > 1024 || A[1].Len() > 1024)) ||
        A.Last().Len() > 65536)
        DSLFail(TEXT("Text default identity/source limits exceeded"));
    D.Kind = TEXT("string");
    D.String = DasTextLiteral(Invariant ? FString() : A[0], Invariant ? FString() : A[1], A.Last(), Invariant);
    return D;
}
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
das::ExpressionPtr DSLTextDefaultExpression(const FDasDeclaredLiteral& D, const das::LineInfo& At)
{
    FText Text;
    FString Error;
    if (!ParseDasTextDefault(D.String, Text, Error))
        DSLFail(Error);
    auto* Call = new das::ExprCall(At, Text.IsCultureInvariant() ? "ue_text_invariant" : "ue_text");
    if (!Text.IsCultureInvariant())
    {
        const auto N = FTextInspector::GetNamespace(Text), K = FTextInspector::GetKey(Text);
        if (!N.IsSet() || !K.IsSet())
            DSLFail(TEXT("Text default has no localization identity"));
        Call->arguments.push_back(new das::ExprConstString(At, DSLUtf8(N.GetValue())));
        Call->arguments.push_back(new das::ExprConstString(At, DSLUtf8(K.GetValue())));
    }
    const auto* Source = FTextInspector::GetSourceString(Text);
    Call->arguments.push_back(new das::ExprConstString(At, DSLUtf8(Source ? *Source : FString())));
    return Call;
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
    if (P.Type.Kind == TEXT("text"))
        return DSLTextDefaultExpression(P.Default, At);
    if (P.Type.Kind == TEXT("byte"))
        return new das::ExprConstUInt8(At, uint8(P.Default.Number));
    if (P.Type.Kind == TEXT("int"))
        return new das::ExprConstInt(At, int32(P.Default.Number));
    if (P.Type.Kind == TEXT("float"))
        return new das::ExprConstFloat(At, float(P.Default.Number));
    if (P.Type.Kind == TEXT("double"))
        return new das::ExprConstDouble(At, P.Default.Number);
    if (P.Type.Kind == TEXT("bool"))
        return new das::ExprConstBool(At, P.Default.Boolean);
    if (P.Type.Kind == TEXT("string"))
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
            Name == "ue_delegate" || Name == "ue_multicast")
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
    if (DSLSoft(T) || DSLGameplay(T))
        return TEXT("struct");
    if (DSLObjectKey(T))
        return TEXT("object_key");
    if (DSLTypedClass(T))
        return TEXT("class");
    if (T->baseType == das::Type::tStructure && T->structType &&
        DSLAnnotation(T->structType->annotations, "ue_delegate"))
        return TEXT("delegate");
    if (DSLTypedReference(T))
        return TEXT("object");
    if (T->baseType == das::Type::tEnumeration && T->enumType &&
        (DSLAnnotation(T->enumType->annotations, "ue_enum") ||
         DSLAnnotation(T->enumType->annotations, "ue_native_enum")))
        return TEXT("enum");
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
        if (T->annotation && T->annotation->name == "UETextValue")
            return TEXT("text");
        if (T->annotation && T->annotation->name == "UEClassHandle")
            return TEXT("class");
        if (T->annotation && T->annotation->name == "UEEnumValue")
            return TEXT("enum");
        if (T->annotation && T->annotation->name == "UENameValue")
            return TEXT("name");
        if (T->annotation && T->annotation->name == "UObjectHandle")
            return TEXT("object");
        if (T->annotation && (T->annotation->name == "InputActionValue" || T->annotation->name == "UEVector2D" ||
                              T->annotation->name == "UEVector" || T->annotation->name == "UERotator" ||
                              T->annotation->name == "UETransform" || T->annotation->name == "UEStructValue"))
            return TEXT("struct");
    default:
        break;
    }
    return {};
}
FString DSLMathPath(const das::TypeDeclPtr& T)
{
    if (!T || !T->annotation)
        return {};
    return T->annotation->name == "InputActionValue" ? TEXT("/Script/EnhancedInput.InputActionValue")
                                                     : T->annotation->name == "UEVector2D"
        ? TEXT("/Script/CoreUObject.Vector2D")
        : T->annotation->name == "UEVector"
        ? TEXT("/Script/CoreUObject.Vector")
        : T->annotation->name == "UERotator"
        ? TEXT("/Script/CoreUObject.Rotator")
        : T->annotation->name == "UETransform"
        ? TEXT("/Script/CoreUObject.Transform")
        : FString();
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
bool DSLDirectEnum(const das::TypeDeclPtr&);
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
        if (D.Kind != TEXT("set") && D.Kind != TEXT("map"))
            DSLFail(TEXT("Object keys are only supported as map keys or set elements"));
        const auto* Key = DSLKeyAnnotation(T->firstType);
        First->Kind = TEXT("object");
        First->TypePath = Key->ClassPath;
        First->AuthoredId = Key->AuthoredId;
    }
    else if ((D.Kind == TEXT("set") || D.Kind == TEXT("map")) && First->Kind == TEXT("object"))
        DSLFail(TEXT("Use UEObjectKey<Reference> for UObject table keys"));
    if (D.Kind == TEXT("array") && DSLTypedReference(T->firstType))
    {
        if (DSLAuthoredView(T->firstType))
            First->AuthoredId = DSLReferenceDeclaration(T->firstType).Id;
        First->TypePath = DSLTypedReferencePath(T->firstType);
        if (!First->TypePath.StartsWith(TEXT("/Script/")))
            DSLFail(TEXT("Reference arrays require native object/interface types"));
    }
    if ((D.Kind == TEXT("array") || D.Kind == TEXT("set") || D.Kind == TEXT("map")) && DSLDirectEnum(T->firstType))
        First->TypePath = DSLDirectPath(T->firstType, Folder, Registry);
    if (D.Kind == TEXT("array") && First->Kind == TEXT("struct"))
    {
        if (DSLGameplay(T->firstType))
            First->TypePath = DSLGameplayPath(T->firstType);
        else if (DSLSoft(T->firstType))
            First->TypePath = DSLSoftPath(T->firstType);
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
    if (D.Kind == TEXT("map"))
    {
        D.Key = First;
        auto Value = MakeShared<FDasDeclaredType>();
        Value->Kind = DSLTypeName(T->secondType);
        if (DSLDirectEnum(T->secondType))
            Value->TypePath = DSLDirectPath(T->secondType, Folder, Registry);
        if (DSLTypedReference(T->secondType))
        {
            if (DSLAuthoredView(T->secondType))
                Value->AuthoredId = DSLReferenceDeclaration(T->secondType).Id;
            Value->TypePath = DSLTypedReferencePath(T->secondType);
            if (!Value->TypePath.StartsWith(TEXT("/Script/")))
                DSLFail(TEXT("Reference maps require native object/interface hosts"));
        }
        if (Value->Kind == TEXT("struct"))
        {
            if (DSLGameplay(T->secondType))
                Value->TypePath = DSLGameplayPath(T->secondType);
            else if (DSLSoft(T->secondType))
                Value->TypePath = DSLSoftPath(T->secondType);
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
#include "DasDirectTypes.inl"
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
            if (!DSLDirectEnum(Source) && !DSLDirectStruct(Source))
            {
                if (Function->arguments.size() != 1)
                    DSLFail(TEXT("Source value converter requires one parameter"));
                Source = Function->arguments[0]->type;
            }
            if (DSLNativeEnum(Source) ||
                (Kind == TEXT("enum")? !DSLDirectEnum(Source)
                                     : Kind == TEXT("struct")? !DSLDirectStruct(Source) : true) ||
                              Registry.IsEmpty())
                DSLFail(TEXT("Source value binding type/kind mismatch"));
            const auto* A = DSLDirectEnum(Source) ? DSLAnnotation(Source->enumType->annotations, "ue_enum")
                                                  : DSLAnnotation(Source->structType->annotations, "ue_struct");
            const auto* Wire = A->arguments.find("_ue_types_json", das::Type::tString);
            FDasReflectedTypes Declared;
            FString Issue;
            if (!Wire || !FDasReflectedTypes::FromJson(DSLText(Wire->sValue), Declared, Issue, true))
                DSLFail(TEXT("Source binding has no declaration identity"));
            const FString ActualId = Kind == TEXT("enum")? Declared.Enums[0].Id : Declared.Structs[0].Id;
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
FString DSLStructEnumDefault(const das::Structure::FieldDeclaration& Field)
{
    FString Name;
    if (!Field.init)
        DSLFail(TEXT("Enum struct members require an explicit named default"));
    if (Field.init->__rtti == das::string("ExprConstEnumeration"))
    {
        auto* E = static_cast<das::ExprConstEnumeration*>(Field.init);
        if (E->enumType && E->enumType != Field.type->enumType)
            DSLFail(TEXT("Enum struct default has a different enum type"));
        Name = DSLText(E->text);
    }
    else if (Field.init->rtti_isField())
    {
        auto* E = static_cast<das::ExprField*>(Field.init);
        const auto Qualified = Field.type->enumType->module->name + "::" + Field.type->enumType->name;
        if (!E->value->rtti_isVar() || (static_cast<das::ExprVar*>(E->value)->name != Field.type->enumType->name &&
                                        static_cast<das::ExprVar*>(E->value)->name != Qualified))
            DSLFail(TEXT("Enum struct default must name its declared enum"));
        Name = DSLText(E->name);
    }
    else
        DSLFail(TEXT("Enum struct defaults require named enum literals"));
    if (Name.Contains(TEXT("::")))
        Name = Name.RightChop(Name.Find(TEXT("::"), ESearchCase::CaseSensitive, ESearchDir::FromEnd) + 2);
    if (!std::any_of(Field.type->enumType->list.begin(), Field.type->enumType->list.end(),
                     [&](const auto& V) { return DSLText(V.name) == Name; }))
        DSLFail(TEXT("Unknown enum struct default member"));
    return Name;
}
bool DSLDelegate(const das::TypeDeclPtr&);
das::ExpressionPtr DSLDelegateView(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr, const FString&);
das::ExpressionPtr DSLDelegateFieldRead(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr,
                                        const FString&);
das::ExpressionPtr DSLDelegateFieldWrite(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr,
                                         const FString&, das::ExpressionPtr);
#include "DasComputedDefaultsState.inl"
#include "DasAuthoredRefs.inl"
#include "DasDelegateDSL.inl"
#include "DasComponentFields.inl"
#include "DasDeclarationGroups.inl"
#include "DasComputedDefaults.inl"
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
    // Explicit test-only native host extension, never inferred from bridge storage.
    if (C.NativeParent == TEXT("/Script/Engine.Actor") && IsDasNativeArraySelfProbeEnabled() &&
                               DSLText(Name).StartsWith(TEXT("DasNativeArray")))
        return ADasProbeActor::StaticClass()->FindPropertyByName(*DSLText(Name));
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
        if ((D.Type.Key->Kind !=
             TEXT("int") && D.Type.Key->Kind != TEXT("string") &&
                                                     D.Type.Key->Kind != TEXT("name") &&
                                                                              D.Type.Key->Kind !=
                                                                                  TEXT("enum") && D.Type.Key->Kind !=
                                                                                                      TEXT("object")) ||
                                                                                  D.Type.Value->Kind.IsEmpty())
            DSLFail(TEXT("Native maps require int/string/name/enum/object keys and supported values"));
    }
    if (auto* Set = CastField<FSetProperty>(P))
    {
        D.Type.Kind = TEXT("set");
        D.Type.Element = NativeValue(Set->ElementProp);
        if (D.Type.Element->Kind !=
            TEXT("int") && D.Type.Element->Kind != TEXT("string") &&
                                                        D.Type.Element->Kind !=
                                                            TEXT("name") && D.Type.Element->Kind !=
                                                                                TEXT("enum") && D.Type.Element->Kind !=
                                                                                                    TEXT("object"))
            DSLFail(TEXT("Native sets require int/string/name/enum/object elements"));
    }
    if (D.Type.Kind == TEXT("array") || D.Type.Kind == TEXT("map") || D.Type.Kind == TEXT("set"))
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
    if (!Math.IsEmpty())
    {
        if (Math == TEXT("/Script/CoreUObject.Vector"))
            return TEXT("vector");
        if (Math == TEXT("/Script/CoreUObject.Rotator"))
            return TEXT("rotator");
        if (Math == TEXT("/Script/CoreUObject.Transform"))
            return TEXT("transform");
        if (Math == TEXT("/Script/CoreUObject.Vector2D"))
            return TEXT("vector2d");
    }
    return K;
}
#include "DasCallbackFramesDSL.inl"
#include "DasWidgetLayoutsDSL.inl"
#include "DasWidgetLayoutUsesDSL.inl"
#include "DasDSLBodyVisitor.inl"
#include "DasDSLMemberAnnotations.inl"
#include "DasDSLClassAnnotation.inl"
#include "DasDSLInterfaceAnnotation.inl"
#include "DasDSLTypeAnnotations.inl"
#include "DasStaticLibraryDSL.inl"

bool DSLValidateOwner(const das::ProgramPtr& Program, const FString& Entry, UObject* Self, FString& Wire,
                      FString& Error)
{
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
                    const bool ActorAdapter = Self->IsA<ADasProbeActor>() || Self->IsA<UDasScriptComponent>() ||
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
                        if (P.Type.Kind == TEXT("delegate"))
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
                        if (P.Type.Kind == TEXT("event"))
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
                        if (P.Type.Kind == TEXT("class"))
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
                        if (P.Type.Kind == TEXT("enum"))
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
                        if (P.Type.Kind == TEXT("object"))
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
                        if (P.Type.Kind == TEXT("struct"))
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
                        if (P.Type.Kind == TEXT("byte"))
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
                        if (P.Type.Kind == TEXT("text"))
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
                                P.Type.Kind ==
                                TEXT("int")
                                    ? CastField<FIntProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                          static_cast<int32>(P.Default.Number)
                                    : P.Type.Kind ==
                                          TEXT("float")
                                              ? CastField<FFloatProperty>(Actual)->GetPropertyValue_InContainer(CDO) ==
                                                    static_cast<float>(P.Default.Number)
                                              : P.Type.Kind ==
                                                    TEXT("bool")
                                                         ? CastField<FBoolProperty>(Actual)
                                                                   ->GetPropertyValue_InContainer(CDO) ==
                                                               P.Default.Boolean
                                                         : P.Type.Kind ==
                                                               TEXT("double")
                                                                    ? CastField<FDoubleProperty>(Actual)
                                                                              ->GetPropertyValue_InContainer(CDO) ==
                                                                          P.Default.Number
                                                                    : P.Type.Kind ==
                                                                          TEXT("string")
                                                                               ? CastField<FStrProperty>(Actual)
                                                                                         ->GetPropertyValue_InContainer(
                                                                                             CDO) == P.Default.String
                                                                               : CastField<FNameProperty>(Actual)
                                                                                         ->GetPropertyValue_InContainer(
                                                                                             CDO) ==
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
                                if (T.Kind == TEXT("delegate"))
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
                            bool Fits =
                                Method && Method->HasAnyFunctionFlags(FUNC_BlueprintPure) == M.bPure &&
                                Method->HasAnyFunctionFlags(FUNC_Const) == M.bConst &&
                                Method->NumParms == M.Parameters.Num() + (M.ReturnType.Kind == TEXT("void")? 0 : 1);
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
                                    Fits &=
                                        Matches(Actual, P.Type) &&
                                        (P.Direction ==
                                         TEXT("out")
                                              ? Actual->HasAnyPropertyFlags(CPF_OutParm) &&
                                                    !Actual->HasAnyPropertyFlags(CPF_ReferenceParm)
                                              : P.Direction ==
                                                    TEXT("inout")
                                                         ? Actual->HasAllPropertyFlags(CPF_OutParm | CPF_ReferenceParm)
                                                         : (Actual->HasAnyPropertyFlags(CPF_ConstParm) ||
                                                            P.Type.Kind == TEXT("array") ||
                                                                                !Actual->HasAnyPropertyFlags(
                                                                                    CPF_OutParm | CPF_ReferenceParm)));
                                    if (Fits && P.Type.Kind == TEXT("enum"))
                                        Fits = DasEnumPropertyType(Actual)->GetPathName() == P.Type.TypePath;
                                    if (Fits && P.Type.Kind == TEXT("struct") && !DasContainerCarrier(P.Type))
                                    {
                                        auto* NativeStruct = CastField<FStructProperty>(Actual);
                                        Fits = NativeStruct && NativeStruct->Struct->GetPathName() == P.Type.TypePath;
                                    }
                                    if (Fits && P.Type.Kind == TEXT("class"))
                                        Fits = CastFieldChecked<FClassProperty>(Actual)->MetaClass->GetPathName() ==
                                               P.Type.TypePath;
                                    if (Fits && P.Type.Kind == TEXT("object"))
                                        Fits = DasReferencePropertyClass(Actual)->GetPathName() == P.Type.TypePath;
                                }
                            if (Fits)
                                Fits = M.ReturnType.Kind == TEXT("void")
                                                                 ? !Method->GetReturnProperty()
                                                                 : Matches(Method->GetReturnProperty(), M.ReturnType);
                            if (Fits && M.ReturnType.Kind == TEXT("enum"))
                                Fits = DasEnumPropertyType(Method->GetReturnProperty())->GetPathName() ==
                                       M.ReturnType.TypePath;
                            if (Fits && M.ReturnType.Kind == TEXT("struct") && !DasContainerCarrier(M.ReturnType))
                            {
                                auto* NativeStruct = CastField<FStructProperty>(Method->GetReturnProperty());
                                Fits = NativeStruct && NativeStruct->Struct->GetPathName() == M.ReturnType.TypePath;
                            }
                            if (Fits && M.ReturnType.Kind == TEXT("class"))
                                Fits = CastFieldChecked<FClassProperty>(Method->GetReturnProperty())
                                           ->MetaClass->GetPathName() == M.ReturnType.TypePath;
                            if (Fits && M.ReturnType.Kind == TEXT("object"))
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
