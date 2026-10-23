// Source identities are module-relative, never absolute filenames or VM addresses.
FString DSLModuleIdentity(das::Module* M, const das::LineInfo& At)
{
    const FString Name = M ? DSLText(M->name) : FString();
    return Name.IsEmpty() ||
           Name == TEXT("_") ? FPaths::GetBaseFilename(At.fileInfo ? DSLText(At.fileInfo->name) : FString()) : Name;
}
FString DSLStateKey(das::Variable* V)
{
    const FString Module = V->module ? DSLText(V->module->name) : FString();
    const FString Scope = (Module.IsEmpty() || Module == TEXT("_")) ? TEXT("source:"): TEXT("module:");
    return Scope + DSLModuleIdentity(V->module, V->at) + TEXT("::") + DSLText(V->name);
}
das::TypeDeclPtr DSLIdentityType(das::Expression* E, das::ModuleGroup& G, das::Module* M)
{
    if (!E || !E->rtti_isTypeDecl())
        DSLFail(TEXT("Expected type<UnrealReference>"));
    auto T = DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E)->typeexpr, G, M);
    if (!DSLTypedReference(T) || DSLTypedInterface(T))
        DSLFail(TEXT("Expected an Unreal object reference type"));
    return T;
}
FString DSLIdentityPath(const das::TypeDeclPtr& T)
{
    return DSLAuthoredView(T) ? DSLAuthoredReferencePath(T) : DSLTypedReferencePath(T);
}
das::ExpressionPtr DSLLoctext(das::ExprCallMacro* E, das::Module* M)
{
    if (E->arguments.size() != 2 || !E->arguments[0]->rtti_isVar() ||
        E->arguments[1]->__rtti != das::string("ExprConstString"))
        DSLFail(TEXT("Expected ue_loctext(Key, \"source text\")"));
    const auto Key = static_cast<das::ExprVar*>(E->arguments[0])->name;
    const FString NS = DSLModuleIdentity(M, E->at);
    if (NS.IsEmpty() || NS.Len() > 1024 || Key.empty() || Key.size() > 1024)
        DSLFail(TEXT("Localization namespace/key must contain 1..1024 characters"));
    return DSLCall(
        E->at, "ue_text",
        {new das::ExprConstString(E->at, DSLUtf8(NS)), new das::ExprConstString(E->at, Key), E->arguments[1]});
}
struct FUEIdentityMacro : das::CallMacro
{
    bool Text;
    bool Subsystem;
    FUEIdentityMacro(bool InText, bool InSubsystem = false)
        : CallMacro(InText        ? "ue_loctext"
                    : InSubsystem ? "ue_subsystem"
                                  : "ue_class_default"),
          Text(InText), Subsystem(InSubsystem)
    {
    }
    virtual bool canVisitArguments(das::ExprCallMacro*, int I) override { return !Text && I == 1; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (Text)
                return DSLLoctext(E, M);
            if (E->arguments.empty() || E->arguments.size() > 2)
                DSLFail(TEXT("Expected ue_class_default(type<Reference>[, concreteClass])"));
            const auto T = DSLIdentityType(E->arguments[0], *P->thisModuleGroup, M);
            const bool Authored = DSLAuthoredView(T);
            const bool Concrete = E->arguments.size() == 2;
            if (Authored && !Concrete)
                DSLFail(TEXT("An authored reference is a contract; its CDO requires a concrete class handle"));
            auto* CDO =
                DSLCall(E->at, Subsystem ? "__ue_subsystem" : "__ue_class_default_object",
                        {new das::ExprConstString(E->at, DSLUtf8(DSLIdentityPath(T))),
                         Concrete ? DSLClassArgument(E->at, E->arguments[1]) : DSLCall(E->at, "ue_class_null", {}),
                         new das::ExprConstString(E->at, DSLUtf8(Authored ? DSLReferenceDeclaration(T).Id : FString())),
                         new das::ExprConstBool(E->at, Concrete)});
            return DSLAuthoredView(T) ? DSLReferenceValue(E->at, T, CDO)
                                      : DSLDirectUnpack(E->at, T, CDO, DSLIdentityPath(T));
        }
        catch (const std::exception& X)
        {
            P->error("Unreal identity macro failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
thread_local TSet<FString> DSLIdentityPathWarnings;
void DSLResetIdentityLint()
{
    DSLIdentityPathWarnings.Reset();
}
bool DSLIdentityFixture(const FString& File)
{
    const FString Base = FPaths::GetBaseFilename(File);
    return File.Contains(TEXT("/Tests/")) ||
                         File.Contains(TEXT("\\Tests\\")) ||
                                       Base.StartsWith(TEXT("ue_")) ||
                                                       Base.StartsWith(TEXT("dsl_")) ||
                                                                       Base.Contains(TEXT("fixture")) ||
                                                                                     Base.EndsWith(TEXT("_probe"));
}
TMap<FString, FString> DSLPathWrappers(das::ModuleGroup& G)
{
    TMap<FString, FString> Result;
    for (auto* M : G.getModules())
        M->structures.foreach (
            [&](das::Structure* S)
            {
                for (const char* Name : {"ue_native_object", "ue_native_interface", "ue_native_struct"})
                    if (const auto* A = DSLAnnotation(S->annotations, Name))
                    {
                        const auto Path = DSLString(A->arguments, "path");
                        if (!Path.IsEmpty() && !Result.Contains(Path))
                            Result.Add(Path, DSLText(M->name + "::" + S->name));
                    }
            });
    return Result;
}
struct FDSLPathLiteralLint : das::Visitor
{
    TMap<FString, FString> Wrappers;
    explicit FDSLPathLiteralLint(das::ModuleGroup& G) : Wrappers(DSLPathWrappers(G)) {}
    virtual das::ExpressionPtr visit(das::ExprConstString* E) override
    {
        const FString Path = DSLText(E->text);
        const auto* Wrapper = Wrappers.Find(Path);
        if (!Wrapper || !Path.StartsWith(TEXT("/Script/")) || !E->at.fileInfo ||
                                         DSLIdentityFixture(DSLText(E->at.fileInfo->name)))
            return E;
        // Generated transport literals carry their call's location, not a quoted source token.
        const char* Source = nullptr;
        uint32 Length = 0;
        E->at.fileInfo->getSourceAndLength(Source, Length);
        uint32 Pos = 0, Line = 1;
        while (Pos < Length && Line < E->at.line)
            if (Source[Pos++] == '\n')
                ++Line;
        Pos += E->at.column ? E->at.column - 1 : 0;
        const auto Bytes = DSLUtf8(Path);
        bool OriginalLiteral = false;
        for (int Shift : {-1, 0, 1})
        {
            const int64 At = int64(Pos) + Shift;
            if (At >= 0 && At + int64(Bytes.size()) + 2 <= Length && Source[At] == '"' &&
                FCStringAnsi::Strncmp(Source + At + 1, Bytes.c_str(), Bytes.size()) == 0 &&
                Source[At + Bytes.size() + 1] == '"')
                OriginalLiteral = true;
        }
        if (!OriginalLiteral)
            return E;
        const FString Location =
            DSLText(E->at.fileInfo->name) + FString::Printf(TEXT(":%u:%u"), E->at.line, E->at.column);
        if (!DSLIdentityPathWarnings.Contains(Location))
        {
            DSLIdentityPathWarnings.Add(Location);
            UE_LOG(LogDasRuntime, Warning,
                   TEXT("DAS_DSL_PATH_LITERAL %s: %s has typed wrapper %s; prefer type<T>/ue_ref/ue_class_default"),
                        *Location, *Path, **Wrapper);
        }
        return E;
    }
};
struct FUEPathLintPass : das::PassMacro
{
    FUEPathLintPass() : PassMacro("ue_literal_paths") {}
    virtual bool apply(das::Program* P, das::Module* M) override
    {
        FDSLPathLiteralLint L(*P->thisModuleGroup);
        M->functions.foreach (
            [&](das::Function* F)
            {
                if (F->body && !F->generated)
                    F->body->visit(L);
            });
        M->globals.foreach (
            [&](das::Variable* V)
            {
                if (V->init)
                    V->init->visit(L);
            });
        return false;
    }
};
// Declaration defaults are interpreted before ordinary call macros are inferred.
void DSLNormalizeIdentityDefaults(const das::StructurePtr& S, das::ModuleGroup& G)
{
    struct FNormalize : das::Visitor
    {
        das::Structure* S;
        das::ModuleGroup& G;
        bool ClassMarkers = true;
        FNormalize(das::Structure* InS, das::ModuleGroup& InG) : S(InS), G(InG) {}
        virtual das::ExpressionPtr visit(das::ExprCallMacro* E) override
        {
            if (E->name == "ue_loctext")
                return DSLLoctext(E, S->module);
            if (ClassMarkers && E->name == "ue_class_default")
            {
                if (E->arguments.size() != 1)
                    DSLFail(TEXT("Expected one class default type or literal path"));
                if (E->arguments[0]->rtti_isTypeDecl())
                {
                    auto T = DSLIdentityType(E->arguments[0], G, S->module);
                    if (DSLAuthoredView(T))
                        DSLFail(TEXT("Authored class defaults require an explicit Blueprint asset identity"));
                    return DSLCall(E->at, "ue_class_default",
                                   {new das::ExprConstString(E->at, DSLUtf8(DSLIdentityPath(T)))});
                }
                // Existing literal markers remain data; they are never runtime CDO calls.
                if (E->arguments[0]->__rtti != das::string("ExprConstString"))
                    DSLFail(TEXT("Class default requires a type or literal path"));
                return DSLCall(E->at, "ue_class_default", {E->arguments[0]});
            }
            return E;
        }
    } Normalize(S, G);
    FDSLPathLiteralLint Lint(G);
    for (auto& F : S->fields)
        if (!F.classMethod && F.init)
        {
            F.init->visit(Lint);
            F.init = F.init->visit(Normalize);
        }
    Normalize.ClassMarkers = false;
    S->module->functions.foreach (
        [&](das::Function* F)
        {
            if (F->classParent == S)
                for (auto* A : F->arguments)
                    if (A->init)
                        A->init = A->init->visit(Normalize);
        });
}
struct FUEStatePass : das::PassMacro
{
    FUEStatePass() : PassMacro("ue_persistent_globals") {}
    virtual bool apply(das::Program* P, das::Module* M) override
    {
        struct FLocalCheck : das::Visitor
        {
            das::Program* P;
            explicit FLocalCheck(das::Program* InP) : P(InP) {}
            virtual das::VariablePtr visitLet(das::ExprLet*, const das::VariablePtr& V, bool) override
            {
                if (V->annotation.find("ue_state", das::Type::tBool))
                    P->error("Persistent state declaration failed", "ue_state is only supported on module globals", "",
                             V->at, das::CompilationError::invalid_type);
                return V;
            }
        } LocalCheck(P);
        M->functions.foreach (
            [&](das::Function* F)
            {
                if (F->body)
                    F->body->visit(LocalCheck);
            });
        bool Changed = false;
        M->globals.foreach (
            [&](das::Variable* V)
            {
                const das::AnnotationArgument* A = nullptr;
                for (const auto& Mark : V->annotation)
                    if (Mark.name == "ue_state")
                        A = &Mark;
                if (!A)
                    return;
                try
                {
                    if (!DSLBool(*A) || V->global_shared || !V->type || V->type->constant || V->type->ref)
                        DSLFail(TEXT("ue_state requires a mutable, non-shared global with an explicit scalar type"));
                    const auto Kind = V->type->baseType;
                    if (Kind == das::Type::autoinfer || Kind == das::Type::alias)
                        return;
                    if (!DSLDefaultScalar(Kind))
                        DSLFail(TEXT("ue_state supports int, bool, float, double and string; object/VM references cannot persist"));
                    V->do_not_delete = true;
                    V->marked_used = true;
                    if (const auto* Done = V->annotation.find("_ue_state_lowered", das::Type::tBool))
                    {
                        if (Done->at.fileInfo)
                            DSLFail(TEXT("Reserved state compiler annotation"));
                        return;
                    }
                    if (V->init && !V->init->rtti_isConstant())
                        DSLFail(TEXT("ue_state initializer must be a scalar literal"));
                    das::ExpressionPtr Default = V->init;
                    das::string Suffix;
                    switch (Kind)
                    {
                    case das::Type::tInt:
                        Suffix = "int";
                        if (!Default)
                            Default = new das::ExprConstInt(V->at, 0);
                        break;
                    case das::Type::tBool:
                        Suffix = "bool";
                        if (!Default)
                            Default = new das::ExprConstBool(V->at, false);
                        break;
                    case das::Type::tFloat:
                        Suffix = "float";
                        if (!Default)
                            Default = new das::ExprConstFloat(V->at, 0);
                        break;
                    case das::Type::tDouble:
                        Suffix = "double";
                        if (!Default)
                            Default = new das::ExprConstDouble(V->at, 0);
                        break;
                    default:
                        Suffix = "string";
                        if (!Default)
                            Default = new das::ExprConstString(V->at, "");
                        break;
                    }
                    V->init = DSLCall(V->at, "__ue_state_init_" + Suffix,
                                      {new das::ExprConstString(V->at, DSLUtf8(DSLStateKey(V))), Default});
                    V->annotation.push_back(das::AnnotationArgument("_ue_state_lowered", true));
                    Changed = true;
                }
                catch (const std::exception& X)
                {
                    P->error("Persistent state declaration failed", X.what(), "", V->at,
                             das::CompilationError::invalid_type);
                }
            });
        return Changed;
    }
};
