// Part of DasDSL.inl: [ue_native_struct], [ue_native_enum], [ue_enum] and [ue_struct] annotations.
// Included in place; declaration order matters.
const char* DSLTypesKey = "_ue_types_json";
void DSLTypeArgs(const das::AnnotationArgumentList& Args, TMap<FString, FString>& Meta)
{
    DSLAnnotationArguments(Args);
    for (const auto& A : Args)
    {
        if (A.name == "module_id" || A.name == "id")
            continue;
        if (A.name == "BlueprintType")
        {
            if (!DSLBool(A))
                DSLFail(TEXT("BlueprintType must be enabled"));
            continue;
        }
        if (DSLIsMetadata(A.name))
            DSLMetadata(A, Meta);
        else
            DSLFail(TEXT("Unknown reflected type annotation"));
    }
}

struct FUENativeStructAnnotation : das::StructureAnnotation
{
    FUENativeStructAnnotation() : StructureAnnotation("ue_native_struct") {}
    virtual bool touch(const das::StructurePtr&, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            DSLAnnotationArguments(Args);
            if (Args.size() != 1 || Args[0].name != "path" || Args[0].type != das::Type::tString)
                DSLFail(TEXT("ue_native_struct requires one native path"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(S->annotations);
            auto* T = new das::TypeDecl(das::Type::tStructure, S->at);
            das::TypeDeclPtr Type = T;
            T->structType = S;
            DSLValidateNativeStruct(Type);
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUENativeEnumAnnotation : das::EnumerationAnnotation
{
    FUENativeEnumAnnotation() : EnumerationAnnotation("ue_native_enum") {}
    virtual bool touch(const das::EnumerationPtr& E, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            DSLAnnotationArguments(Args);
            if (Args.size() != 1 || Args[0].name != "path" || Args[0].type != das::Type::tString ||
                DSLAnnotation(E->annotations, "ue_enum"))
                DSLFail(TEXT("ue_native_enum requires a single native path and no ue_enum"));
            const FString Path = DSLString(Args, "path");
            FString Issue;
            auto* Native = Path.StartsWith(TEXT("/Script/")) ? ResolveDasEnum(Path, Issue) : nullptr;
            if (!Native)
                DSLFail(TEXT("Expected loaded native enum path"));
            TSet<int64> Values;
            for (const auto& V : E->list)
            {
                if (!V.value || V.value->__rtti != das::string("ExprConstInt"))
                    DSLFail(TEXT("Native enum declarations require explicit integer values"));
                const int64 Number = static_cast<das::ExprConstInt*>(V.value)->getValue();
                const int64 Actual = Native->GetValueByNameString(DSLText(V.name));
                const int32 Index = Native->GetIndexByValue(Actual);
                if (Number < 0 || Number > 254 || Index == INDEX_NONE || Actual != Number ||
                    !Native->GetNameStringByIndex(Index).Equals(DSLText(V.name), ESearchCase::CaseSensitive) ||
                    Values.Contains(Number) || Native->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX")))
                    DSLFail(TEXT("Native enum member name/value differs from Unreal"));
                Values.Add(Number);
            }
            for (int32 I = 0; I < Native->NumEnums(); ++I)
            {
                const int64 Number = Native->GetValueByIndex(I);
                if (Number >= 0 && Number <= 254 &&
                    !Native->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) && !Values.Contains(Number))
                    DSLFail(TEXT("Native enum declaration omits a native value"));
            }
            auto* F = new das::Function();
            F->at = E->at;
            F->name = "__ue_pack_native_enum_" + E->name;
            F->generated = true;
            F->privateFunction = false;
            F->result = DSLHandleType(Group, E->module, "UEEnumValue");
            auto* Arg = new das::Variable();
            Arg->at = E->at;
            Arg->name = "value";
            Arg->type = new das::TypeDecl(das::Type::tEnumeration, E->at);
            Arg->type->enumType = E;
            Arg->type->constant = true;
            F->arguments.push_back(Arg);
            auto* Body = new das::ExprBlock();
            Body->at = E->at;
            Body->list.push_back(
                new das::ExprReturn(E->at, DSLCall(E->at, "ue_enum_from_number",
                                                   {new das::ExprConstString(E->at, DSLUtf8(Path)),
                                                    DSLCall(E->at, "int", {new das::ExprVar(E->at, "value")})})));
            F->body = Body;
            if (!E->module->addFunction(F, true))
                DSLFail(TEXT("Native enum pack helper collision"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEEnumAnnotation : das::EnumerationAnnotation
{
    FUEEnumAnnotation() : EnumerationAnnotation("ue_enum") {}
    virtual bool touch(const das::EnumerationPtr& E, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (const auto* Wire = Args.find(DSLTypesKey, das::Type::tString))
            {
                if (Wire->at.fileInfo)
                    DSLFail(TEXT("Reserved type annotation argument"));
                return true;
            }
            DSLCommentTooltip(E->at, Args);
            FDasReflectedTypes M;
            M.Id = DSLString(Args, "module_id", FPaths::GetBaseFilename(DSLText(E->at.fileInfo->name)));
            FDasEnumDeclaration D;
            D.Name = DSLText(E->name);
            D.Id = DSLString(Args, "id", M.Id + TEXT(".")+ D.Name);
            das::AnnotationArgumentList TypeArgs;
            for (const auto& A : Args)
                if (A.name.find("member_id_") != 0)
                    TypeArgs.push_back(A);
            DSLTypeArgs(TypeArgs, D.Metadata);
            for (const auto& A : Args)
                if (A.name.find("member_id_") == 0)
                {
                    bool Found = false;
                    for (const auto& V : E->list)
                        Found |= A.name == "member_id_" + V.name;
                    if (!Found || A.type != das::Type::tString)
                        DSLFail(TEXT("Unknown member or invalid member ID annotation"));
                }
            for (const auto& V : E->list)
            {
                if (!V.value || V.value->__rtti != das::string("ExprConstInt"))
                    DSLFail(TEXT("ue_enum requires explicit int literal values"));
                D.Entries.Add({DSLText(V.name), static_cast<das::ExprConstInt*>(V.value)->getValue(),
                               DSLString(Args, ("member_id_" + V.name).c_str(), D.Id + TEXT(".")+ DSLText(V.name))});
            }
            M.Enums.Add(D);
            FString Wire, Issue;
            if (!M.ToJson(Wire, Issue))
                DSLFail(Issue);
            Args.push_back(das::AnnotationArgument(DSLTypesKey, DSLUtf8(Wire)));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEStructAnnotation : das::StructureAnnotation
{
    FUEStructAnnotation() : StructureAnnotation("ue_struct") {}
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(St->annotations);
            DSLAnnotation(St->annotations, "ue_struct");
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            DSLCommentTooltip(S->at, Args);
            DSLExpandDeclarationGroups(S, false);
            DSLNormalizeIdentityDefaults(S, Group);
            for (auto& Field : S->fields)
                if (!Field.generated)
                    DSLCommentTooltip(Field.at, Field.annotation);
            if (const auto* Wire = Args.find(DSLTypesKey, das::Type::tString))
            {
                if (Wire->at.fileInfo)
                    DSLFail(TEXT("Reserved type annotation argument"));
                return true;
            }
            if (S->isClass || S->parent)
                DSLFail(TEXT("ue_struct requires plain struct without inheritance"));
            FDasReflectedTypes M;
            M.Id = DSLString(Args, "module_id", FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name)));
            FDasDeclaredClass D;
            D.Name = DSLText(S->name);
            D.Id = DSLString(Args, "id", M.Id + TEXT(".")+ D.Name);
            DSLTypeArgs(Args, D.Metadata);
            for (auto& F : S->fields)
            {
                if (F.generated)
                    continue;
                if (DSLNativeStruct(F.type) ||
                    (F.type && (DSLNativeStruct(F.type->firstType) || DSLNativeStruct(F.type->secondType))))
                    DSLFail(TEXT("Native structs inside authored struct declarations require a separate adapter"));
                if (DSLNativeEnum(F.type) ||
                    (F.type && (DSLNativeEnum(F.type->firstType) || DSLNativeEnum(F.type->secondType))))
                    DSLFail(TEXT("Native enum fields inside authored structs require a separate adapter"));
                if (F.classMethod)
                    DSLFail(TEXT("Reflected struct methods require separate support"));
                FDasDeclaredProperty P;
                P.Name = DSLText(F.name);
                P.Id = D.Id + TEXT(".")+ P.Name;
                P.Type.Kind = DSLTypeName(F.type);
                if (DSLAdapted(F.type))
                {
                    if (F.init && DSLAdaptedImplicitDefault(F.type))
                        DSLFail(DSLText(F.type->structType->name) + TEXT(" struct fields use implicit empty defaults"));
                    P.Type.TypePath = DSLAdaptedPath(F.type);
                }
                if (DSLDirectEnum(F.type))
                {
                    const auto* A = DSLAnnotation(F.type->enumType->annotations, "ue_enum");
                    const FString Module =
                        DSLString(A->arguments, "module_id",
                                  FPaths::GetBaseFilename(DSLText(F.type->enumType->at.fileInfo->name)));
                    const FString Id =
                        DSLString(A->arguments, "id", Module + TEXT(".")+ DSLText(F.type->enumType->name));
                    P.Type.TypePath = TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Id) + TEXT(".Type");
                    P.Default.Kind = TEXT("string");
                    P.Default.String = DSLStructEnumDefault(F);
                }
                if (DSLAuthoredView(F.type))
                    DSLFail(TEXT("Authored reference struct fields require a separate ownership adapter"));
                if (P.Type.GetKind() == EDasTypeKind::Object)
                {
                    if (!DSLTypedReference(F.type) || F.init)
                        DSLFail(TEXT("Struct object fields require typed native wrappers and implicit null defaults"));
                    P.Type.TypePath = DSLTypedReferencePath(F.type);
                }
                if (DSLSet(F.type))
                {
                    const FString K = DSLTypeName(F.type->firstType);
                    if (F.init || (K != TEXT("byte") && K != TEXT("int") && K != TEXT("string")))
                        DSLFail(TEXT("Authored sets require int/string elements and empty defaults"));
                    P.Type.Kind = TEXT("set");
                    auto E = MakeShared<FDasDeclaredType>();
                    E->Kind = K;
                    P.Type.Element = E;
                }
                else if (F.type && F.type->baseType == das::Type::tTable)
                {
                    const FString Key = DSLTypeName(F.type->firstType), Value = DSLTypeName(F.type->secondType);
                    if (F.init ||
                        (Key !=
                         TEXT("byte") &&
                             Key != TEXT("int") && Key != TEXT("string")) ||
                                 (Value !=
                                  TEXT("byte") &&
                                      Value !=
                                          TEXT("text") &&
                                              Value !=
                                                  TEXT("int") &&
                                                      Value !=
                                                          TEXT("string") &&
                                                              Value !=
                                                                  TEXT("float") &&
                                                                      Value !=
                                                                          TEXT("double") &&
                                                                              Value !=
                                                                                  TEXT("bool") &&
                                                                                      Value !=
                                                                                          TEXT("name") &&
                                                                                              Value !=
                                                                                                  TEXT("struct") &&
                                                                                                      Value !=
                                                                                                          TEXT("enum") &&
                                                                                                              Value !=
                                                                                                                  TEXT("object")))
                        DSLFail(TEXT("Authored maps require int/string keys, scalar/name/struct/enum/reference values and empty defaults"));
                    P.Type.Kind = TEXT("map");
                    auto K = MakeShared<FDasDeclaredType>();
                    K->Kind = Key;
                    auto V = MakeShared<FDasDeclaredType>();
                    V->Kind = Value;
                    if (Value == TEXT("enum"))
                    {
                        if (!DSLDirectEnum(F.type->secondType))
                            DSLFail(TEXT("Enum maps require authored ue_enum values"));
                        const auto* A = DSLAnnotation(F.type->secondType->enumType->annotations, "ue_enum");
                        const FString Module = DSLString(
                            A->arguments, "module_id",
                            FPaths::GetBaseFilename(DSLText(F.type->secondType->enumType->at.fileInfo->name)));
                        const FString Id = DSLString(A->arguments, "id",
                                                     Module + TEXT(".")+ DSLText(F.type->secondType->enumType->name));
                        V->TypePath = TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Id) + TEXT(".Type");
                    }
                    if (Value == TEXT("object"))
                    {
                        if (!DSLTypedReference(F.type->secondType) || DSLAuthoredView(F.type->secondType))
                            DSLFail(TEXT("Struct reference map fields require native typed wrappers"));
                        V->TypePath = DSLTypedReferencePath(F.type->secondType);
                        if (!V->TypePath.StartsWith(TEXT("/Script/")))
                            DSLFail(TEXT("Struct reference maps require native object/interface types"));
                    }
                    if (Value == TEXT("struct"))
                    {
                        if (DSLAdapted(F.type->secondType))
                            V->TypePath = DSLAdaptedPath(F.type->secondType);
                        else if (!DSLMathPath(F.type->secondType).IsEmpty())
                            V->TypePath = DSLMathPath(F.type->secondType);
                        else
                        {
                            const auto* A =
                                F.type->secondType->structType
                                    ? DSLAnnotation(F.type->secondType->structType->annotations, "ue_struct")
                                    : nullptr;
                            if (!A)
                                DSLFail(TEXT("Map values require authored ue_struct or supported math types"));
                            const FString Module = DSLString(
                                A->arguments, "module_id",
                                FPaths::GetBaseFilename(DSLText(F.type->secondType->structType->at.fileInfo->name)));
                            const FString Id = DSLString(
                                A->arguments, "id", Module + TEXT(".")+ DSLText(F.type->secondType->structType->name));
                            V->TypePath = TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Id) + TEXT(".Type");
                        }
                    }
                    P.Type.Key = K;
                    P.Type.Value = V;
                }
                if (F.type && F.type->baseType == das::Type::tArray)
                {
                    const FString K = DSLTypeName(F.type->firstType);
                    if ((!DSLDirectStruct(F.type->firstType) && !DSLDirectEnum(F.type->firstType) &&
                         !DSLTypedReference(F.type->firstType) && DSLMathPath(F.type->firstType).IsEmpty() &&
                         K != TEXT("byte") &&
                                   K != TEXT("text") &&
                                             K != TEXT("int") &&
                                                       K != TEXT("float") &&
                                                                 K != TEXT("double") &&
                                                                           K != TEXT("bool") &&
                                                                                     K != TEXT("string") &&
                                                                                               K != TEXT("name")) ||
                                                                                         F.init)
                        DSLFail(TEXT("Authored arrays require supported scalar/ue_struct elements and empty defaults"));
                    auto E = MakeShared<FDasDeclaredType>();
                    E->Kind = K;
                    if (DSLTypedReference(F.type->firstType))
                    {
                        if (DSLAuthoredView(F.type->firstType))
                            DSLFail(
                                TEXT("Authored object array fields require a separate ownership/identity contract"));
                        E->TypePath = DSLTypedReferencePath(F.type->firstType);
                        if (!E->TypePath.StartsWith(TEXT("/Script/")))
                            DSLFail(TEXT("Reference array fields require native object/interface types"));
                    }
                    else if (DSLDirectEnum(F.type->firstType))
                    {
                        const auto* A = DSLAnnotation(F.type->firstType->enumType->annotations, "ue_enum");
                        const FString Module =
                            DSLString(A->arguments, "module_id",
                                      FPaths::GetBaseFilename(DSLText(F.type->firstType->enumType->at.fileInfo->name)));
                        const FString Id = DSLString(A->arguments, "id",
                                                     Module + TEXT(".")+ DSLText(F.type->firstType->enumType->name));
                        E->TypePath = TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Id) + TEXT(".Type");
                    }
                    else if (DSLAdapted(F.type->firstType))
                        E->TypePath = DSLAdaptedPath(F.type->firstType);
                    else if (!DSLMathPath(F.type->firstType).IsEmpty())
                        E->TypePath = DSLMathPath(F.type->firstType);
                    else if (DSLDirectStruct(F.type->firstType))
                    {
                        const auto* A = DSLAnnotation(F.type->firstType->structType->annotations, "ue_struct");
                        const FString Module = DSLString(
                            A->arguments, "module_id",
                            FPaths::GetBaseFilename(DSLText(F.type->firstType->structType->at.fileInfo->name)));
                        const FString Id = DSLString(A->arguments, "id",
                                                     Module + TEXT(".")+ DSLText(F.type->firstType->structType->name));
                        E->TypePath = TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Id) + TEXT(".Type");
                    }
                    P.Type.Kind = TEXT("array");
                    P.Type.Element = E;
                }
                if (DSLDirectStruct(F.type) && !DSLAdapted(F.type))
                {
                    const auto* Child = DSLAnnotation(F.type->structType->annotations, "ue_struct");
                    const FString Module =
                        DSLString(Child->arguments, "module_id",
                                  FPaths::GetBaseFilename(DSLText(F.type->structType->at.fileInfo->name)));
                    const FString Id =
                        DSLString(Child->arguments, "id", Module + TEXT(".")+ DSLText(F.type->structType->name));
                    P.Type.TypePath = TEXT("/DasTypes/")+ FMD5::HashAnsiString(*Id) + TEXT(".Type");
                    if (F.init)
                    {
                        bool Empty = false;
                        if (F.init->rtti_isCall())
                        {
                            auto* Init = static_cast<das::ExprCall*>(F.init);
                            Empty = Init->name == F.type->structType->name && Init->arguments.empty();
                        }
                        else if (F.init->rtti_isMakeStruct())
                        {
                            auto* Init = static_cast<das::ExprMakeStruct*>(F.init);
                            Empty = Init->structs.empty() && !Init->block;
                        }
                        if (!Empty)
                            DSLFail(TEXT("Nested ue_struct fields require an empty child constructor"));
                    }
                }
                if (!DSLMathPath(F.type).IsEmpty())
                {
                    P.Type.TypePath = DSLMathPath(F.type);
                    if (!F.init && P.Type.TypePath == TEXT("/Script/CoreUObject.Transform"))
                        F.init = DSLCall(F.at, "ue_transform_identity", {});
                    P.Default.Kind = TEXT("string");
                    P.Default.String = DSLMathPropertyDefault(F);
                }
                bool Marked = false;
                for (const auto& A : F.annotation)
                {
                    if (A.name == "ue_property")
                    {
                        Marked = true;
                        if (A.type == das::Type::tString && A.sValue == "BlueprintReadOnly")
                            P.bReadOnly = true;
                        else if (A.type == das::Type::tString &&
                                 (A.sValue == "EditAnywhere" || A.sValue == "BlueprintReadWrite"))
                        {
                        }
                        else if (A.type != das::Type::tBool || !A.bValue)
                            DSLFail(TEXT("Unsupported struct property annotation"));
                    }
                    else if (A.name == "ue_id")
                        P.Id = DSLText(A.sValue);
                    else if (DSLIsMetadata(A.name))
                        DSLMetadata(A, P.Metadata);
                    else
                        DSLFail(TEXT("Unknown struct field annotation"));
                }
                if (!Marked || !F.type || F.type->ref)
                    DSLFail(TEXT("Struct fields require ue_property and by-value scalar type"));
                if (P.Type.GetKind() == EDasTypeKind::Text)
                    P.Default = DSLTextDefault(F.init);
                else if (P.Type.GetKind() == EDasTypeKind::Byte)
                    P.Default = DSLByteDefault(F.init);
                else if (F.init && P.Type.GetKind() != EDasTypeKind::Struct && P.Type.GetKind() != EDasTypeKind::Enum)
                {
                    if (F.init->__rtti == das::string("ExprConstInt"))
                    {
                        P.Default.Kind = TEXT("number");
                        P.Default.Number = static_cast<das::ExprConstInt*>(F.init)->getValue();
                    }
                    else if (F.init->__rtti == das::string("ExprConstFloat"))
                    {
                        P.Default.Kind = TEXT("number");
                        P.Default.Number = static_cast<das::ExprConstFloat*>(F.init)->getValue();
                    }
                    else if (F.init->__rtti == das::string("ExprConstDouble"))
                    {
                        P.Default.Kind = TEXT("number");
                        P.Default.Number = static_cast<das::ExprConstDouble*>(F.init)->getValue();
                    }
                    else if (F.init->__rtti == das::string("ExprConstBool"))
                    {
                        P.Default.Kind = TEXT("bool");
                        P.Default.Boolean = static_cast<das::ExprConstBool*>(F.init)->getValue();
                    }
                    else if (F.init->__rtti == das::string("ExprConstString"))
                    {
                        P.Default.Kind = TEXT("string");
                        P.Default.String = DSLText(static_cast<das::ExprConstString*>(F.init)->text);
                    }
                    else
                        DSLFail(TEXT("Struct defaults require scalar literals"));
                }
                D.Properties.Add(P);
            }
            M.Structs.Add(D);
            FString Wire, Issue;
            if (!M.ToJson(Wire, Issue, true))
                DSLFail(Issue);
            Args.push_back(das::AnnotationArgument(DSLTypesKey, DSLUtf8(Wire)));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
