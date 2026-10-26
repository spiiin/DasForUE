#include "DasObjectNativeView.inl"
#include "DasEnumNativeView.inl"
#include "DasSoftNativeView.inl"
das::TypeDeclPtr DSLNativeValueType(das::Program* Program, das::Module* Module, const FDasDeclaredType& Type,
                                    const das::LineInfo& At)
{
    if (Type.GetKind() == EDasTypeKind::Map || Type.GetKind() == EDasTypeKind::Set)
    {
        auto* T = new das::TypeDecl(das::Type::tTable, At);
        const auto Key = Type.GetKind() == EDasTypeKind::Map ? Type.Key : Type.Element;
        if (!Key || (Type.GetKind() == EDasTypeKind::Map && !Type.Value))
            DSLFail(TEXT("Missing native hash container descriptor"));
        T->firstType = DSLNativeValueType(Program, Module, *Key, At);
        if (Key->GetKind() == EDasTypeKind::Object)
            T->firstType = DSLMakeObjectKeyType(T->firstType, *Program->thisModuleGroup, Module, At);
        T->secondType = Type.GetKind() == EDasTypeKind::Set
                            ? static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tVoid, At))
                            : DSLNativeValueType(Program, Module, *Type.Value, At);
        return T;
    }
    if (Type.GetKind() == EDasTypeKind::Array)
    {
        if (!Type.Element)
            DSLFail(TEXT("Missing native array element"));
        auto* T = new das::TypeDecl(das::Type::tArray, At);
        T->firstType = DSLNativeValueType(Program, Module, *Type.Element, At);
        return T;
    }
    if (Type.GetKind() == EDasTypeKind::Object || Type.GetKind() == EDasTypeKind::Class)
        return DSLKindFacet(Type.GetKind())->MakeViewForPath(Type.TypePath, *Program->thisModuleGroup, Module, At);
    if (Type.GetKind() == EDasTypeKind::Enum)
        return DSLKindFacet(Type.GetKind())->MakeViewForPath(Type.TypePath, *Program->thisModuleGroup, Module, At);
    if (Type.GetKind() == EDasTypeKind::Struct)
    {
        const auto& Path = Type.TypePath;
        if (auto View = DSLAdaptedNativeView(Path, *Program->thisModuleGroup, Module, At))
            return View;
        const TCHAR* Source = nullptr;
        for (const auto& V : DasNativeGameplayStructs)
            if (Path == V.Path)
            {
                Source = V.Source;
                break;
            }
        if (!Source)
            if (const auto* Adapter = DasAdapterValueStruct(FindObject<UScriptStruct>(nullptr, *Path)))
                Source = Adapter->Source;
        if (!Source)
            DSLFail(TEXT("Native struct not generated: ")+ Path);
        auto Types = Program->thisModuleGroup->findStructure(DSLUtf8(Source), Module);
        if (Types.size() != 1)
            DSLFail(TEXT("Import ue_native_gameplay for native struct: ")+ Path);
        auto* T = new das::TypeDecl(das::Type::tStructure, At);
        T->structType = Types[0];
        return T;
    }
    return DSLPropertyType(Type.Kind, *Program->thisModuleGroup, Module, At);
}
FDasDeclaredProperty DSLNativeReferenceProperty(const das::TypeDeclPtr& Receiver, const das::string& Name)
{
    if (Name.empty() || Name.length() >= NAME_SIZE)
        DSLFail(TEXT("Invalid native reference field name"));
    const FString Path = DSLTypedReferencePath(Receiver), Field = DSLText(Name);
    const auto* Row = DasNativeGameplayField(Path, Field);
    auto* Class = FindObject<UClass>(nullptr, *Path);
    auto* P = Row && Class ? Class->FindPropertyByName(*Field) : nullptr;
    FString Shape;
    if (P)
        AppendCallShape(P, Shape);
    if (!P || P->GetFName() != FName(*Field) || !Shape.Equals(Row->Signature, ESearchCase::IgnoreCase) ||
        P->GetOwnerClass()->GetPathName() != Row->Declaring || !P->HasAnyPropertyFlags(CPF_BlueprintVisible) ||
        P->HasAnyPropertyFlags(CPF_Deprecated | CPF_EditorOnly) ||
        P->HasAnyPropertyFlags(CPF_BlueprintReadOnly) != Row->ReadOnly)
        DSLFail(TEXT("Native field not generated or stale: ")+ Path + TEXT(".")+ Field);
    FString AccessError;
    if (!ValidateDasPropertyAccess(P, DasNativeAccessContract(*Row), AccessError))
        DSLFail(AccessError);
    FDasDeclaredProperty D;
    D.Name = Field;
    if (!DasNativeGameplayType(P, D.Type))
        DSLFail(TEXT("Unsupported generated native property shape"));
    D.bReadOnly = Row->ReadOnly;
    return D;
}
FDasDeclaredLiteral DSLNativeDefault(const FString& Class, const FString& Method, const FString& Name,
                                     const FDasDeclaredType& Type)
{
    const EDasTypeKind Kind = Type.GetKind();
    FDasDeclaredLiteral D;
    for (const auto& Row : DasNativeGameplayDefaults)
        if (Class == Row.Class && Method == Row.Method && Name == Row.Name)
        {
            const FString Text = Row.Text;
            if (Kind == EDasTypeKind::Bool && (Text == TEXT("true") || Text == TEXT("false")))
            {
                D.Kind = TEXT("bool");
                D.Boolean = Text == TEXT("true");
            }
            else if (Kind == EDasTypeKind::String)
            {
                D.Kind = TEXT("string");
                D.String = Text;
            }
            else if (Kind == EDasTypeKind::Enum)
            {
                auto* E = FindObject<UEnum>(nullptr, *Type.TypePath);
                int64 Value = E ? E->GetValueByNameString(Text) : -1;
                FString Error;
                if (E && ValidateDasEnumNumber(Type.TypePath, Value, Error))
                {
                    D.Kind = TEXT("enum");
                    D.String = E->GetNameStringByIndex(E->GetIndexByValue(Value));
                }
            }
            else if (Kind == EDasTypeKind::Int || Kind == EDasTypeKind::Byte)
            {
                int32 Value = 0;
                if (LexTryParseString(Value, *Text) && (Kind != EDasTypeKind::Byte || (Value >= 0 && Value <= 255)))
                {
                    D.Kind = TEXT("number");
                    D.Number = Value;
                }
            }
            else if (Kind == EDasTypeKind::Float || Kind == EDasTypeKind::Double)
            {
                double Value = 0;
                if (LexTryParseString(Value, *Text) && FMath::IsFinite(Value))
                {
                    D.Kind = TEXT("number");
                    D.Number = Value;
                }
            }
            break;
        }
    return D;
}
void DSLNativeMethodDescriptor(das::Program* Program, das::Module* Module, const das::TypeDeclPtr& Receiver,
                               const das::string& Name, const das::LineInfo& At, FDasDeclaredClass& Declaration,
                               FDasDeclaredMethod& Method, FDSLMethodSourceSignature& Descriptor)
{
    if (Name.empty() || Name.length() >= NAME_SIZE)
        DSLFail(TEXT("Invalid native method name"));
    Declaration.Id = DSLTypedReferencePath(Receiver);
    Declaration.Name = DSLText(Receiver->structType->name);
    Declaration.NativeParent = Declaration.Id;
    const auto* Row = DasNativeGameplayMethod(Declaration.Id, DSLText(Name));
    auto* Class = FindObject<UClass>(nullptr, *Declaration.Id);
    auto* Function = Row && Class ? Class->FindFunctionByName(*DSLText(Name)) : nullptr;
    if (!Function || !CallSignatureKey(Function).Equals(Row->Signature, ESearchCase::IgnoreCase))
        DSLFail(TEXT("Native method not generated or stale: ")+ Declaration.Id + TEXT(".")+ DSLText(Name));
    Method.Name = DSLText(Name);
    Method.Id = Declaration.Id + TEXT(".")+ Method.Name;
    Method.ReturnType.Kind = TEXT("void");
    Method.bConst = Function->HasAnyFunctionFlags(FUNC_Const);
    Descriptor.Scope = {FString(), FString(), FString(), nullptr, nullptr};
    for (TFieldIterator<FProperty> It(Function); It; ++It)
        if (It->HasAnyPropertyFlags(CPF_Parm))
        {
            FDasDeclaredType Type;
            if (!DasNativeGameplayType(*It, Type))
                DSLFail(TEXT("Unsupported native method parameter"));
            const auto SourceType = DSLNativeValueType(Program, Module, Type, At);
            const auto Source = DSLMethodSourceName(SourceType);
            if (It->HasAnyPropertyFlags(CPF_ReturnParm))
            {
                Method.ReturnType = Type;
                Descriptor.Result = Source;
                continue;
            }
            FDasDeclaredParameter Param;
            Param.Id = Method.Id + TEXT(".")+ It->GetName();
            Param.Name = It->GetName();
            Param.Type = Type;
            const bool Out = It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm);
            Param.Direction =
                Out ? (It->HasAnyPropertyFlags(CPF_ReferenceParm) ? TEXT("inout"): TEXT("out")) : TEXT("in");
            if (!Out)
                Param.Default = DSLNativeDefault(Declaration.Id, Method.Name, Param.Name, Type);
            Method.Parameters.Add(Param);
            Descriptor.Parameters.push_back(Source);
        }
    // Defaults must form a trailing suffix in daScript. Keep unsupported defaults
    // explicit rather than reinterpret C++ expressions or silently invent values.
    bool Trailing = true;
    for (int32 I = Method.Parameters.Num() - 1; I >= 0; --I)
    {
        if (Method.Parameters[I].Default.Kind.IsEmpty())
            Trailing = false;
        if (!Trailing)
            Method.Parameters[I].Default = {};
    }
    Declaration.Methods.Add(Method);
}
