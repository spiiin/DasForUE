// Universal property operators use compiler-resolved receiver types, not lexical guesses.
TMap<FString, FDasDeclaredClass> DSLActiveRefDeclarations;
TSet<FString> DSLRefSourceStack;
TMap<FString, FString> DSLSourceCatalogExpectedIds;
FDasDeclaredClass DSLReferenceDeclaration(const das::TypeDeclPtr&);
// Compile-chain descriptors hold names only: never AST pointers from a nested compiler.
struct FDSLMethodSourceSignature
{
    das::string Result;
    das::vector<das::string> Parameters;
    FDSLTypeScope Scope;
};
TMap<FString, FDSLMethodSourceSignature> DSLMethodSourceSignatures;
struct FDSLPropertySourceSignature
{
    das::string Name;
    FDSLTypeScope Scope;
};
TMap<FString, FDSLPropertySourceSignature> DSLPropertySourceSignatures;
void DSLResetIdentityLint();
void DSLResetInputNameLint();
void DSLResetReferenceDeclarations()
{
    DSLResetDocComments();
    DSLResetIdentityLint();
    DSLResetInputNameLint();
    DSLSourceCatalogExpectedIds.Reset();
    DSLActiveRefDeclarations.Reset();
    DSLMethodSourceSignatures.Reset();
    DSLPropertySourceSignatures.Reset();
}
das::string DSLMethodSourceName(const das::TypeDeclPtr& Type)
{
    if (Type && Type->baseType == das::Type::tUInt8)
        return "@byte";
    if (DSLObjectKey(Type))
        return das::string("@objectkey:") + Type->annotation->module->name + "::" + Type->annotation->name;
    if (DSLTypedClass(Type))
        return Type->structType->module->name + "::" + Type->structType->name;
    if (!DSLMathPath(Type).IsEmpty())
        return das::string("@math:") + Type->annotation->name;
    if (DSLTypedReference(Type))
        return Type->structType->module->name + "::" + Type->structType->name;
    if (DSLContainer(Type))
    {
        if (Type->baseType == das::Type::tTable && !DSLSet(Type))
            return das::string("@map:") + DSLMethodSourceName(Type->firstType) + "|" +
                   DSLMethodSourceName(Type->secondType);
        return das::string("@container:") + DSLMethodSourceName(Type->firstType);
    }
    if (DSLDirectEnum(Type))
        return Type->enumType->module->name + "::" + Type->enumType->name;
    if (DSLDelegate(Type))
        return Type->structType->module->name + "::" + Type->structType->name;
    if (DSLDirectStruct(Type))
        return Type->structType->module->name + "::" + Type->structType->name;
    return {};
}
void DSLRememberMethodSource(const FString& Key, FDasDeclaredMethod& Method, const das::FunctionPtr& Function,
                             const FDSLTypeScope& Scope)
{
    if (DSLDiagnosticActiveMap)
    {
        const FString Entity = DSLDiagnosticCurrent.Entity.Contains(
            TEXT(".")) ? DSLDiagnosticCurrent.Entity : DSLDiagnosticCurrent.Entity + TEXT(".")+ Method.Name;
        auto Origin = DSLDiagnosticLocation(Function->at, Entity);
        if (Function->body)
            Origin.EndLine = FMath::Max(Origin.EndLine, int32(Function->body->at.last_line));
        DSLDiagnosticActiveMap->Remember(Origin);
        DSLDiagnosticActiveMap->Symbols.Add(Method.EntryPoint, Origin);
        DSLDiagnosticActiveMap->Symbols.Add(TEXT("__impl_")+ Method.EntryPoint, Origin);
    }
    FDSLMethodSourceSignature Descriptor;
    Descriptor.Scope = {Scope.ModuleId, Scope.Folder, Scope.Registry, nullptr, nullptr};
    Descriptor.Result = DSLMethodSourceName(Function->result);
    if (DSLAuthoredView(Function->result))
        Method.ReturnType.AuthoredId = DSLReferenceDeclaration(Function->result).Id;
    for (size_t Index = 1; Index < Function->arguments.size(); ++Index)
    {
        Descriptor.Parameters.push_back(DSLMethodSourceName(Function->arguments[Index]->type));
        if (DSLAuthoredView(Function->arguments[Index]->type))
            Method.Parameters[int32(Index) - 1].Type.AuthoredId =
                DSLReferenceDeclaration(Function->arguments[Index]->type).Id;
    }
    DSLMethodSourceSignatures.Add(Key + TEXT("::")+ Method.Name, MoveTemp(Descriptor));
}

void DSLRememberPropertySources(const FString& Key, const das::StructurePtr& Source, const FDasDeclaredClass& Class,
                                const FDSLTypeScope& Scope)
{
    for (const auto& Property : Class.Properties)
    {
        const auto* Field = Source->findField(DSLUtf8(Property.Name));
        if (!Field)
            DSLFail(TEXT("Missing authored property source field"));
        FDSLPropertySourceSignature Descriptor;
        Descriptor.Name = DSLMethodSourceName(Field->type);
        Descriptor.Scope = {Scope.ModuleId, Scope.Folder, Scope.Registry, nullptr, nullptr};
        DSLPropertySourceSignatures.Add(Key + TEXT("::")+ Property.Name, MoveTemp(Descriptor));
    }
}
int32 DSLDeclarationCompileDepth = 0;
struct FDSLDeclarationCompileScope
{
    FDSLDeclarationCompileScope()
    {
        if (DSLDeclarationCompileDepth++ == 0)
            DSLResetReferenceDeclarations();
    }
    ~FDSLDeclarationCompileScope()
    {
        if (--DSLDeclarationCompileDepth == 0)
            DSLResetReferenceDeclarations();
    }
};
void EnsureDasNativeModules(); // DasNativeBinds.inl
auto DSLCompile(const char* File, const das::FileAccessPtr& Access, das::TextWriter& Output, das::ModuleGroup& Group,
                const das::CodeOfPolicies& Policies = das::CodeOfPolicies())
{
    // Native modules registered by code modules since the last compilation become requirable now.
    EnsureDasNativeModules();
    FDSLDeclarationCompileScope Scope;
    FDSLDiagnosticCompileScope Diagnostics(Group);
    FDSLComputedDefaults Computed;
    TGuardValue<FDSLComputedDefaults*> Active(DSLComputedDefaults, &Computed);
    const auto ReferenceBefore = DSLActiveRefDeclarations;
    const auto MethodsBefore = DSLMethodSourceSignatures;
    const auto PropertiesBefore = DSLPropertySourceSignatures;
    const auto CatalogBefore = DSLSourceCatalogExpectedIds;
    auto Program = das::compileDaScript(File, Access, Output, Group, Policies);
    DSLDiagnosticGenerated(Program);
    if (!Program || Program->failed() || Computed.Pending.IsEmpty())
        return Program;
    DSLEvaluateComputedDefaults(Program, Output);
    Computed.Resolved = true;
    Computed.SourceFields.Reset();
    // Required modules cache their annotation models in ModuleGroup. Reusing the
    // collection modules would publish placeholder defaults in authored parents.
    // Every caller supplies a fresh concrete ModuleGroup; rebuild it for pass two.
    for (auto& V : Computed.Values)
        V.Value->at = das::LineInfo();
    Computed.Pending.Reset();
    Computed.PendingReferences.Reset();
    Computed.PendingNativeExpressions.Reset();
    Program = nullptr;
    Group.~ModuleGroup();
    new (&Group) das::ModuleGroup();
    DSLActiveRefDeclarations = ReferenceBefore;
    DSLMethodSourceSignatures = MethodsBefore;
    DSLPropertySourceSignatures = PropertiesBefore;
    DSLSourceCatalogExpectedIds = CatalogBefore;
    Diagnostics.RebuiltGroup();
    auto Resolved = das::compileDaScript(File, Access, Output, Group, Policies);
    DSLDiagnosticGenerated(Resolved);
    return Resolved;
}

