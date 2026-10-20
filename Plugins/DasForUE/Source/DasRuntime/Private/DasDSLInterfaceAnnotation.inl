// Part of DasDSL.inl: The [ue_interface] structure annotation.
// Included in place; declaration order matters.
const char* DSLInterfaceKey = "_ue_interface_json";
struct FUEInterfaceAnnotation : das::StructureAnnotation
{
    FUEInterfaceAnnotation() : StructureAnnotation("ue_interface") {}
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(St->annotations);
            DSLAnnotation(St->annotations, "ue_interface");
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (const auto* Wire = Args.find(DSLInterfaceKey, das::Type::tString))
            {
                if (Wire->at.fileInfo)
                    DSLFail(TEXT("Reserved interface annotation argument"));
                return true;
            }
            DSLCommentTooltip(St->at, Args);
            DSLAnnotationArguments(Args, {TEXT("out")});
            if (!St->isClass || St->parent)
                DSLFail(TEXT("ue_interface requires a class without inheritance"));
            FDasAuthoringModule Model;
            Model.Id = DSLString(Args, "module_id", FPaths::GetBaseFilename(DSLText(St->at.fileInfo->name)));
            FDasDeclaredClass C;
            C.Id = DSLString(Args, "id", Model.Id + TEXT(".")+ DSLText(St->name));
            C.Name = DSLText(St->name);
            C.NativeParent = TEXT("/Script/CoreUObject.Interface");
            TSet<FString> LegacyOuts;
            for (const auto& A : Args)
            {
                if (A.name == "module_id" || A.name == "id" || A.name == "type_registry" || A.name == "type_folder")
                    continue;
                if (A.name == "BlueprintType")
                {
                    if (!DSLBool(A))
                        DSLFail(TEXT("BlueprintType must be true"));
                    continue;
                }
                if (A.name == "out")
                {
                    TArray<FString> Names;
                    DSLString(Args, "out").ParseIntoArray(Names, TEXT(","), false);
                    for (auto N : Names)
                    {
                        N = N.TrimStartAndEnd();
                        if (N.IsEmpty() || LegacyOuts.Contains(N))
                            DSLFail(TEXT("Duplicate/empty interface out name"));
                        LegacyOuts.Add(N);
                    }
                    continue;
                }
                if (DSLIsMetadata(A.name))
                    DSLMetadata(A, C.Metadata);
                else
                    DSLFail(TEXT("Unknown ue_interface annotation"));
            }
            const FString Folder = DSLString(Args, "type_folder"), Registry = DSLString(Args, "type_registry");
            if (!Folder.IsEmpty() && !Registry.IsEmpty())
                DSLFail(TEXT("Choose type_registry or type_folder"));
            const FDSLTypeScope TypeScope{Model.Id, Folder, Registry, &Group, St->module};
            auto Type = [&](const das::TypeDeclPtr& T, TMap<FString, FString>& Constraints, const FString& Prefix)
            {
                FDasDeclaredType D;
                D.Kind = DSLTypeName(T);
                if (DSLContainer(T))
                    return DSLContainerType(T, Folder, Registry);
                if (D.GetKind() == EDasTypeKind::Object)
                {
                    D.TypePath = DSLConstraint(
                        Constraints,
                        Prefix + TEXT("class"), DSLTypedInterface(T) ? EDSLTypeKind::Interface : EDSLTypeKind::Class,
                                      DSLTypedReference(T) ? DSLTypedReferencePath(T) : FString(), TypeScope);
                    if (D.TypePath.IsEmpty())
                        D.TypePath = TEXT("/Script/CoreUObject.Object");
                }
                else if (D.GetKind() == EDasTypeKind::Class)
                {
                    D.TypePath = DSLConstraint(
                        Constraints, Prefix + TEXT("class"), EDSLTypeKind::Class,
                                                   DSLTypedClass(T) ? DSLTypedClassPath(T) : FString(), TypeScope);
                    if (D.TypePath.IsEmpty())
                        DSLFail(TEXT("Interface class requires UEClass<T> or an explicit base constraint"));
                }
                else if (D.GetKind() == EDasTypeKind::Enum)
                {
                    D.TypePath = DSLConstraint(
                        Constraints, Prefix + TEXT("enum"), EDSLTypeKind::Enum,
                                                   DSLDirectEnum(T) ? DSLDirectPath(T, Folder, Registry) : FString(),
                                                   TypeScope);
                    if (D.TypePath.IsEmpty())
                        DSLFail(TEXT("Interface enum requires an explicit type constraint"));
                }
                else if (D.GetKind() == EDasTypeKind::Struct)
                {
                    const FString Inferred = DSLDirectStruct(T) ? DSLDirectPath(T, Folder, Registry)
                                             : T->annotation && T->annotation->name != "UEStructValue" ? DSLMathPath(T)
                                                                                                       : FString();
                    D.TypePath =
                        DSLConstraint(Constraints, Prefix + TEXT("struct"), EDSLTypeKind::Struct, Inferred, TypeScope);
                    if (D.TypePath.IsEmpty())
                        DSLFail(TEXT("Interface snapshot requires an explicit struct constraint"));
                }
                else if (D.GetKind() != EDasTypeKind::Byte && D.GetKind() != EDasTypeKind::Text &&
                         D.GetKind() != EDasTypeKind::Int && D.GetKind() != EDasTypeKind::Float &&
                         D.GetKind() != EDasTypeKind::Double && D.GetKind() != EDasTypeKind::Bool &&
                         D.GetKind() != EDasTypeKind::String && D.GetKind() != EDasTypeKind::Name &&
                         D.GetKind() != EDasTypeKind::Void)
                    DSLFail(TEXT("Unsupported authored interface signature type"));
                return D;
            };
            St->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent != St || F->generated)
                        return;
                    const auto Name = F->name.substr(F->name.find_last_of("`") + 1);
                    if (F->isStaticClassMethod || Name == St->name || Name == "finalize" || !St->findField(Name))
                        DSLFail(
                            TEXT("Interface definitions cannot declare static methods, constructors or finalizers"));
                });
            for (const auto& Field : St->fields)
            {
                if (Field.generated || Field.name == "__rtti")
                    continue;
                if (!Field.classMethod)
                    DSLFail(TEXT("Interface definitions cannot declare fields"));
                FDasDeclaredMethod M;
                M.Name = DSLText(Field.name);
                M.Id = C.Id + TEXT(".")+ M.Name;
                M.ScriptFile = FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));
                M.EntryPoint = M.Name;
                TMap<FString, FString> Directions, Constraints;
                das::FunctionPtr Function = nullptr;
                das::TypeDeclPtr Return;
                das::vector<das::TypeDeclPtr> Types;
                das::vector<das::string> Names;
                if (Field._abstract)
                {
                    if (Field.init || !Field.type || Field.type->baseType != das::Type::tFunction)
                        DSLFail(TEXT("Invalid legacy abstract interface signature"));
                    Return = Field.type->firstType;
                    M.bConst = !Field.type->argTypes.empty() && Field.type->argTypes[0]->constant;
                    for (size_t I = 1; I < Field.type->argTypes.size(); ++I)
                    {
                        Types.push_back(Field.type->argTypes[I]);
                        Names.push_back(Field.type->argNames[I]);
                    }
                }
                else
                {
                    St->module->functions.foreach (
                        [&](const das::FunctionPtr& Candidate)
                        {
                            if (Candidate->classParent == St && Candidate->name == St->name + "`" + Field.name)
                                Function = Candidate;
                        });
                    if (!Function || Function->isStaticClassMethod || Function->arguments.empty())
                        DSLFail(TEXT("Interface requires ordinary instance method stubs"));
                    int32 AnnotationCount = 0;
                    for (const auto* A : Function->annotations)
                        if (A->annotation->name == "ue_function")
                            ++AnnotationCount;
                    if (AnnotationCount != 1)
                        DSLFail(TEXT("Interface stub requires exactly one ue_function annotation"));
                    auto* Annotation =
                        const_cast<das::AnnotationDeclaration*>(DSLAnnotation(Function->annotations, "ue_function"));
                    DSLCommentTooltip(Function->at, Annotation->arguments, &Function->annotations);
                    DSLAnnotationArguments(Annotation->arguments, {TEXT("out"), TEXT("ref")});
                    M.bCallable = false;
                    for (const auto& A : Annotation->arguments)
                    {
                        if (A.name == "id")
                        {
                            M.Id = DSLString(Annotation->arguments, "id");
                            continue;
                        }
                        if (A.name == "BlueprintCallable")
                        {
                            M.bCallable = DSLBool(A);
                            continue;
                        }
                        if (A.name == "BlueprintPure")
                        {
                            M.bPure = DSLBool(A);
                            continue;
                        }
                        if (A.name == "out" || A.name == "ref")
                        {
                            DSLAddDirections(A, Directions);
                            continue;
                        }
                        const FString ConstraintKey = DSLText(A.name);
                        const bool ArgumentConstraint = ConstraintKey.StartsWith(
                            TEXT("arg_")) &&
                            (ConstraintKey.EndsWith(TEXT("_class")) ||
                                                    ConstraintKey.EndsWith(TEXT("_enum")) ||
                                                                           ConstraintKey.EndsWith(TEXT("_struct")));
                        if (ConstraintKey == TEXT("return_class") ||
                                                  ConstraintKey == TEXT("return_enum") ||
                                                                        ConstraintKey == TEXT("return_struct") ||
                                                                                              ArgumentConstraint)
                        {
                            if (A.type != das::Type::tString)
                                DSLFail(TEXT("Interface type constraint requires a type identity"));
                            Constraints.Add(ConstraintKey, DSLText(A.sValue));
                            continue;
                        }
                        if (DSLIsMetadata(A.name))
                            DSLMetadata(A, M.Metadata);
                        else
                            DSLFail(TEXT("Unsupported interface ue_function specifier: ")+ DSLText(A.name));
                    }
                    if (M.bPure)
                    {
                        const auto* Callable = Annotation->arguments.find("BlueprintCallable", das::Type::tBool);
                        if (Callable && !DSLBool(*Callable))
                            DSLFail(TEXT("BlueprintPure conflicts with BlueprintCallable=false"));
                        M.bCallable = true;
                    }
                    if (!M.bCallable)
                        DSLFail(TEXT("Interface stub requires BlueprintCallable or BlueprintPure"));
                    das::ExpressionPtr Body = Function->body;
                    if (Body && Body->rtti_isBlock())
                    {
                        auto* Outer = static_cast<das::ExprBlock*>(Body);
                        if (Outer->generated && Outer->list.size() == 1 && Outer->finalList.empty() &&
                            Outer->list[0]->rtti_isWith())
                        {
                            auto* With = static_cast<das::ExprWith*>(Outer->list[0]);
                            if (With->generated)
                                Body = With->body;
                        }
                    }
                    if (!Body || !Body->rtti_isBlock())
                        DSLFail(TEXT("Interface method body must be an empty declaration stub"));
                    auto* Stub = static_cast<das::ExprBlock*>(Body);
                    if (!Stub->list.empty() || !Stub->finalList.empty() || !Stub->arguments.empty())
                        DSLFail(TEXT("Interface method body must be an empty declaration stub"));
                    M.bConst = Function->isConstClassMethod;
                    Return = Function->result;
                    for (size_t I = 1; I < Function->arguments.size(); ++I)
                    {
                        if (Function->arguments[I]->init)
                            DSLFail(TEXT("Interface parameter defaults are unsupported"));
                        Types.push_back(Function->arguments[I]->type);
                        Names.push_back(Function->arguments[I]->name);
                    }
                }
                Return = DSLObjectNormalize(Return, Group, St->module);
                if (Function)
                    Function->result = Return;
                M.ReturnType = Type(Return, Constraints, TEXT("return_"));
                if (!Return || Return->ref)
                    DSLFail(TEXT("Interface return references unsupported"));
                for (size_t I = 0; I < Types.size(); ++I)
                {
                    Types[I] = DSLObjectNormalize(Types[I], Group, St->module);
                    const auto& T = Types[I];
                    if (Function)
                        Function->arguments[I + 1]->type = T;
                    FDasDeclaredParameter P;
                    P.Name = DSLText(Names[I]);
                    P.Id = M.Id + TEXT(".")+ P.Name;
                    P.Type = Type(T, Constraints, TEXT("arg_")+ P.Name + TEXT("_"));
                    if (!T || P.Type.GetKind() == EDasTypeKind::Void || (T->ref && T->constant))
                        DSLFail(TEXT("Interface parameters require value or var mutable reference types"));
                    const FString Key = M.Name + TEXT(".")+ P.Name;
                    const bool LegacyOut = LegacyOuts.Contains(Key);
                    const FString Direction = Directions.FindRef(P.Name);
                    if (LegacyOut && !Direction.IsEmpty())
                        DSLFail(TEXT("Do not mix class-level and method-level directions for one parameter"));
                    if (Field._abstract)
                    {
                        P.Direction = LegacyOut ? TEXT("out"): T->ref ? TEXT("inout"): TEXT("in");
                        if (LegacyOut && !T->ref)
                            DSLFail(TEXT("Interface out requires mutable reference"));
                    }
                    else
                    {
                        P.Direction =
                            DSLParameterDirection(Function->arguments[I + 1], LegacyOut ? TEXT("out"): Direction);
                    }

                    Directions.Remove(P.Name);
                    LegacyOuts.Remove(Key);
                    M.Parameters.Add(P);
                }
                if (!Constraints.IsEmpty())
                    DSLFail(TEXT("Unused interface type constraint"));
                if (!Directions.IsEmpty())
                    DSLFail(TEXT("Unknown interface out/ref parameter name"));
                if (M.Parameters.Num() > 16)
                    DSLFail(TEXT("Interface parameter count exceeds 16"));
                bool Output = M.ReturnType.GetKind() != EDasTypeKind::Void;
                for (const auto& P : M.Parameters)
                    Output |= P.Direction != TEXT("in");
                C.Methods.Add(M);
                if (Function)
                {
                    // Empty source bodies are declaration markers. Compiled direct calls trap.
                    auto* Trap = new das::ExprBlock();
                    Trap->at = Function->at;
                    Trap->generated = true;
                    Trap->list.push_back(
                        DSLCall(Function->at, "panic",
                                {new das::ExprConstString(Function->at,
                                                          "ue_interface declaration stub cannot be called directly")}));
                    if (M.ReturnType.GetKind() != EDasTypeKind::Void)
                    {
                        auto* Default = new das::ExprMakeStruct(Function->at);
                        Default->makeType = DSLCopyType(Return);
                        Default->alwaysUseInitializer = true;
                        das::ExpressionPtr Placeholder = Default;
                        if (DSLDirectStruct(Return))
                            Placeholder = DSLCall(Function->at, Return->structType->name, {});
                        if (DSLDirectEnum(Return))
                        {
                            if (Return->enumType->list.empty())
                                DSLFail(TEXT("Empty interface enum declaration"));
                            Placeholder =
                                new das::ExprField(Function->at, new das::ExprVar(Function->at, Return->enumType->name),
                                                   Return->enumType->list.front().name);
                        }
                        auto* Result = new das::ExprReturn(Function->at, Placeholder);
                        Result->moveSemantics = DSLContainer(Return);
                        Trap->list.push_back(Result);
                    }
                    Function->body = Trap;
                }
            }
            if (C.Methods.IsEmpty() || C.Methods.Num() > 64 || !LegacyOuts.IsEmpty())
                DSLFail(TEXT("Expected 1..64 interface methods and known legacy out names"));
            Model.Classes.Add(C);
            FString Wire, Issue;
            if (!Model.ToJson(Wire, Issue))
                DSLFail(Issue);
            Args.push_back(das::AnnotationArgument(DSLInterfaceKey, DSLUtf8(Wire)));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
