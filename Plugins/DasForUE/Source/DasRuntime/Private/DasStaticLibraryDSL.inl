// Static source declarations share IR and frame transport with instance methods.
struct FUELibraryAnnotation : das::StructureAnnotation
{
    FUELibraryAnnotation() : StructureAnnotation("ue_library") {}
    virtual bool look(const das::StructurePtr&, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            if (const auto* Done = Args.find(DSLModelKey, das::Type::tString))
            {
                if (Done->at.fileInfo)
                    DSLFail(TEXT("Reserved library argument"));
                return true;
            }
            DSLDeclarationKind(St->annotations);
            DSLAnnotationArguments(Args);
            DSLCommentTooltip(St->at, Args);
            if (!St->isClass || St->parent || St->hasStaticMembers)
                DSLFail(TEXT("ue_library requires a root class without fields/static state"));
            DSLExpandDeclarationGroups(St, true);
            DSLNormalizeIdentityDefaults(St, Group);
            for (const auto& Field : St->fields)
                if (!Field.generated && !Field.classMethod && Field.name != "__rtti")
                    DSLFail(TEXT("Function library cannot declare properties or components"));
            FDasAuthoringModule Model;
            const FString Script = FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));
            Model.Id = DSLString(Args, "module_id", FPaths::GetBaseFilename(Script));
            FDasDeclaredClass C;
            C.Id = DSLString(Args, "id", Model.Id + TEXT(".")+ DSLText(St->name));
            C.Name = DSLText(St->name);
            C.SourceScript = Script;
            C.NativeParent = TEXT("/Script/Engine.BlueprintFunctionLibrary");
            for (const auto& A : Args)
            {
                if (A.name == "module_id" || A.name == "id" || A.name == "type_folder" || A.name == "type_registry")
                    continue;
                if (DSLIsMetadata(A.name))
                    DSLMetadata(A, C.Metadata);
                else
                    DSLFail(TEXT("Unsupported ue_library option: ")+ DSLText(A.name));
            }
            das::vector<das::FunctionPtr> Functions, Generated;
            St->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent == St && !F->generated)
                        Functions.push_back(F);
                });
            const FString TypeFolder = DSLString(Args, "type_folder"), TypeRegistry = DSLString(Args, "type_registry");
            if (!TypeFolder.IsEmpty() && !TypeRegistry.IsEmpty())
                DSLFail(TEXT("Choose type_registry or type_folder"));
            C.LibraryTypeRegistry = TypeRegistry;
            const FDSLTypeScope TypeScope{Model.Id, TypeFolder, TypeRegistry, &Group, St->module};
            das::vector<das::FunctionPtr> HelpersGenerated;
            TSet<FString> Helpers;
            auto DeclaredType = [&](const das::TypeDeclPtr& T)
            {
                FDasDeclaredType D;
                D.Kind = DSLTypeName(T);
                if (D.Kind.IsEmpty())
                    DSLFail(TEXT("Unsupported static library value type"));
                if (DSLDelegate(T))
                    D = DSLDelegateMethodType(T);
                else if (DSLContainer(T))
                {
                    D = DSLContainerType(T, TypeFolder, TypeRegistry);
                    DSLEnsureStructHelpers(T, DSLDirectPath(T, TypeFolder, TypeRegistry), Group, St->module,
                                           HelpersGenerated, Helpers);
                }
                else if (DSLTypedClass(T))
                    D.TypePath = DSLTypedClassPath(T);
                else if (DSLTypedReference(T))
                {
                    D.TypePath = DSLTypedReferencePath(T);
                    if (DSLAuthoredView(T))
                        D.AuthoredId = DSLReferenceDeclaration(T).Id;
                }
                else if (DSLTypedValue(T, EDasTypeKind::Enum) || DSLDirectStruct(T) || !DSLMathPath(T).IsEmpty())
                {
                    D.TypePath = DSLDirectPath(T, TypeFolder, TypeRegistry);
                    if (DSLDirectStruct(T) || DSLAdaptedSnapshot(T, EDasCallBridge::MethodBridge))
                        DSLEnsureStructHelpers(T, D.TypePath, Group, St->module, HelpersGenerated, Helpers);
                }
                else if (D.GetKind() == EDasTypeKind::Object || D.GetKind() == EDasTypeKind::Class)
                    D.TypePath = TEXT("/Script/CoreUObject.Object");
                else if (D.GetKind() == EDasTypeKind::Struct || D.GetKind() == EDasTypeKind::Enum)
                    DSLFail(TEXT("Use a typed struct/enum instead of an untyped value token"));
                return D;
            };
            auto Packed = [](const das::TypeDeclPtr& T)
            {
                return DSLTypedClass(T) || DSLContainer(T) || DSLTypedValue(T, EDasTypeKind::Enum) ||
                       DSLDirectStruct(T) || DSLAdaptedSnapshot(T, EDasCallBridge::MethodBridge) ||
                       DSLTypedReference(T);
            };
            auto ABI = [&](const das::TypeDeclPtr& T)
            {
                return DSLDelegate(T) ? static_cast<das::TypeDeclPtr>(new das::TypeDecl(das::Type::tUInt64, T->at))
                                      : DSLReflectedValueABI(T, Group, St->module);
            };
            for (const auto& F : Functions)
            {
                const auto* A = DSLAnnotation(F->annotations, "ue_function");
                if (!A || !F->isStaticClassMethod || F->isConstClassMethod || !F->body || !F->result ||
                    F->result->ref || F->arguments.size() > 16)
                    DSLFail(TEXT("Library methods require annotated def static with scalar/void result and up to 16 arguments"));
                FDasDeclaredMethod M;
                M.Name = DSLText(F->name.substr(F->name.find_last_of('`') + 1));
                M.Id = C.Id + TEXT(".")+ M.Name;
                M.ScriptFile = Script;
                M.EntryPoint = TEXT("ue_")+ C.Name + TEXT("_")+ M.Name;
                M.bStatic = true;
                M.bCallable = false;
                M.ReturnType = DeclaredType(F->result);
                auto MethodArgs = A->arguments;
                DSLAnnotationArguments(MethodArgs);
                DSLCommentTooltip(F->at, MethodArgs, &F->annotations);
                for (const auto& V : MethodArgs)
                {
                    if (V.name == "id")
                        M.Id = DSLString(MethodArgs, "id");
                    else if (V.name == "BlueprintCallable")
                        M.bCallable = DSLBool(V);
                    else if (V.name == "BlueprintPure")
                        M.bPure = DSLBool(V);
                    else if (V.name == "WorldContext")
                        M.WorldContext = DSLString(MethodArgs, "WorldContext");
                    else if (DSLIsMetadata(V.name))
                        DSLMetadata(V, M.Metadata);
                    else
                        DSLFail(TEXT("Unsupported static function option: ")+ DSLText(V.name));
                }
                if (M.bPure)
                {
                    if (const auto* B = MethodArgs.find("BlueprintCallable", das::Type::tBool); B && !DSLBool(*B))
                        DSLFail(TEXT("BlueprintPure conflicts with BlueprintCallable=false"));
                    M.bCallable = true;
                }
                if (!M.bCallable)
                    DSLFail(TEXT("Static library method requires BlueprintCallable or BlueprintPure"));
                for (const auto& Arg : F->arguments)
                {
                    FDasDeclaredParameter P;
                    P.Name = DSLText(Arg->name);
                    if (P.Name == TEXT("__WorldContext") || P.Name.StartsWith(TEXT("__ue_")))
                        DSLFail(TEXT("Reserved library parameter name"));
                    P.Id = M.Id + TEXT(".")+ P.Name;
                    P.Type = DeclaredType(Arg->type);
                    P.Direction = DSLParameterDirection(Arg, FString());
                    if (P.Type.GetKind() == EDasTypeKind::Void)
                        DSLFail(TEXT("Void static parameter"));
                    P.Default = DSLParameterDefault(Arg->init);
                    if (DSLDelegate(Arg->type) && P.Direction != TEXT("in"))
                        DSLFail(TEXT("Delegate ref/out is unsupported"));
                    M.Parameters.Add(P);
                }
                struct FNoSelf : das::Visitor
                {
                    virtual void preVisit(das::ExprVar* E) override
                    {
                        if (E->name == "self" || E->name == "Super")
                            DSLFail(TEXT("Static library cannot use self or Super"));
                    }
                    virtual void preVisit(das::ExprCall* E) override
                    {
                        const auto N = E->name.substr(E->name.find_last_of(':') + 1);
                        if (N == "ue_self_object" || N.find("ue_get_self_") == 0 || N.find("ue_set_self_") == 0)
                            DSLFail(TEXT("Static library cannot use implicit self APIs"));
                    }
                } NoSelf;
                F->body->visit(NoSelf);
                if (!M.WorldContext.IsEmpty())
                {
                    const auto* W =
                        M.Parameters.FindByPredicate([&](const auto& P) { return P.Name == M.WorldContext; });
                    if (!W || W->Type.GetKind() != EDasTypeKind::Object ||
                        W->Direction !=
                            TEXT("in") ||
                                 FindObject<UClass>(nullptr, *W->Type.TypePath)->HasAnyClassFlags(CLASS_Interface))
                        DSLFail(TEXT("WorldContext must name an input UObject reference"));
                }
                FDSLTypeCallVisitor TypeCalls(TypeScope);
                F->body = F->body->visit(TypeCalls);
                // Native scope restores the caller's world even when the implementation panics.
                auto Impl = F->clone();
                Impl->name = DSLUtf8(TEXT("__library_impl_")+ M.EntryPoint);
                Impl->classParent = nullptr;
                Impl->isClassMethod = false;
                Impl->isStaticClassMethod = false;
                Impl->isConstClassMethod = false;
                Impl->annotations.clear();
                Impl->generated = true;
                Impl->exports = false;
                Impl->privateFunction = false;
                for (auto& Arg : Impl->arguments)
                    Arg->init = nullptr;
                auto* SourceBody = new das::ExprBlock();
                SourceBody->at = F->at;
                auto* Inner = new das::ExprCall(F->at, Impl->name);
                for (const auto& Arg : F->arguments)
                    Inner->arguments.push_back(new das::ExprVar(F->at, Arg->name));
                if (M.ReturnType.GetKind() != EDasTypeKind::Void)
                {
                    auto* V = new das::Variable();
                    V->at = F->at;
                    V->name = "__ue_scoped_result";
                    if (DSLDirectStruct(F->result))
                        V->init = DSLCall(F->at,
                                          F->result->structType->module->name + "::" + F->result->structType->name, {});
                    V->type = DSLCopyType(F->result);
                    V->type->ref = false;
                    V->type->constant = false;
                    auto* Let = new das::ExprLet();
                    Let->at = F->at;
                    Let->variables.push_back(V);
                    SourceBody->list.push_back(Let);
                }
                auto* Closure = new das::ExprBlock();
                Closure->at = F->at;
                Closure->isClosure = true;
                Closure->returnType = new das::TypeDecl(das::Type::tVoid, F->at);
                if (M.ReturnType.GetKind() == EDasTypeKind::Void)
                    Closure->list.push_back(Inner);
                else
                    Closure->list.push_back(DSLContainer(F->result)
                                                ? static_cast<das::ExpressionPtr>(new das::ExprMove(
                                                      F->at, new das::ExprVar(F->at, "__ue_scoped_result"), Inner))
                                                : static_cast<das::ExpressionPtr>(new das::ExprCopy(
                                                      F->at, new das::ExprVar(F->at, "__ue_scoped_result"), Inner)));
                das::ExpressionPtr World = DSLCall(F->at, "ue_null_object", {});
                if (!M.WorldContext.IsEmpty())
                {
                    const auto Index =
                        M.Parameters.IndexOfByPredicate([&](const auto& P) { return P.Name == M.WorldContext; });
                    World = new das::ExprVar(F->at, F->arguments[size_t(Index)]->name);
                    if (DSLTypedReference(F->arguments[size_t(Index)]->type))
                        World = DSLDirectPack(F->at, F->arguments[size_t(Index)]->type, World,
                                              M.Parameters[Index].Type.TypePath);
                }
                SourceBody->list.push_back(DSLCall(F->at, "__ue_library_scope",
                                                   {World, new das::ExprConstBool(F->at, !M.WorldContext.IsEmpty()),
                                                    new das::ExprMakeBlock(F->at, Closure)}));
                if (M.ReturnType.GetKind() != EDasTypeKind::Void)
                {
                    auto* Ret = new das::ExprReturn(F->at, new das::ExprVar(F->at, "__ue_scoped_result"));
                    Ret->moveSemantics = DSLContainer(F->result);
                    SourceBody->list.push_back(Ret);
                }
                F->body = SourceBody;
                Generated.push_back(Impl);
                auto Wrapper = F->clone();
                Wrapper->name = DSLUtf8(M.EntryPoint);
                Wrapper->classParent = nullptr;
                Wrapper->isClassMethod = false;
                Wrapper->isStaticClassMethod = false;
                Wrapper->isConstClassMethod = false;
                Wrapper->annotations.clear();
                Wrapper->generated = true;
                Wrapper->privateFunction = false;
                Wrapper->exports = true;
                auto* Block = new das::ExprBlock();
                Block->at = F->at;
                auto* Call = new das::ExprCall(F->at, F->name);
                bool Outputs = false;
                auto Local = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Init)
                {
                    auto* V = new das::Variable();
                    V->at = F->at;
                    V->name = N;
                    V->type = DSLCopyType(T);
                    V->type->ref = false;
                    V->type->constant = false;
                    V->init = Init;
                    V->init_via_move = DSLContainer(T);
                    auto* Let = new das::ExprLet();
                    Let->at = F->at;
                    Let->variables.push_back(V);
                    Block->list.push_back(Let);
                };
                for (size_t I = 0; I < F->arguments.size(); ++I)
                {
                    auto Arg = Wrapper->arguments[I];
                    Arg->init = nullptr;
                    Arg->type = DSLCopyType(Arg->type);
                    Arg->type->ref = false;
                    Arg->type->constant = true;
                    das::ExpressionPtr Value = new das::ExprVar(F->at, Arg->name);
                    auto Original = F->arguments[I]->type;
                    if (DSLDelegate(Original) || Packed(Original))
                    {
                        Arg->type = ABI(Original);
                        Value =
                            DSLDirectUnpack(F->at, Original, Value,
                                            DSLContainer(Original) ? DSLDirectPath(Original, TypeFolder, TypeRegistry)
                                                                   : M.Parameters[int32(I)].Type.TypePath);
                    }
                    if (M.Parameters[int32(I)].Direction != TEXT("in"))
                    {
                        Outputs = true;
                        const auto N = "__ue_out_" + das::to_string(I);
                        Local(N, F->arguments[I]->type, Value);
                        Value = new das::ExprVar(F->at, N);
                    }
                    Call->arguments.push_back(Value);
                }
                das::ExpressionPtr ReturnValue = Call;
                if (Outputs)
                {
                    if (M.ReturnType.GetKind() == EDasTypeKind::Void)
                    {
                        Block->list.push_back(Call);
                        ReturnValue = nullptr;
                    }
                    else
                    {
                        Local("__ue_result", F->result, Call);
                        ReturnValue = new das::ExprVar(F->at, "__ue_result");
                    }
                    for (int32 I = 0; I < M.Parameters.Num(); ++I)
                        if (M.Parameters[I].Direction != TEXT("in"))
                        {
                            auto T = F->arguments[size_t(I)]->type;
                            const auto K = M.Parameters[I].Type.Kind;
                            const das::string Suffix =
                                K == TEXT("class")
                                          ? "class"
                                          : K == TEXT("object")
                                                      ? "object"
                                                      : K == TEXT("enum")
                                                                  ? "enum"
                                                                  : K == TEXT("struct")
                                                                              ? (DSLAdaptedBridge(
                                                                                     T, EDasCallBridge::FrameSlot)
                                                                                     ? DSLAdaptedBridge(
                                                                                           T, EDasCallBridge::FrameSlot)
                                                                                     : "struct")
                                                                          : DSLContainer(T) ? "struct"
                                                                                            : DSLScalarSuffix(K);
                            das::ExpressionPtr Value = new das::ExprVar(F->at, "__ue_out_" + das::to_string(I));
                            if (Packed(T))
                                Value = DSLDirectPack(F->at, T, Value,
                                                      DSLContainer(T) ? DSLDirectPath(T, TypeFolder, TypeRegistry)
                                                                      : M.Parameters[I].Type.TypePath);
                            Block->list.push_back(
                                DSLCall(F->at, "ue_frame_out_" + Suffix, {new das::ExprConstInt(F->at, I), Value}));
                        }
                }
                if (DSLDelegate(F->result) || Packed(F->result))
                {
                    Wrapper->result = ABI(F->result);
                    ReturnValue =
                        DSLDirectPack(F->at, F->result, ReturnValue,
                                      DSLContainer(F->result) ? DSLDirectPath(F->result, TypeFolder, TypeRegistry)
                                                              : M.ReturnType.TypePath);
                }
                if (ReturnValue)
                    Block->list.push_back(
                        M.ReturnType.GetKind() == EDasTypeKind::Void
                            ? ReturnValue
                            : static_cast<das::ExpressionPtr>(new das::ExprReturn(F->at, ReturnValue)));
                Wrapper->body = Block;
                Generated.push_back(Wrapper);
                C.Methods.Add(MoveTemp(M));
            }
            Model.Classes.Add(C);
            FString Wire, Issue;
            FDasClassSchema Schema;
            if (!Model.ToJson(Wire, Issue) || !Model.ToActorSchema(C.Id, Schema, Issue))
                DSLFail(Issue);
            for (const auto& F : HelpersGenerated)
                Generated.push_back(F);
            for (const auto& F : Generated)
                if (!St->module->addFunction(F, true))
                    DSLFail(TEXT("Static wrapper name collision"));
            Args.push_back(das::AnnotationArgument(DSLModelKey, DSLUtf8(Wire)));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