struct FUEAuthoredRefAnnotation : das::StructureAnnotation
{
    FUEAuthoredRefAnnotation() : StructureAnnotation("ue_object_ref") {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            DSLAnnotationArguments(Args);
            if (S->isClass || S->parent || S->fields.size() != 1 || S->fields[0].name != "handle" ||
                !S->fields[0].type || S->fields[0].type->ref || S->fields[0].type->baseType != das::Type::tHandle ||
                !S->fields[0].type->annotation || S->fields[0].type->annotation->name != "UObjectHandle" ||
                S->fields[0].init)
                DSLFail(TEXT("Authored reference requires one UObjectHandle field"));
            for (const auto& A : Args)
                if (((A.name == "_ue_catalog_id" || A.name == "_ue_super_id")
                         ? A.at.fileInfo != nullptr
                         : (A.name != "script" && A.name != "type" && A.name != "catalog")) ||
                    A.type != das::Type::tString)
                    DSLFail(TEXT("Expected ue_object_ref(type=ClassNameOrId, optional script or catalog)"));
            FString File = DSLString(Args, "script"), Name = DSLString(Args, "type"),
                    CatalogFile = DSLString(Args, "catalog");
            if (Name.IsEmpty())
                DSLFail(TEXT("Authored reference requires type"));
            if (!File.IsEmpty() && !CatalogFile.IsEmpty())
                DSLFail(TEXT("Use either explicit script or source catalog"));
            if (File.IsEmpty())
            {
                if (CatalogFile.IsEmpty())
                    CatalogFile = TEXT("ue_type_catalog.json");
                if (!FDasSourceCatalog::LocalFile(CatalogFile, TEXT("json")))
                    DSLFail(TEXT("Expected local source catalog JSON filename"));
                auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
                const FString Path = R.ScriptPath(CatalogFile);
                auto Snapshot = FSourceSnapshot::Read(Path);
                if (AuthoredReferenceSources)
                    AuthoredReferenceSources->Add(Path, Snapshot);
                FString Wire, Issue;
                FDasSourceCatalog Catalog;
                FDasSourceCatalogEntry Entry;
                if (!Snapshot.bExists || Snapshot.Bytes.Num() > 1024 * 1024)
                    DSLFail(TEXT("Missing/oversized source catalog: ")+ Path);
                FFileHelper::BufferToString(Wire, Snapshot.Bytes.GetData(), Snapshot.Bytes.Num());
                if (!FDasSourceCatalog::FromJson(Wire, Catalog, Issue) || !Catalog.Resolve(Name, Entry, Issue))
                    DSLFail(Issue);
                File = Entry.Script;
                Name = Entry.Name;
                const FString Key = File + TEXT("::")+ Name;
                if (const auto* Prior = DSLSourceCatalogExpectedIds.Find(Key))
                    if (*Prior != Entry.Id)
                        DSLFail(TEXT("Conflicting source catalogs for authored class: ")+ Key);
                DSLSourceCatalogExpectedIds.Add(Key, Entry.Id);
                // Canonicalize once so every existing accessor/component source-key consumer agrees.
                bool HadScript = false;
                for (auto& A : Args)
                {
                    if (A.name == "script")
                    {
                        A.sValue = DSLUtf8(File);
                        HadScript = true;
                    }
                    if (A.name == "type")
                        A.sValue = DSLUtf8(Name);
                }
                if (!HadScript)
                    Args.emplace_back(das::AnnotationArgument("script", DSLUtf8(File)));
                Args.emplace_back(das::AnnotationArgument("_ue_catalog_id", DSLUtf8(Entry.Id)));
                // catalog has served its purpose; resolved source and the dependency record remain.
                for (auto I = Args.begin(); I != Args.end();)
                    if (I->name == "catalog")
                        I = Args.erase(I);
                    else
                        ++I;
            }
            if (!FDasSourceCatalog::LocalFile(File, TEXT("das")))
                DSLFail(TEXT("Expected local authored source filename"));
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
            DSLAnnotation(S->annotations, "ue_object_ref");
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};

bool DSLAuthoredView(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tStructure && T->structType &&
           DSLAnnotation(T->structType->annotations, "ue_object_ref");
}
FDasDeclaredClass DSLReferenceDeclaration(const das::TypeDeclPtr& T)
{
    const auto* A = DSLAnnotation(T->structType->annotations, "ue_object_ref");
    const FString File = DSLString(A->arguments, "script"), Name = DSLString(A->arguments, "type"),
                  Key = File + TEXT("::")+ Name;
    auto CheckIdentity = [&](const FDasDeclaredClass& C)
    {
        const FString CatalogId = DSLString(A->arguments, "_ue_catalog_id");
        if (!CatalogId.IsEmpty() && CatalogId != C.Id)
            DSLFail(TEXT("Source catalog stable ID differs from authored declaration: ")+ Key);
        if (const auto* Expected = DSLSourceCatalogExpectedIds.Find(Key))
            if (C.Id != *Expected)
                DSLFail(TEXT("Source catalog stable ID differs from authored declaration: ")+ Key);
        return C;
    };
    if (const auto* Found = DSLActiveRefDeclarations.Find(Key))
        return CheckIdentity(*Found);
    if (DSLRefSourceStack.Contains(Key))
        DSLFail(TEXT("Authored reference source dependency cycle"));
    DSLRefSourceStack.Add(Key);
    ON_SCOPE_EXIT
    {
        DSLRefSourceStack.Remove(Key);
    };
    auto& R = FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));
    FDasAuthoringModule Model;
    FString Error;
    if (!R.ReadAuthoringScript(R.ScriptPath(File), Model, Error))
        DSLFail(Error);
    const auto* C = Model.Classes.FindByPredicate([&](const auto& Item) { return Item.Name == Name; });
    if (!C)
        DSLFail(TEXT("Unknown authored reference class: ")+ Name);
    FDasDeclaredClass Effective;
    if (!Model.ResolveClass(C->Id, Effective, Error))
        DSLFail(Error);
    return CheckIdentity(Effective);
}
FString DSLAuthoredReferencePath(const das::TypeDeclPtr& Type)
{
    const auto Class = DSLReferenceDeclaration(Type);
    if (DasActorNativeParent(Class.NativeParent))
        return DasNativeScriptHost(Class.NativeParent)->GetPathName();
    if (Class.NativeParent == TEXT("/Script/Engine.ActorComponent"))
        return TEXT("/Script/DasRuntime.DasScriptComponent");
    if (Class.NativeParent == TEXT("/Script/Engine.WorldSubsystem") ||
                                   Class.NativeParent == TEXT("/Script/Engine.GameInstanceSubsystem"))
        return DasNativeScriptHost(Class.NativeParent)->GetPathName();
    DSLFail(TEXT("Authored signature reference requires an Actor/Component/Subsystem host"));
}
das::ExpressionPtr DSLAuthoredPack(const das::LineInfo& At, const das::TypeDeclPtr& Type, das::ExpressionPtr Value)
{
    const auto Class = DSLReferenceDeclaration(Type);
    return DSLCall(At, "__ue_authored_nullable_check",
                   {new das::ExprField(At, Value, "handle"), new das::ExprConstString(At, DSLUtf8(Class.Id))});
}
das::ExpressionPtr DSLAuthoredUnpack(const das::LineInfo& At, const das::TypeDeclPtr& Type, das::ExpressionPtr Handle)
{
    const auto Class = DSLReferenceDeclaration(Type);
    auto* Value = new das::ExprMakeStruct(At);
    Value->makeType = DSLObjectCopyType(Type);
    Value->makeType->ref = false;
    Value->makeType->constant = false;
    auto* Fields = new das::MakeStruct();
    Fields->push_back(new das::MakeFieldDecl(
        At, "handle",
        DSLCall(At, "__ue_authored_nullable_check", {Handle, new das::ExprConstString(At, DSLUtf8(Class.Id))}), false,
        false));
    Value->structs.push_back(Fields);
    return Value;
}
das::ExpressionPtr DSLReferenceValue(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr H)
{
    if (!DSLAuthoredView(T))
        return DSLDirectUnpack(At, T, H, DSLTypedReferencePath(T));
    auto C = DSLReferenceDeclaration(T);
    auto* V = new das::ExprMakeStruct(At);
    V->makeType = DSLObjectCopyType(T);
    auto* Fields = new das::MakeStruct();
    Fields->push_back(new das::MakeFieldDecl(
        At, "handle", DSLCall(At, "__ue_authored_object_check", {H, new das::ExprConstString(At, DSLUtf8(C.Id))}),
        false, false));
    V->structs.push_back(Fields);
    return V;
}
bool DSLClassHandle(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == "UEClassHandle";
}
struct FUEClassLoadedFactory : das::CallMacro
{
    FUEClassLoadedFactory() : CallMacro("ue_class_loaded") {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int Index) override { return Index != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (E->arguments.size() != 2 || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_class_loaded(type<ObjectRef>, path)"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (DSLTypedClass(T))
            {
                const FString Base = DSLTypedClassPath(T);
                return DSLClassUnpack(E->at, T,
                                      DSLCall(E->at, "__ue_class_loaded",
                                              {E->arguments[1], new das::ExprConstString(E->at, DSLUtf8(Base)),
                                               new das::ExprConstString(E->at, "")}));
            }
            if (!DSLTypedReference(T) || DSLTypedInterface(T))
                DSLFail(TEXT("Class reference requires native or authored object type"));
            const FString Base = DSLTypedReferencePath(T);
            const FString Id = DSLAuthoredView(T) ? DSLReferenceDeclaration(T).Id : FString();
            return DSLCall(E->at, "__ue_class_loaded",
                           {E->arguments[1], new das::ExprConstString(E->at, DSLUtf8(Base)),
                            new das::ExprConstString(E->at, DSLUtf8(Id))});
        }
        catch (const std::exception& X)
        {
            P->error("Class reference resolution failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
#include "DasObjectQueryMacros.inl"
#include "DasDataTableDSL.inl"
struct FUEActorSpawnFactory : das::CallMacro
{
    FUEActorSpawnFactory() : CallMacro("ue_actor_spawn") {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int Index) override { return Index != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (E->arguments.size() < 2 || E->arguments.size() > 5 || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_actor_spawn(type<ActorRef>, classHandle[, transform, owner, collision])"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (!DSLTypedReference(T) || DSLTypedInterface(T))
                DSLFail(TEXT("Actor spawn requires native or authored Actor reference"));
            const FString Base = DSLTypedReferencePath(T);
            auto* Class = FindObject<UClass>(nullptr, *Base);
            if (!Class || !Class->IsChildOf(AActor::StaticClass()))
                DSLFail(TEXT("Expected Actor reference"));
            const FString Id = DSLAuthoredView(T) ? DSLReferenceDeclaration(T).Id : FString();
            auto* Call = DSLCall(
                E->at, "__ue_actor_spawn",
                {DSLClassArgument(E->at, E->arguments[1]),
                 E->arguments.size() > 2 ? E->arguments[2]
                                         : static_cast<das::ExpressionPtr>(DSLCall(E->at, "ue_transform_identity", {})),
                 E->arguments.size() > 3
                     ? static_cast<das::ExpressionPtr>(new das::ExprField(E->at, E->arguments[3], "handle"))
                     : static_cast<das::ExpressionPtr>(DSLCall(
                           E->at, "ue_object_value", {new das::ExprConstString(E->at, "/Script/Engine.Actor")})),
                 E->arguments.size() > 4
                     ? E->arguments[4]
                     : static_cast<das::ExpressionPtr>(new das::ExprConstString(E->at, "AlwaysSpawn")),
                 new das::ExprConstString(E->at, DSLUtf8(Base)), new das::ExprConstString(E->at, DSLUtf8(Id))});
            return DSLAuthoredView(T) ? DSLAuthoredUnpack(E->at, T, Call) : DSLDirectUnpack(E->at, T, Call, Base);
        }
        catch (const std::exception& X)
        {
            P->error("Actor spawn factory failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
struct FUENativeComponentFactory : das::CallMacro
{
    int Mode;
    FUENativeComponentFactory(int V)
        : CallMacro(V == 0   ? "ue_component_get"
                    : V == 1 ? "ue_component_create"
                             : "ue_component_get_or_create"),
          Mode(V)
    {
    }
    virtual bool canVisitArguments(das::ExprCallMacro*, int Index) override { return Index != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if ((E->arguments.size() != 3 && E->arguments.size() != 4) || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected component operation(type<ComponentRef>, actor, name[, generatedClassPath])"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (DSLAuthoredView(T))
            {
                const auto Declaration = DSLReferenceDeclaration(T);
                if (E->arguments.size() != (Mode == 0 ? 3 : 4))
                    DSLFail(TEXT("Authored create/get_or_create requires explicit generated class path"));
                if (Declaration.NativeParent != TEXT("/Script/Engine.ActorComponent"))
                    DSLFail(TEXT("Expected authored ActorComponent reference"));
                auto* Call = Mode == 0
                                 ? DSLCall(E->at, "__ue_authored_component_get",
                                           {new das::ExprField(E->at, E->arguments[1], "handle"),
                                            new das::ExprConstString(E->at, DSLUtf8(Declaration.Id)), E->arguments[2]})
                                 : DSLCall(E->at, "__ue_class_component_operation",
                                           {new das::ExprField(E->at, E->arguments[1], "handle"),
                                            DSLClassArgument(E->at, E->arguments[3]), E->arguments[2],
                                            new das::ExprConstInt(E->at, Mode),
                                            new das::ExprConstString(E->at, DSLUtf8(DSLTypedReferencePath(T))),
                                            new das::ExprConstString(E->at, DSLUtf8(Declaration.Id))});
                return DSLAuthoredUnpack(E->at, T, Call);
            }
            if (!DSLTypedReference(T) || DSLAuthoredView(T) || DSLTypedInterface(T))
                DSLFail(TEXT("Component operation requires a native object reference"));
            if (Mode == 0 && E->arguments.size() != 3)
                DSLFail(TEXT("Component get does not take a class argument"));
            const FString Path = DSLTypedReferencePath(T);
            auto* Class = FindObject<UClass>(nullptr, *Path);
            if (!Path.StartsWith(TEXT("/Script/")) || !Class || !Class->IsChildOf(UActorComponent::StaticClass()) ||
                                 Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
                DSLFail(TEXT("Expected concrete native ActorComponent reference"));
            auto* Call = DSLCall(E->at, "__ue_class_component_operation",
                                 {new das::ExprField(E->at, E->arguments[1], "handle"),
                                  E->arguments.size() == 4
                                      ? DSLClassArgument(E->at, E->arguments[3])
                                      : static_cast<das::ExpressionPtr>(new das::ExprConstString(E->at, DSLUtf8(Path))),
                                  E->arguments[2], new das::ExprConstInt(E->at, Mode),
                                  new das::ExprConstString(E->at, DSLUtf8(Path)), new das::ExprConstString(E->at, "")});
            return DSLDirectUnpack(E->at, T, Call, Path);
        }
        catch (const std::exception& X)
        {
            P->error("Native component factory failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
struct FUEReferenceFactory : das::CallMacro
{
    bool Owner;
    FUEReferenceFactory(bool O) : CallMacro(O ? "ue_owner" : "ue_ref"), Owner(O) {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int Index) override { return Index != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (E->arguments.size() != (Owner ? 1 : 2) || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(Owner ? TEXT("Expected ue_owner(type<Reference>)")
                              : TEXT("Expected ue_ref(type<Reference>, handle)"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (!Owner && DSLTypedReference(T) && !DSLAuthoredView(T))
                return DSLDirectUnpack(E->at, T, E->arguments[1], DSLTypedReferencePath(T));
            if (!DSLAuthoredView(T))
                DSLFail(Owner ? TEXT("ue_owner requires an authored reference type")
                              : TEXT("ue_ref requires an authored reference type"));
            if (Owner && !DasActorNativeParent(DSLReferenceDeclaration(T).NativeParent))
                DSLFail(TEXT("ue_owner reference must describe an Actor"));
            return Owner ? DSLReferenceValue(
                               E->at, T, DSLCall(E->at, "ue_component_owner", {DSLCall(E->at, "ue_self_object", {})}))
                         : DSLAuthoredUnpack(E->at, T, E->arguments[1]);
        }
        catch (const std::exception& X)
        {
            P->error("Authored reference factory failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
das::TypeDeclPtr DSLPropertyType(const FString& Kind, das::ModuleGroup& G, das::Module* M, const das::LineInfo& At)
{
    if (Kind == TEXT("class"))
        return DSLHandleType(G, M, "UEClassHandle");
    if (Kind == TEXT("object"))
        return DSLHandleType(G, M, "UObjectHandle");
    if (Kind == TEXT("struct"))
        return DSLHandleType(G, M, "UEStructValue");
    if (Kind == TEXT("enum"))
        return DSLHandleType(G, M, "UEEnumValue");
    if (Kind == TEXT("text"))
        return DSLHandleType(G, M, "UETextValue");
    if (Kind == TEXT("name"))
        return DSLHandleType(G, M, "UENameValue");
    return new das::TypeDecl(
        (Kind == TEXT("int") || Kind == TEXT("byte"))
         ? das::Type::tInt
         : Kind == TEXT("float")
                        ? das::Type::tFloat
                        : Kind == TEXT("double")? das::Type::tDouble
                                                : Kind == TEXT("bool")? das::Type::tBool
                                                                      : Kind == TEXT("string")? das::Type::tString
                                                                                              : das::Type::tVoid,
                                                                                     At);
}
das::TypeDeclPtr DSLMethodValueType(das::Program*, das::Module*, const FDasDeclaredType&, const das::string&,
                                    const FDSLTypeScope&, const das::LineInfo&);
#include "DasNativeGameplayDSL.inl"
bool DSLEmitProperty(das::Program* P, das::Module* M, const das::TypeDeclPtr& T, const das::string& Name,
                     const das::LineInfo& At)
{
    const das::string Getter = ".`" + Name;
    bool Exists = false;
    P->library.foreach (
        [&](das::Module* Mod)
        {
            Mod->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->name == Getter && !F->arguments.empty() &&
                        F->arguments[0]->type->structType == T->structType)
                        Exists = true;
                });
            return true;
        },
        "*");
    if (Exists)
        return false;
    const bool Native = !DSLAuthoredView(T);
    FDasDeclaredClass C;
    if (Native)
    {
        C.Id = DSLTypedReferencePath(T);
        C.Properties.Add(DSLNativeReferenceProperty(T, Name));
    }
    else
        C = DSLReferenceDeclaration(T);
    const auto* Prop = C.Properties.FindByPredicate([&](const auto& V) { return V.Name == DSLText(Name); });
    const auto* Component = C.Components.FindByPredicate([&](const auto& V) { return V.Name == DSLText(Name); });
    if (!Prop && !Component)
        DSLFail(TEXT("Unknown authored object field: ")+ DSLText(Name));
    das::TypeDeclPtr ValueType = nullptr;
    FDasDeclaredProperty ComponentProperty;
    const auto* Reference = DSLAnnotation(T->structType->annotations, "ue_object_ref");
    const FString SourceKey =
        Reference ? DSLString(Reference->arguments, "script") + TEXT("::")+ DSLString(Reference->arguments, "type") +
                                                                     TEXT("::")+ DSLText(Name) : FString();
    const auto* Descriptor = DSLPropertySourceSignatures.Find(SourceKey);
    if (Component)
    {
        P->library.foreach (
            [&](das::Module* Mod)
            {
                Mod->structures.foreach (
                    [&](const das::StructurePtr& Structure)
                    {
                        // Compiler views describe self/Super dispatch, not public component reference types.
                        if (Structure->name.find("__ue_self_events_") == 0 || Structure->name.find("__ue_super_") == 0)
                            return;
                        const auto* Annotation = DSLAnnotation(Structure->annotations, "ue_object_ref");
                        if (Component->NativeClass.IsEmpty())
                        {
                            if (!Annotation || DSLString(Annotation->arguments, "script") != Component->ScriptFile ||
                                DSLString(Annotation->arguments, "type") != Component->ClassName)
                                return;
                        }
                        else
                        {
                            const auto* Native = DSLAnnotation(Structure->annotations, "ue_native_object");
                            if (!Native || DSLString(Native->arguments, "path") != Component->NativeClass)
                                return;
                        }
                        if (ValueType && ValueType->structType != Structure)
                            DSLFail(
                                TEXT("Ambiguous component reference view; import only the declared component view"));
                        ValueType = new das::TypeDecl(das::Type::tStructure, At);
                        ValueType->structType = Structure;
                    });
                return true;
            },
            "*");
        if (!ValueType)
            DSLFail(TEXT("Import the component reference type before accessing its member"));
        ComponentProperty.Name = Component->Name;
        ComponentProperty.Type.Kind = TEXT("object");
        ComponentProperty.Type.TypePath = Component->NativeClass;
        ComponentProperty.bReadOnly = true;
        Prop = &ComponentProperty;
    }
    else if (Native)
        ValueType = DSLNativeValueType(P, M, Prop->Type, At);
    else if (Descriptor && !Descriptor->Name.empty())
        ValueType = DSLMethodValueType(P, M, Prop->Type, Descriptor->Name, Descriptor->Scope, At);
    else if (auto View = Prop->Type.GetKind() == EDasTypeKind::Struct
                             ? DSLAdaptedView(Prop->Type.TypePath, EDasScriptViewUse::AuthoredReference,
                                              *P->thisModuleGroup, M, At)
                             : nullptr)
        ValueType = View;
    else
        ValueType = DSLPropertyType(Prop->Type.Kind, *P->thisModuleGroup, M, At);
    if (ValueType->baseType == das::Type::tVoid)
        DSLFail(TEXT("Unsupported authored property kind: ")+ Prop->Type.Kind);
    const bool Container = DSLContainer(ValueType);
    const FString ValuePath = Container ? DasContainerCarrier(Prop->Type)->GetPathName() : Prop->Type.TypePath;
    const FString RuntimeKind = Container ? TEXT("struct"): Prop->Type.Kind;
    const bool Convert = (ValueType->baseType == das::Type::tUInt8) || DSLTypedClass(ValueType) || Container ||
                         DSLDirectStruct(ValueType) || DSLDirectEnum(ValueType) || DSLTypedReference(ValueType) ||
                         !DSLMathPath(ValueType).IsEmpty();
    if (DSLDirectStruct(ValueType) || Container)
    {
        das::vector<das::FunctionPtr> Helpers;
        TSet<FString> Paths;
        DSLEnsureStructHelpers(ValueType, ValuePath, *P->thisModuleGroup, M, Helpers, Paths);
        for (const auto& Helper : Helpers)
            if (!M->addFunction(Helper, true))
                DSLFail(TEXT("Authored property source helper collision"));
    }
    auto Handle = [&]()
    {
        return new das::ExprField(At, new das::ExprVar(At, "receiver"), "handle");
    };
    auto Read = [&]() -> das::ExpressionPtr
    {
        if (Prop->Type.Kind == TEXT("delegate"))
            return DSLDelegateFieldRead(
                At, ValueType,
                DSLCall(At, "__ue_authored_object_check", {Handle(), new das::ExprConstString(At, DSLUtf8(C.Id))}),
                Prop->Name);
        if (Prop->Type.Kind == TEXT("event"))
            return DSLDelegateView(
                At, ValueType,
                DSLCall(At, "__ue_authored_object_check", {Handle(), new das::ExprConstString(At, DSLUtf8(C.Id))}),
                Prop->Name);
        auto* Value =
            DSLCall(At, DSLUtf8((Native ? TEXT("__ue_native_get_"): TEXT("__ue_authored_get_")) + RuntimeKind),
                                {Handle(), new das::ExprConstString(At, DSLUtf8(C.Id)),
                                 new das::ExprConstString(At, DSLUtf8(Prop->Name))});
        if (Prop->Type.Kind == TEXT("object"))
            Value->arguments.push_back(new das::ExprConstString(At, DSLUtf8(Prop->Type.TypePath)));
        return Component ? DSLReferenceValue(At, ValueType, Value)
               : Convert ? DSLDirectUnpack(At, ValueType, Value, ValuePath)
                         : Value;
    };
    auto Write = [&](das::ExpressionPtr Value)
    {
        if (Prop->Type.Kind == TEXT("delegate"))
            return static_cast<das::ExprCall*>(DSLDelegateFieldWrite(
                At, ValueType,
                DSLCall(At, "__ue_authored_object_check", {Handle(), new das::ExprConstString(At, DSLUtf8(C.Id))}),
                Prop->Name, Value));
        auto* Call = DSLCall(At, DSLUtf8((Native ? TEXT("__ue_native_set_"): TEXT("__ue_authored_set_")) + RuntimeKind),
                                         {Handle(), new das::ExprConstString(At, DSLUtf8(C.Id)),
                                          new das::ExprConstString(At, DSLUtf8(Prop->Name)),
                                          Convert ? DSLDirectPack(At, ValueType, Value, ValuePath) : Value});
        if (Prop->Type.Kind == TEXT("object"))
            Call->arguments.insert(Call->arguments.begin() + 3,
                                   new das::ExprConstString(At, DSLUtf8(Prop->Type.TypePath)));
        return Call;
    };
    auto Add = [&](const das::string& Suffix, das::ExpressionPtr Body, bool Setter)
    {
        auto* F = new das::Function();
        F->at = At;
        F->name = Getter + Suffix;
        F->generated = true;
        F->privateFunction = false;
        F->isCustomProperty = true;
        F->result = Setter ? new das::TypeDecl(das::Type::tVoid, At) : DSLObjectCopyType(ValueType);
        auto* Receiver = new das::Variable();
        Receiver->at = At;
        Receiver->name = "receiver";
        Receiver->type = DSLObjectCopyType(T);
        Receiver->type->ref = false;
        Receiver->type->constant = true;
        F->arguments.push_back(Receiver);
        if (Setter)
        {
            auto* Value = new das::Variable();
            Value->at = At;
            Value->name = "value";
            Value->type = DSLObjectCopyType(ValueType);
            Value->type->ref = false;
            Value->type->constant = true;
            F->arguments.push_back(Value);
        }
        auto* Block = new das::ExprBlock();
        Block->at = At;
        if (Setter)
            Block->list.push_back(Body);
        else
        {
            auto* Return = new das::ExprReturn(At, Body);
            Return->moveSemantics = DSLDirectStruct(ValueType) || Container;
            Block->list.push_back(Return);
        }
        F->body = Block;
        if (!M->addFunction(F, true))
            DSLFail(TEXT("Duplicate authored property accessor"));
    };
    Add("", Read(), false);
    if (!Prop->bReadOnly && Prop->Type.Kind != TEXT("event"))
    {
        Add("`clone", Write(new das::ExprVar(At, "value")), true);
        if (Prop->Type.Kind == TEXT("int") || Prop->Type.Kind == TEXT("float") || Prop->Type.Kind == TEXT("double"))
            for (const auto* Op : {"+", "-", "*", "/"})
                Add(das::string("`") + Op + "=", Write(new das::ExprOp2(At, Op, Read(), new das::ExprVar(At, "value"))),
                    true);
    }
    return true;
}
das::TypeDeclPtr DSLMethodValueType(das::Program* Program, das::Module* Module, const FDasDeclaredType& Declared,
                                    const das::string& SourceName, const FDSLTypeScope& Scope, const das::LineInfo& At)
{
    if (SourceName == "@byte")
    {
        if (Declared.Kind != TEXT("byte"))
            DSLFail(TEXT("Byte source identity mismatch"));
        return new das::TypeDecl(das::Type::tUInt8, At);
    }
    if (SourceName.find("@objectkey:") == 0)
    {
        auto Matches = Program->thisModuleGroup->findAnnotation(SourceName.substr(11), Module);
        if (Matches.size() != 1)
            DSLFail(TEXT("Import the typed object key module"));
        auto* Type = new das::TypeDecl(das::Type::tHandle, At);
        Type->annotation = static_cast<das::TypeAnnotation*>(Matches[0]);
        const auto* A = DSLKeyAnnotation(Type);
        if (Declared.Kind != TEXT("object") || A->ClassPath != Declared.TypePath ||
                                  A->AuthoredId != Declared.AuthoredId)
            DSLFail(TEXT("Object key source identity mismatch"));
        return Type;
    }
    if (Declared.Kind == TEXT("class") && SourceName.empty())
        return DSLHandleType(*Program->thisModuleGroup, Module, "UEClassHandle");
    if (Declared.Kind == TEXT("array") || Declared.Kind == TEXT("map") || Declared.Kind == TEXT("set"))
    {
        auto* Container = new das::TypeDecl(Declared.Kind == TEXT("array")? das::Type::tArray : das::Type::tTable, At);
        const das::string Leaf = SourceName.find("@container:") == 0 ? SourceName.substr(11) : das::string();
        if (Declared.Kind == TEXT("map"))
        {
            const auto Separator = SourceName.find('|');
            if (SourceName.find("@map:") != 0 || Separator == das::string::npos)
                DSLFail(TEXT("Missing authored map key/value source descriptors"));
            Container->firstType =
                DSLMethodValueType(Program, Module, *Declared.Key, SourceName.substr(5, Separator - 5), Scope, At);
            Container->secondType =
                DSLMethodValueType(Program, Module, *Declared.Value, SourceName.substr(Separator + 1), Scope, At);
        }
        else
        {
            Container->firstType = DSLMethodValueType(Program, Module, *Declared.Element, Leaf, Scope, At);
            if (Declared.Kind == TEXT("set"))
                Container->secondType = new das::TypeDecl(das::Type::tVoid, At);
        }
        if (DSLContainerType(Container, Scope.Folder, Scope.Registry).Identity() != Declared.Identity())
            DSLFail(TEXT("Authored method container source identity mismatch"));
        return Container;
    }
    if (SourceName.find("@math:") == 0)
    {
        auto Type = DSLHandleType(*Program->thisModuleGroup, Module, SourceName.substr(6).c_str());
        if (Declared.Kind != TEXT("struct") || DSLMathPath(Type) != Declared.TypePath)
            DSLFail(TEXT("Authored method math type identity mismatch"));
        return Type;
    }
    if (SourceName.empty())
        return DSLPropertyType(Declared.Kind, *Program->thisModuleGroup, Module, At);
    // The in-progress anonymous module is not yet in thisModuleGroup. Resolve its ::Type locally.
    const bool Local = SourceName.find("::") == 0;
    const auto LookupName = Local ? SourceName.substr(2) : SourceName;
    das::TypeDeclPtr Type = nullptr;
    if (Declared.Kind == TEXT("enum"))
    {
        das::vector<das::EnumerationPtr> Types;
        if (Local)
        {
            if (auto Found = Program->getThisModule()->findEnum(LookupName))
                Types.push_back(Found);
        }
        else
            Types = Program->thisModuleGroup->findEnum(LookupName, Module);
        if (Types.size() == 1)
        {
            Type = new das::TypeDecl(das::Type::tEnumeration, At);
            Type->enumType = Types[0];
        }
    }
    else if (Declared.Kind ==
             TEXT("class") ||
                  Declared.Kind == TEXT("struct") ||
                                        Declared.Kind == TEXT("object") ||
                                                              Declared.Kind == TEXT("event") ||
                                                                                    Declared.Kind == TEXT("delegate"))
    {
        das::vector<das::StructurePtr> Types;
        if (Local)
        {
            if (auto Found = Program->getThisModule()->findStructure(LookupName))
                Types.push_back(Found);
        }
        else
            Types = Program->thisModuleGroup->findStructure(LookupName, Module);
        if (Types.size() == 1)
        {
            Type = new das::TypeDecl(das::Type::tStructure, At);
            Type->structType = Types[0];
        }
    }
    if (!Type)
        DSLFail(TEXT("Import the authored method value declaration: ")+ DSLText(SourceName));
    if (Declared.Kind == TEXT("delegate"))
    {
        if (!DSLDelegate(Type) || !DSLAnnotation(Type->structType->annotations, "ue_delegate"))
            DSLFail(TEXT("Single delegate accessor requires matching type"));
        const auto* A = DSLAnnotation(Type->structType->annotations, "ue_delegate");
        if (Declared.TypePath !=
            TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*DSLString(A->arguments, "_ue_delegate_wire")) + TEXT(".Type"))
            DSLFail(TEXT("Delegate accessor signature mismatch"));
        return Type;
    }
    if (Declared.Kind == TEXT("event"))
    {
        if (!DSLDelegate(Type) || !DSLAnnotation(Type->structType->annotations, "ue_multicast"))
            DSLFail(TEXT("Event accessor requires matching ue_multicast type"));
        const auto* A = DSLAnnotation(Type->structType->annotations, "ue_multicast");
        if (Declared.TypePath !=
            TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*DSLString(A->arguments, "_ue_delegate_wire")) + TEXT(".Type"))
            DSLFail(TEXT("Event accessor signature mismatch"));
        return Type;
    }
    if (Declared.Kind == TEXT("class"))
    {
        if (!DSLTypedClass(Type) || DSLTypedClassPath(Type) != Declared.TypePath)
            DSLFail(TEXT("Typed class source constraint mismatch"));
        return Type;
    }
    if (Declared.Kind == TEXT("object"))
    {
        if (DSLAuthoredView(Type) ? DSLReferenceDeclaration(Type).Id != Declared.AuthoredId
                                  : !Declared.AuthoredId.IsEmpty())
            DSLFail(TEXT("Authored signature identity differs from declaration"));
        if (!DSLTypedReference(Type) || DSLTypedReferencePath(Type) != Declared.TypePath)
            DSLFail(TEXT("Authored method object/interface type identity mismatch"));
        return Type;
    }
    if ((Declared.Kind == TEXT("enum")? !DSLDirectEnum(Type) : !DSLDirectStruct(Type)) ||
         DSLDirectPath(Type, Scope.Folder, Scope.Registry) != Declared.TypePath)
        DSLFail(TEXT("Authored method source type identity/layout mismatch: ")+ DSLText(SourceName));
    return Type;
}
FString DSLMethodValuePath(const FDasDeclaredType& Type)
{
    if (Type.Kind == TEXT("delegate"))
        return Type.DelegateWire;
    if (Type.Kind == TEXT("array") || Type.Kind == TEXT("map") || Type.Kind == TEXT("set"))
    {
        auto* Carrier = DasContainerCarrier(Type);
        if (!Carrier)
            DSLFail(TEXT("Unsupported authored method container carrier"));
        return Carrier->GetPathName();
    }
    return Type.TypePath;
}
das::string DSLMethodNativeKind(const FString& Kind)
{
    return Kind == TEXT("array") || Kind == TEXT("map") || Kind == TEXT("set")? "struct" : DSLUtf8(Kind);
}
// Bridge kind of a method argument or result: a handle view's own method bridge (vector, rotator,
// transform, input_value), otherwise the declared kind; Vector2D falls back to a struct snapshot.
das::string DSLMethodAdapterKind(const FString& Kind, const das::TypeDeclPtr& Type)
{
    if (const char* Bridge = DSLAdaptedBridge(Type, EDasCallBridge::MethodBridge))
        return Bridge;
    return DSLMethodNativeKind(Kind);
}
// Call frame bridge for an argument or result of an adapter kind. InputActionValue has its own native
// transport; text, name, enum, struct and the math structs use the __ue_method_* family.
das::string DSLMethodBridge(const das::string& Kind, bool bResult)
{
    if (Kind == "input_value")
        return bResult ? "__ue_result_input_value" : "__ue_arg_input_value";
    const bool Method = Kind == "text" || Kind == "name" || Kind == "enum" || Kind == "struct" || Kind == "vector" ||
                        Kind == "rotator" || Kind == "transform";
    return das::string(Method ? (bResult ? "__ue_method_result_" : "__ue_method_arg_")
                              : (bResult ? "ue_result_" : "ue_arg_")) +
           Kind;
}
// Math handles with their own method bridge pass as they are; every other described value converts.
bool DSLMethodSourceConversion(const das::string& Descriptor)
{
    if (Descriptor.empty())
        return false;
    if (Descriptor.find("@math:") != 0)
        return true;
    const auto* Script = DSLAdaptedHandleNamed(Descriptor.substr(6));
    return !Script || !Script->GetCallBridge(EDasCallBridge::MethodBridge);
}
das::ExpressionPtr DSLMethodArgumentDefault(bool Native, const FDasDeclaredParameter& Param,
                                            const das::TypeDeclPtr& Type, const das::LineInfo& At)
{
    if (Native && Param.Type.Kind == TEXT("byte") && Param.Default.Kind == TEXT("number"))
        return new das::ExprConstInt(At, int32(Param.Default.Number));
    if (Native && Param.Default.Kind == TEXT("enum"))
        return new das::ExprConstEnumeration(At, DSLUtf8(Param.Default.String), Type);
    return DSLParameterDefaultExpression(Param, At);
}
// DasNativeBinds.inl
FString DasNativeRoutePolicy(const UFunction* Function, const UClass* Receiver);
FString DasNativeRouteTypes(const FDasNativeRoute& Route, const das::ModuleLibrary& Lib,
                            const das::vector<das::TypeDeclPtr>& Types,
                            TFunctionRef<FString(const das::TypeDeclPtr&)> ObjectPath);
// A native gameplay method that a native bind reflects (DasNativeBinds.h, Reflects) calls that bind: the wrapper has
// the method's name, parameters and defaults and calls the bind's route (SimNode_DasNativeRoute) with the
// receiver's handle and class path; object arguments pass their handles, an object result becomes the method's
// reference type. Not when the script sets `options ue_native_routes = false`, the UFUNCTION's
// policy keeps the reflected call (DasNativeRoutePolicy), or the script types differ from the bind's. Returns false
// to emit the reflected wrapper.
bool DSLEmitRoutedMethod(das::Program* Program, das::Module* Module, const das::TypeDeclPtr& ReceiverType,
                         const das::string& Name, const das::LineInfo& At, const FString& ClassPath,
                         const FDasDeclaredMethod& Method, const FDSLMethodSourceSignature& Descriptor)
{
    if (!Program->options.getBoolOption("ue_native_routes", true))
        return false;
    UClass* Class = FindObject<UClass>(nullptr, *ClassPath);
    UFunction* Function = Class ? Class->FindFunctionByName(*DSLText(Name)) : nullptr;
    const FDasNativeRoute Route = FindDasNativeRoute(Function, Class);
    if (!Route.Function)
        return false;
    FString Why = DasNativeRoutePolicy(Function, Class);
    das::vector<das::TypeDeclPtr> Types; // the result, then the parameters
    Types.push_back(DSLMethodValueType(Program, Module, Method.ReturnType, Descriptor.Result, Descriptor.Scope, At));
    for (int32 Index = 0; Index < Method.Parameters.Num(); ++Index)
    {
        auto Type = DSLMethodValueType(Program, Module, Method.Parameters[Index].Type,
                                       Descriptor.Parameters[size_t(Index)], Descriptor.Scope, At);
        // An output parameter is the bind's T& (DasNativeRouteTypes compares the type without the reference).
        if (Method.Parameters[Index].Direction != TEXT("in"))
        {
            Type = DSLObjectCopyType(Type);
            Type->ref = true;
        }
        Types.push_back(Type);
    }
    // An object is a native object reference (not an interface or an authored view), whose class the bind must
    // take or return.
    auto ObjectPath = [](const das::TypeDeclPtr& T)
    {
        return DSLTypedObject(T) ? DSLTypedReferencePath(T) : FString();
    };
    if (Why.IsEmpty())
        Why = DasNativeRouteTypes(Route, *Program->thisModuleGroup, Types, ObjectPath);
    if (!Why.IsEmpty())
    {
        UE_LOG(LogDasRuntime, Verbose, TEXT("%s.%s keeps the reflected call: %s"), *ClassPath, *DSLText(Name), *Why);
        return false;
    }
    auto* Fn = new das::Function();
    Fn->at = At;
    Fn->name = Name;
    Fn->generated = true;
    Fn->privateFunction = false;
    Fn->result = DSLObjectCopyType(Types[0]);
    auto AddArgument = [&](const das::string& ArgName, const das::TypeDeclPtr& Type, bool bMutable)
    {
        auto* Arg = new das::Variable();
        Arg->at = At;
        Arg->name = ArgName;
        Arg->type = DSLObjectCopyType(Type);
        Arg->type->ref = bMutable;
        Arg->type->constant = !bMutable;
        Fn->arguments.push_back(Arg);
        return Arg;
    };
    AddArgument("receiver", ReceiverType, false);
    auto* Block = new das::ExprBlock();
    Block->at = At;
    // A pure out parameter starts from the default value, as on reflection (whose call frame starts it at zero), and
    // is copied to the caller's variable after a successful call; inout passes the variable itself.
    das::vector<das::ExpressionPtr> OutCopies;
    das::vector<das::ExpressionPtr> Arguments{new das::ExprField(At, new das::ExprVar(At, "receiver"), "handle"),
                                              new das::ExprConstString(At, DSLUtf8(ClassPath))};
    for (int32 Index = 0; Index < Method.Parameters.Num(); ++Index)
    {
        const auto& Direction = Method.Parameters[Index].Direction;
        const auto ArgName = "arg_" + das::to_string(Index);
        auto* Arg = AddArgument(ArgName, Types[size_t(Index) + 1], Direction != TEXT("in"));
        if (Direction == TEXT("in"))
            Arg->init = DSLMethodArgumentDefault(true, Method.Parameters[Index], Arg->type, At);
        das::ExpressionPtr Value = new das::ExprVar(At, ArgName);
        if (Direction == TEXT("out"))
        {
            const auto OutName = "__out_" + das::to_string(Index);
            auto* Var = new das::Variable();
            Var->at = At;
            Var->name = OutName;
            Var->type = DSLObjectCopyType(Arg->type);
            Var->type->ref = false;
            Var->type->constant = false;
            auto* Let = new das::ExprLet();
            Let->at = At;
            Let->variables.push_back(Var);
            Block->list.push_back(Let);
            Value = new das::ExprVar(At, OutName);
            OutCopies.push_back(new das::ExprCopy(At, new das::ExprVar(At, ArgName), new das::ExprVar(At, OutName)));
        }
        else if (!ObjectPath(Arg->type).IsEmpty())
            Value = new das::ExprField(At, Value, "handle");
        Arguments.push_back(Value);
    }
    auto* Call = DSLCall(At, DSLUtf8(Route.Function->RouteName), {});
    Call->arguments = std::move(Arguments);
    das::ExpressionPtr Result = Call;
    if (!ObjectPath(Fn->result).IsEmpty())
        Result = DSLNativeReferenceUnpack(At, Fn->result, Call);
    if (Fn->result->baseType == das::Type::tVoid)
        Block->list.push_back(Call);
    else if (OutCopies.empty())
        Block->list.push_back(new das::ExprReturn(At, Result));
    else
    {
        auto* Var = new das::Variable();
        Var->at = At;
        Var->name = "__result";
        Var->type = DSLObjectCopyType(Fn->result);
        Var->type->ref = false;
        Var->type->constant = false;
        Var->init = Result;
        auto* Let = new das::ExprLet();
        Let->at = At;
        Let->variables.push_back(Var);
        Block->list.push_back(Let);
    }
    for (auto& Copy : OutCopies)
        Block->list.push_back(Copy);
    if (Fn->result->baseType != das::Type::tVoid && !OutCopies.empty())
        Block->list.push_back(new das::ExprReturn(At, new das::ExprVar(At, "__result")));
    Fn->body = Block;
    if (!Module->addFunction(Fn, true))
        DSLFail(TEXT("Authored method overload collision"));
    return true;
}
// Method-call syntax uses daScript's ordinary receiver-first overload resolution.
bool DSLEmitReferenceMethod(das::Program* Program, das::Module* Module, const das::TypeDeclPtr& ReceiverType,
                            const das::string& Name, const das::LineInfo& At)
{
    bool Exists = false;
    Program->library.foreach (
        [&](das::Module* Mod)
        {
            Mod->functions.foreach (
                [&](const das::FunctionPtr& Fn)
                {
                    if (Fn->name == Name && !Fn->arguments.empty() &&
                        Fn->arguments[0]->type->structType == ReceiverType->structType)
                        Exists = true;
                });
            return true;
        },
        "*");
    if (Exists)
        return false;
    const bool Native = !DSLAuthoredView(ReceiverType);
    FDasDeclaredClass Declaration;
    FDasDeclaredMethod NativeMethod;
    FDSLMethodSourceSignature NativeDescriptor;
    if (Native)
        DSLNativeMethodDescriptor(Program, Module, ReceiverType, Name, At, Declaration, NativeMethod, NativeDescriptor);
    else
        Declaration = DSLReferenceDeclaration(ReceiverType);
    const auto* Method =
        Declaration.Methods.FindByPredicate([&](const auto& Item) { return Item.Name == DSLText(Name); });
    if (!Method || !Method->bCallable || Method->bStatic)
        DSLFail(TEXT("Unknown callable authored method: ")+ DSLText(Name));
    FString Wire;
    if (!Native)
    {
        FDasAuthoringModule Model;
        FDasDeclaredClass Contract;
        Contract.Id = Declaration.Id;
        Contract.Name = Declaration.Name;
        Contract.NativeParent = Declaration.NativeParent;
        Contract.Methods.Add(*Method);
        Model.Id = Declaration.Id + TEXT(".dispatch_transport");
        while (Model.Id == Contract.Id || Model.Id == Method->Id ||
               Method->Parameters.ContainsByPredicate([&](const auto& Param) { return Param.Id == Model.Id; }))
            Model.Id += TEXT("_");
        Model.Classes.Add(Contract);
        FString Error;
        if (!Model.ToJson(Wire, Error))
            DSLFail(Error);
    }
    const auto* Reference = DSLAnnotation(ReceiverType->structType->annotations, "ue_object_ref");
    const FString SourceKey =
        Reference ? DSLString(Reference->arguments, "script") + TEXT("::")+ DSLString(Reference->arguments, "type") +
                                                                     TEXT("::")+ Method->Name : FString();
    const auto* Descriptor = Native ? &NativeDescriptor : DSLMethodSourceSignatures.Find(SourceKey);
    if (!Descriptor || Descriptor->Parameters.size() != size_t(Method->Parameters.Num()))
        DSLFail(TEXT("Missing authored method source signature"));
    if (Native && DSLEmitRoutedMethod(Program, Module, ReceiverType, Name, At, Declaration.Id, *Method, *Descriptor))
        return true;
    auto* Fn = new das::Function();
    Fn->at = At;
    Fn->name = Name;
    Fn->generated = true;
    Fn->privateFunction = false;
    Fn->result = DSLMethodValueType(Program, Module, Method->ReturnType, Descriptor->Result, Descriptor->Scope, At);
    das::vector<das::FunctionPtr> Helpers;
    TSet<FString> HelperPaths;
    auto EnsureHelpers = [&](const das::TypeDeclPtr& Type, const FString& Path)
    {
        if (DSLContainer(Type) || DSLDirectStruct(Type))
            DSLEnsureStructHelpers(Type, Path, *Program->thisModuleGroup, Module, Helpers, HelperPaths);
    };
    EnsureHelpers(Fn->result, DSLMethodValuePath(Method->ReturnType));
    if (Fn->result->baseType == das::Type::tVoid && Method->ReturnType.Kind != TEXT("void"))
        DSLFail(TEXT("Unsupported authored method result: ")+ Method->ReturnType.Kind);
    auto AddArgument = [&](const das::string& ArgName, const das::TypeDeclPtr& Type, bool Mutable)
    {
        auto* Arg = new das::Variable();
        Arg->at = At;
        Arg->name = ArgName;
        Arg->type = DSLObjectCopyType(Type);
        Arg->type->ref = Mutable;
        Arg->type->constant = !Mutable;
        Fn->arguments.push_back(Arg);
    };
    AddArgument("receiver", ReceiverType, false);
    auto* Block = new das::ExprBlock();
    Block->at = At;
    Fn->body = Block;
    auto Local = [&](const das::string& LocalName, const das::TypeDeclPtr& Type, das::ExpressionPtr Init)
    {
        auto* Var = new das::Variable();
        Var->at = At;
        Var->name = LocalName;
        Var->type = DSLObjectCopyType(Type);
        Var->type->ref = false;
        Var->type->constant = false;
        Var->init = Init;
        Var->init_via_move = DSLContainer(Type) || DSLDirectStruct(Type);
        auto* Let = new das::ExprLet();
        Let->at = At;
        Let->variables.push_back(Var);
        Block->list.push_back(Let);
    };
    auto Str = [&](const FString& Value)
    {
        return new das::ExprConstString(At, DSLUtf8(Value));
    };
    auto CallHandle = [&]()
    {
        return new das::ExprVar(At, "__call");
    };
    const FString SuperId = Reference ? DSLString(Reference->arguments, "_ue_super_id") : FString();
    das::vector<das::ExpressionPtr> BeginArgs;
    BeginArgs.push_back(new das::ExprField(At, new das::ExprVar(At, "receiver"), "handle"));
    if (!SuperId.IsEmpty())
        BeginArgs.push_back(Str(SuperId));
    BeginArgs.push_back(Str(Declaration.Id));
    BeginArgs.push_back(Str(Method->Name));
    if (!Native)
        BeginArgs.push_back(Str(Wire));
    auto* Begin = DSLCall(At,
                          Native              ? "__ue_begin_native_gameplay_call"
                          : SuperId.IsEmpty() ? "__ue_begin_authored_call"
                                              : "__ue_begin_super_call",
                          {});
    Begin->arguments = std::move(BeginArgs);
    Local("__call", DSLHandleType(*Program->thisModuleGroup, Module, "UCallHandle"), Begin);
    auto Read = [&](const FString& DeclaredKind, const FString& Path, const das::TypeDeclPtr& Type)
    {
        const das::string Kind = DSLMethodAdapterKind(DeclaredKind, Type);
        const das::string Bridge = DSLMethodBridge(Kind, true);
        if (Kind == "delegate")
            return DSLCall(At, "__ue_method_result_delegate",
                           {CallHandle(), Str(Path), Str(Method->ReturnType.DelegateWire)});
        return DSLCall(At, Bridge, {CallHandle(), Str(Path)});
    };
    for (int32 Index = 0; Index < Method->Parameters.Num(); ++Index)
    {
        const auto& Param = Method->Parameters[Index];
        const auto Type = DSLMethodValueType(Program, Module, Param.Type, Descriptor->Parameters[size_t(Index)],
                                             Descriptor->Scope, At);
        EnsureHelpers(Type, DSLMethodValuePath(Param.Type));
        if (Type->baseType == das::Type::tVoid)
            DSLFail(TEXT("Unsupported authored method parameter: ")+ Param.Type.Kind);
        const auto ArgName = "arg_" + das::to_string(Index);
        AddArgument(ArgName, Type, Param.Direction != TEXT("in"));
        Fn->arguments.back()->init = DSLMethodArgumentDefault(Native, Param, Type, At);
        if (Param.Direction != TEXT("out"))
        {
            const das::string Kind = DSLMethodAdapterKind(Param.Type.Kind, Type);
            const das::string Bridge = DSLMethodBridge(Kind, false);
            if (Kind == "delegate")
                Block->list.push_back(
                    DSLCall(At, "__ue_method_arg_delegate",
                            {CallHandle(), Str(Param.Name), Str(Param.Type.DelegateWire),
                             DSLDirectPack(At, Type, new das::ExprVar(At, ArgName), Param.Type.TypePath)}));
            else
                Block->list.push_back(DSLCall(
                    At, Bridge,
                    {CallHandle(), Str(Param.Name),
                     (DSLMethodSourceConversion(Descriptor->Parameters[size_t(Index)])
                          ? DSLDirectPack(At, Type, new das::ExprVar(At, ArgName), DSLMethodValuePath(Param.Type))
                          : static_cast<das::ExpressionPtr>(new das::ExprVar(At, ArgName)))}));
        }
    }
    Block->list.push_back(DSLCall(At, "ue_invoke_call", {CallHandle()}));
    if (Method->ReturnType.Kind != TEXT("void"))
        Local("__result", Fn->result,
              !DSLMethodSourceConversion(Descriptor->Result)
              ? static_cast<das::ExpressionPtr>(Read(Method->ReturnType.Kind, TEXT("ReturnValue"), Fn->result))
              : DSLDirectUnpack(At, Fn->result,
                                Read(Method->ReturnType.Kind, TEXT("ReturnValue"), Fn->result),
                                     DSLMethodValuePath(Method->ReturnType)));
    // Read every output before publishing any of them. Failed calls do not modify ref/out arguments.
    for (int32 Index = 0; Index < Method->Parameters.Num(); ++Index)
    {
        const auto& Param = Method->Parameters[Index];
        if (Param.Direction != TEXT("in"))
            Local("__out_" + das::to_string(Index), Fn->arguments[size_t(Index) + 1]->type,
                  !DSLMethodSourceConversion(Descriptor->Parameters[size_t(Index)])
                      ? static_cast<das::ExpressionPtr>(
                            Read(Param.Type.Kind, Param.Name, Fn->arguments[size_t(Index) + 1]->type))
                      : DSLDirectUnpack(At, Fn->arguments[size_t(Index) + 1]->type,
                                        Read(Param.Type.Kind, Param.Name, Fn->arguments[size_t(Index) + 1]->type),
                                        DSLMethodValuePath(Param.Type)));
    }
    Block->finalList.push_back(DSLCall(At, Native ? "ue_release_call" : "__ue_release_authored_call", {CallHandle()}));
    for (int32 Index = 0; Index < Method->Parameters.Num(); ++Index)
        if (Method->Parameters[Index].Direction != TEXT("in"))
        {
            auto* Left = new das::ExprVar(At, "arg_" + das::to_string(Index));
            auto* Right = new das::ExprVar(At, "__out_" + das::to_string(Index));
            const auto& Type = Fn->arguments[size_t(Index) + 1]->type;
            Block->list.push_back(DSLContainer(Type) || DSLDirectStruct(Type)
                                      ? static_cast<das::ExpressionPtr>(new das::ExprMove(At, Left, Right))
                                      : static_cast<das::ExpressionPtr>(new das::ExprCopy(At, Left, Right)));
        }
    if (Method->ReturnType.Kind != TEXT("void"))
    {
        auto* Return = new das::ExprReturn(At, new das::ExprVar(At, "__result"));
        Return->moveSemantics = DSLContainer(Fn->result) || DSLDirectStruct(Fn->result);
        Block->list.push_back(Return);
    }
    for (const auto& Helper : Helpers)
        if (!Module->addFunction(Helper, true))
            DSLFail(TEXT("Authored method value helper collision"));
    if (!Module->addFunction(Fn, true))
        DSLFail(TEXT("Authored method overload collision"));
    return true;
}

// Nested writes use one owned root snapshot and publish it only after validation.
struct FUEReferenceMutationVisitor : das::Visitor
{
    das::Program* Program;
    das::Module* Module;
    das::vector<das::FunctionPtr> Helpers;
    bool Changed = false;
    FUEReferenceMutationVisitor(das::Program* P, das::Module* M) : Program(P), Module(M) {}
    struct FRoot
    {
        das::Expression* Expr = nullptr;
        das::ExpressionPtr Owner;
        das::TypeDeclPtr OwnerType;
        das::string Name;
    };
    FRoot Root(das::Expression* E)
    {
        for (int Depth = 0; E && Depth < 32; ++Depth)
        {
            if (E->rtti_isField())
            {
                auto* F = static_cast<das::ExprField*>(E);
                if (DSLTypedReference(F->value->type) && !DSLTypedInterface(F->value->type))
                    return {E, F->value, F->value->type, F->name};
                E = F->value;
            }
            else if (E->rtti_isAt())
                E = static_cast<das::ExprAt*>(E)->subexpr;
            else if (E->rtti_isCall())
            {
                auto* C = static_cast<das::ExprCall*>(E);
                if (C->name.find(".`") == 0 && C->name.find('`', 2) == das::string::npos && C->arguments.size() == 1 &&
                    DSLTypedReference(C->arguments[0]->type) && !DSLTypedInterface(C->arguments[0]->type))
                    return {E, C->arguments[0], C->arguments[0]->type, C->name.substr(2)};
                return {};
            }
            else
                return {};
        }
        return {};
    }
    bool Operation(const das::string& N)
    {
        return N == "insert" || N == "insert_clone" || N == "erase" || N == "clear" || N == "push" ||
               N == "push_clone" || N == "pop";
    }
    das::ExpressionPtr Rewrite(das::Expression* Original, das::Expression* Target, das::ExprOp2* Assignment,
                               das::ExprCall* OperationCall)
    {
        const auto R = Root(Target);
        if (!R.Expr)
            return Original;
        try
        {
            // Authored whole-value writes still need the readonly check below.
            const bool Native = !DSLAuthoredView(R.OwnerType);
            if (Native && !OperationCall && R.Expr == Target)
                return Original;
            FDasDeclaredClass C;
            FDasDeclaredProperty NativeProperty;
            if (Native)
            {
                C.Id = DSLTypedReferencePath(R.OwnerType);
                NativeProperty = DSLNativeReferenceProperty(R.OwnerType, R.Name);
            }
            else
                C = DSLReferenceDeclaration(R.OwnerType);
            const auto* Property =
                Native ? &NativeProperty
                       : C.Properties.FindByPredicate([&](const auto& V) { return V.Name == DSLText(R.Name); });
            if (!Property || (Property->Type.Kind != TEXT("struct") && !DasContainerCarrier(Property->Type)))
                return Original;
            if (Property->bReadOnly)
                DSLFail(TEXT("Cannot mutate readonly authored property"));
            if (!OperationCall && R.Expr == Target)
                return Original;
            das::TypeDeclPtr RootType;
            if (Native)
                RootType = DSLNativeValueType(Program, Module, Property->Type, Original->at);
            else
            {
                const auto* A = DSLAnnotation(R.OwnerType->structType->annotations, "ue_object_ref");
                const FString Key = DSLString(A->arguments, "script") + TEXT("::")+ DSLString(A->arguments, "type") +
                                                                             TEXT("::")+ DSLText(R.Name);
                const auto* Descriptor = DSLPropertySourceSignatures.Find(Key);
                if (!Descriptor)
                    return Original;
                RootType = DSLMethodValueType(Program, Module, Property->Type, Descriptor->Name, Descriptor->Scope,
                                              Original->at);
            }

            if (!DSLDirectStruct(RootType) && !DSLContainer(RootType))
            {
                if (Native)
                    DSLFail(TEXT("Native nested writes require a supported value struct or container"));
                return Original;
            }
            const FString RootPath = DasContainerCarrier(Property->Type)
                                         ? DasContainerCarrier(Property->Type)->GetPathName()
                                         : Property->Type.TypePath;
            das::vector<das::Expression*> Steps;
            for (auto* E = Target; E != R.Expr;)
            {
                Steps.push_back(E);
                if (E->rtti_isField())
                    E = static_cast<das::ExprField*>(E)->value;
                else if (E->rtti_isAt())
                    E = static_cast<das::ExprAt*>(E)->subexpr;
                else
                    DSLFail(TEXT("Unsupported authored mutation path"));
            }
            auto* Fn = new das::Function();
            Fn->at = Original->at;
            Fn->name = "__ue_reference_mutation_" + das::to_string(Module->functions.unlocked_size()) + "_" +
                       das::to_string(Helpers.size());
            Fn->generated = true;
            Fn->privateFunction = false;
            auto* Call = DSLCall(Original->at, Fn->name, {R.Owner});
            auto Add = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Input)
            {
                auto* V = new das::Variable();
                V->at = Original->at;
                V->name = N;
                V->type = DSLObjectCopyType(T);
                V->type->ref = false;
                V->type->constant = true;
                Fn->arguments.push_back(V);
                if (Input)
                    Call->arguments.push_back(Input);
            };
            Add("receiver", R.OwnerType, nullptr);
            auto Type = RootType;
            das::ExpressionPtr Left = new das::ExprVar(Original->at, "snapshot");
            for (auto It = Steps.rbegin(); It != Steps.rend(); ++It)
            {
                auto* E = *It;
                if (E->rtti_isField())
                {
                    const auto N = static_cast<das::ExprField*>(E)->name;
                    if (!DSLMathPath(Type).IsEmpty())
                    {
                        if (!DSLAdaptedMember(Type, N, EDasMemberUse::AuthoredMutation))
                            DSLFail(TEXT("Unsupported math mutation member"));
                        Type = DSLAdaptedMemberType(Type, N);
                    }
                    else
                    {
                        if (!DSLDirectStruct(Type))
                            DSLFail(TEXT("Mutation crosses unsupported authored member"));
                        const auto* F = Type->structType->findField(N);
                        if (!F)
                            DSLFail(TEXT("Unknown authored mutation member"));
                        for (const auto& Annotation : F->annotation)
                            if (Annotation.name == "ue_property" && Annotation.type == das::Type::tString &&
                                Annotation.sValue == "BlueprintReadOnly")
                                DSLFail(TEXT("Cannot mutate readonly authored struct member"));
                        Type = F->type;
                    }
                    Left = new das::ExprField(Original->at, Left, N);
                }
                else
                {
                    if (!DSLContainer(Type) || DSLSet(Type))
                        DSLFail(TEXT("Indexed mutation requires array or map"));
                    auto* Index = static_cast<das::ExprAt*>(E);
                    const bool Array = Type->baseType == das::Type::tArray;
                    auto IndexType = Array ? static_cast<das::TypeDeclPtr>(new das::TypeDecl(
                                                 Index->index->type && Index->index->type->baseType == das::Type::tInt64
                                                     ? das::Type::tInt64
                                                     : das::Type::tInt,
                                                 Original->at))
                                           : Type->firstType;
                    const auto N = "index_" + das::to_string(Fn->arguments.size());
                    Add(N, IndexType, Index->index);
                    Left = new das::ExprAt(Original->at, Left, new das::ExprVar(Original->at, N));
                    Type = Array ? Type->firstType : Type->secondType;
                }
            }
            bool Result = false;
            das::ExpressionPtr Mutation;
            if (OperationCall)
            {
                if (!DSLContainer(Type))
                    DSLFail(TEXT("Mutation operation requires container"));
                const auto N = OperationCall->name;
                const bool Array = Type->baseType == das::Type::tArray, Set = DSLSet(Type),
                           Push = N == "push" || N == "push_clone";
                const auto Count = OperationCall->arguments.size();
                if (N == "clear" ? Count != 1
                    : N == "pop" ? (!Array || Count != 1)
                    : Push       ? (!Array || (Count != 2 && Count != 3))
                    : (N == "insert" || N == "insert_clone")
                        ? (Array || (N == "insert_clone" && Set) || Count != (Set ? 2 : 3))
                        : (Array ? (Count != 2 && Count != 3) : Count != 2))
                    DSLFail(TEXT("Unsupported authored container operation/arity"));
                auto* Op = DSLCall(Original->at, N, {Left});
                for (size_t I = 1; I < Count; ++I)
                {
                    const auto Param = "arg_" + das::to_string(I);
                    auto T = Array    ? (Push && I == 1
                                             ? Type->firstType
                                             : static_cast<das::TypeDeclPtr>(new das::TypeDecl(
                                                OperationCall->arguments[I]->type &&
                                                        OperationCall->arguments[I]->type->baseType == das::Type::tInt64
                                                    ? das::Type::tInt64
                                                    : das::Type::tInt,
                                                   Original->at)))
                             : I == 1 ? Type->firstType
                                      : Type->secondType;
                    Add(Param, T, OperationCall->arguments[I]);
                    Op->arguments.push_back(new das::ExprVar(Original->at, Param));
                }
                Mutation = Op;
                Result = N == "erase" && !Array;
            }
            else
            {
                Add("value", Type, Assignment->right);
                auto* Value = new das::ExprVar(Original->at, "value");
                if (Assignment->rtti_isClone())
                    Mutation = new das::ExprClone(Original->at, Left, Value);
                else if (Assignment->rtti_isCopy())
                    Mutation = new das::ExprCopy(Original->at, Left, Value);
                else if (Assignment->__rtti == das::string("ExprMove"))
                    DSLFail(TEXT("Authored nested writes use copy or clone, not consuming move"));
                else
                    Mutation = new das::ExprOp2(Original->at, Assignment->op, Left, Value);
            }
            Fn->result = new das::TypeDecl(Result ? das::Type::tBool : das::Type::tVoid, Original->at);
            auto* Body = new das::ExprBlock();
            Body->at = Original->at;
            auto Local = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Init, bool Move)
            {
                auto* V = new das::Variable();
                V->at = Original->at;
                V->name = N;
                V->type = DSLObjectCopyType(T);
                V->type->ref = false;
                V->type->constant = false;
                V->init = Init;
                V->init_via_move = Move;
                auto* Let = new das::ExprLet();
                Let->at = Original->at;
                Let->variables.push_back(V);
                Body->list.push_back(Let);
            };
            auto Handle = [&]()
            {
                return new das::ExprField(Original->at, new das::ExprVar(Original->at, "receiver"), "handle");
            };
            Local("snapshot", RootType,
                  DSLDirectUnpack(Original->at, RootType,
                                  DSLCall(Original->at, Native ? "__ue_native_get_struct" : "__ue_authored_get_struct",
                                          {Handle(), new das::ExprConstString(Original->at, DSLUtf8(C.Id)),
                                           new das::ExprConstString(Original->at, R.Name)}),
                                  RootPath),
                  true);
            if (Result)
                Local("result", Fn->result, Mutation, false);
            else
                Body->list.push_back(Mutation);
            Body->list.push_back(
                DSLCall(Original->at, Native ? "__ue_native_set_struct" : "__ue_authored_set_struct",
                        {Handle(), new das::ExprConstString(Original->at, DSLUtf8(C.Id)),
                         new das::ExprConstString(Original->at, R.Name),
                         DSLDirectPack(Original->at, RootType, new das::ExprVar(Original->at, "snapshot"), RootPath)}));
            if (Result)
                Body->list.push_back(new das::ExprReturn(Original->at, new das::ExprVar(Original->at, "result")));
            Fn->body = Body;
            Helpers.push_back(Fn);
            Changed = true;
            return Call;
        }
        catch (const std::exception& X)
        {
            Program->error("Authored nested mutation failed", X.what(), "", Original->at,
                           das::CompilationError::invalid_type);
            Changed = true;
            return DSLCall(Original->at, "__ue_invalid_authored_mutation",
                           {new das::ExprConstString(Original->at, X.what())});
        }
    }
    virtual das::ExpressionPtr visit(das::ExprCopy* E) override { return Rewrite(E, E->left, E, nullptr); }
    virtual das::ExpressionPtr visit(das::ExprClone* E) override { return Rewrite(E, E->left, E, nullptr); }
    virtual das::ExpressionPtr visit(das::ExprMove* E) override { return Rewrite(E, E->left, E, nullptr); }
    virtual das::ExpressionPtr visit(das::ExprOp2* E) override
    {
        return E->op == "+=" || E->op == "-=" || E->op == "*=" || E->op == "/=" ? Rewrite(E, E->left, E, nullptr) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprCall* E) override
    {
        return Operation(E->name) && !E->arguments.empty() ? Rewrite(E, E->arguments[0], nullptr, E) : E;
    }
};
#include "DasReferenceUpcasts.inl"
struct FUEPropertyPass : das::PassMacro
{
    FUEPropertyPass() : PassMacro("ue_authored_accessors") {}
    virtual bool apply(das::Program* P, das::Module* M) override
    {
        struct FScan : das::Visitor
        {
            struct FMethod
            {
                das::TypeDeclPtr Type;
                das::string Name;
                das::LineInfo At;
            };
            das::vector<FMethod> Methods;
            das::vector<das::ExprField*> Fields;
            TSet<das::ExprField*> MethodFields;
            virtual void preVisit(das::ExprInvoke* E) override
            {
                if (!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())
                    return;
                auto* Field = static_cast<das::ExprField*>(E->arguments[0]);
                MethodFields.Add(Field);
                auto Type = Field->value->rtti_isTypeDecl() ? static_cast<das::ExprTypeDecl*>(Field->value)->typeexpr
                                                            : Field->value->type;
                if (DSLTypedReference(Type) && !DSLTypedInterface(Type))
                    Methods.push_back({Type, Field->name, E->at});
            }
            virtual das::ExpressionPtr visit(das::ExprField* E) override
            {
                if (!MethodFields.Contains(E) && E->name != "handle" && E->value &&
                    (DSLAuthoredView(E->value->type) ||
                     (DSLTypedReference(E->value->type) && !DSLTypedInterface(E->value->type))))
                    Fields.push_back(E);
                return E;
            }
        } Scan;
        das::vector<das::FunctionPtr> Functions;
        M->functions.foreach (
            [&](const das::FunctionPtr& F)
            {
                if (!F->generated || F->name.find(".`") != 0)
                    Functions.push_back(F);
            });
        for (const auto& F : Functions)
            if (F->body)
            {
                FDSLTypeCallVisitor Types(FDSLTypeScope{DSLText(M->name), FString(), FString(), P->thisModuleGroup, M});
                F->body = F->body->visit(Types);
                F->body->visit(Scan);
            }
        bool Changed = false;
        for (auto* E : Scan.Fields)
            try
            {
                Changed |= DSLEmitProperty(P, M, E->value->type, E->name, E->at);
            }
            catch (const std::exception& X)
            {
                P->error("Authored property generation failed", X.what(), "", E->at,
                         das::CompilationError::invalid_type);
            }
        for (const auto& Method : Scan.Methods)
            try
            {
                Changed |= DSLEmitReferenceMethod(P, M, Method.Type, Method.Name, Method.At);
            }
            catch (const std::exception& X)
            {
                P->error("Authored method generation failed", X.what(), "", Method.At,
                         das::CompilationError::invalid_type);
            }
        FUEReferenceMutationVisitor Mutations(P, M);
        for (const auto& F : Functions)
            if (F->body && F->name.find("__ue_reference_mutation_") != 0)
                F->body = F->body->visit(Mutations);
        for (const auto& F : Mutations.Helpers)
            if (!M->addFunction(F, true))
                P->error("Authored mutation helper collision", "", "", F->at, das::CompilationError::invalid_type);
        for (const auto& F : Functions)
            if (F->body)
            {
                FUEReferenceUpcastVisitor Upcasts(P, M, F);
                F->body = F->body->visit(Upcasts);
                Changed |= Upcasts.Changed;
            }
        return Changed || Mutations.Changed;
    }
};
