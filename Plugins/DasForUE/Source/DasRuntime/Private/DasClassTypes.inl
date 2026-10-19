das::ExprCall* DSLCall(const das::LineInfo&, const das::string&, std::initializer_list<das::ExpressionPtr>);
// Nominal, checked class values. Native Unreal paths live on the type, not on each function.
bool DSLTypedClass(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tStructure && T->structType &&
           DSLAnnotation(T->structType->annotations, "ue_class_ref");
}
FString DSLTypedClassPath(const das::TypeDeclPtr& T)
{
    if (!DSLTypedClass(T))
        DSLFail(TEXT("Expected typed class reference"));
    const auto* A = DSLAnnotation(T->structType->annotations, "ue_class_ref");
    const FString Path = DSLString(A->arguments, "path");
    auto* C = FindObject<UClass>(nullptr, *Path);
    if (!Path.StartsWith(TEXT("/Script/")) || !C ||
                         C->HasAnyClassFlags(CLASS_Interface | CLASS_Deprecated | CLASS_NewerVersionExists))
        DSLFail(TEXT("Typed class requires a loaded native non-interface base"));
    return Path;
}
das::ExpressionPtr DSLClassPack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V)
{
    return DSLCall(At, "__ue_class_check",
                   {new das::ExprField(At, V, "handle"), new das::ExprConstString(At, DSLUtf8(DSLTypedClassPath(T)))});
}
das::ExpressionPtr DSLClassUnpack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr H)
{
    auto* V = new das::ExprMakeStruct(At);
    V->makeType = DSLObjectCopyType(T);
    V->makeType->ref = false;
    V->makeType->constant = false;
    auto* Fields = new das::MakeStruct();
    Fields->push_back(new das::MakeFieldDecl(
        At, "handle", DSLCall(At, "__ue_class_check", {H, new das::ExprConstString(At, DSLUtf8(DSLTypedClassPath(T)))}),
        false, false));
    V->structs.push_back(Fields);
    return V;
}
das::ExpressionPtr DSLClassArgument(const das::LineInfo& At, das::ExpressionPtr E)
{
    return DSLTypedClass(E->type) ? DSLClassPack(At, E->type, E) : E;
}
struct FUEClassTypeAnnotation : das::StructureAnnotation
{
    FUEClassTypeAnnotation() : StructureAnnotation("ue_class_ref") {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (S->isClass || S->parent || S->fields.size() != 1 || S->fields[0].name != "handle" ||
                !S->fields[0].type || S->fields[0].type->ref || S->fields[0].type->baseType != das::Type::tHandle ||
                !S->fields[0].type->annotation || S->fields[0].type->annotation->name != "UEClassHandle" ||
                S->fields[0].init || Args.size() != 1 || Args[0].name != "path" || Args[0].type != das::Type::tString)
                DSLFail(TEXT("Typed class wrapper requires one UEClassHandle field and native path"));
            auto* T = new das::TypeDecl(das::Type::tStructure, S->at);
            T->structType = S;
            DSLTypedClassPath(T);
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
            DSLAnnotation(S->annotations, "ue_class_ref");
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEClassTypeMacro : das::TypeMacro
{
    FUEClassTypeMacro() : TypeMacro("UEClass") {}
    virtual das::TypeDeclPtr visit(das::Program* P, das::Module* M, const das::TypeDeclPtr& Type,
                                   const das::TypeDeclPtr& Pass) override
    {
        try
        {
            if (Type->typeMacroExpr.size() != 2 || !Type->typeMacroExpr[1]->rtti_isTypeDecl())
                DSLFail(TEXT("UEClass requires one generated native object type"));
            auto Base = DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(Type->typeMacroExpr[1])->typeexpr,
                                           *P->thisModuleGroup, M);
            if (!DSLTypedObject(Base))
                DSLFail(TEXT("UEClass requires a generated native object wrapper"));
            const FString Path = DSLTypedObjectPath(Base);
            auto* Native = FindObject<UClass>(nullptr, *Path);
            if (Native->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists))
                DSLFail(TEXT("UEClass base is obsolete"));
            const auto Name = DSLUtf8(TEXT("__ue_class_")+ FMD5::HashAnsiString(*Path));
            auto Existing = P->thisModuleGroup->findStructure(Name, M);
            if (auto Local = M->findStructure(Name))
            {
                bool Found = false;
                for (const auto& Candidate : Existing)
                    Found |= Candidate == Local;
                if (!Found)
                    Existing.push_back(Local);
            }
            if (Existing.size() > 1)
                DSLFail(TEXT("Ambiguous typed class declaration; import a shared alias module"));
            das::StructurePtr S = nullptr;
            if (Existing.size() == 1)
            {
                S = Existing[0];
                auto* Check = new das::TypeDecl(das::Type::tStructure, Type->at);
                Check->structType = S;
                if (!DSLTypedClass(Check) || DSLTypedClassPath(Check) != Path)
                    DSLFail(TEXT("Typed class generated name collision"));
            }
            else
            {
                S = new das::Structure();
                S->name = Name;
                S->module = M;
                S->at = Type->at;
                das::Structure::FieldDeclaration Field;
                Field.flags = 0;
                Field.name = "handle";
                Field.at = Type->at;
                auto Handles = P->thisModuleGroup->findAnnotation("UEClassHandle", M);
                if (Handles.size() != 1)
                    DSLFail(TEXT("Missing class handle type"));
                Field.type = new das::TypeDecl(das::Type::tHandle, Type->at);
                Field.type->annotation = static_cast<das::TypeAnnotation*>(Handles[0]);
                S->fields.push_back(Field);
                auto Marks = P->thisModuleGroup->findAnnotation("ue_class_ref", M);
                if (Marks.size() != 1)
                    DSLFail(TEXT("Missing typed class annotation"));
                auto* A = new das::AnnotationDeclaration();
                A->flags = 0;
                A->annotation = Marks[0];
                A->at = Type->at;
                A->arguments.push_back(das::AnnotationArgument("path", DSLUtf8(Path)));
                S->annotations.push_back(A);
                if (!M->addStructure(S, true))
                    DSLFail(TEXT("Typed class declaration collision"));
            }
            auto* Result = new das::TypeDecl(das::Type::tStructure, Type->at);
            Result->structType = S;
            das::TypeDecl::applyAutoContracts(Result, Type);
            if (Pass && (Pass->baseType != das::Type::tStructure || Pass->structType != S))
                return nullptr;
            return Result;
        }
        catch (const std::exception& X)
        {
            P->error("UEClass type macro failed", X.what(), "", Type->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
struct FUEClassCastFactory : das::CallMacro
{
    bool Raw;
    FUEClassCastFactory(bool R) : CallMacro(R ? "ue_class_handle" : "ue_class_cast"), Raw(R) {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int I) override { return Raw || I != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (Raw)
            {
                if (E->arguments.size() != 1)
                    DSLFail(TEXT("Expected ue_class_handle(value)"));
                auto T = E->arguments[0]->type;
                if (DSLTypedClass(T))
                    return DSLClassPack(E->at, T, E->arguments[0]);
                if (T && T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UEClassHandle")
                    return E->arguments[0];
                DSLFail(TEXT("Expected typed class or UEClassHandle"));
            }
            if (E->arguments.size() != 2 || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_class_cast(type<ClassType>,handle)"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (!DSLTypedClass(T))
                DSLFail(TEXT("ue_class_cast requires a typed class"));
            return DSLClassUnpack(E->at, T, DSLClassArgument(E->at, E->arguments[1]));
        }
        catch (const std::exception& X)
        {
            P->error("Typed class conversion failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
