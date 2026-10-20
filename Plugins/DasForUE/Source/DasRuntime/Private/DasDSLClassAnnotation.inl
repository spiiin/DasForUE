// Part of DasDSL.inl: The [ue_class] structure annotation: validation, lowering and authoring model emission.
// Included in place; declaration order matters.
struct FUEClassAnnotation : das::StructureAnnotation
{
    FUEClassAnnotation() : StructureAnnotation("ue_class") {}
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(St->annotations);
            DSLAnnotation(St->annotations, "ue_class");
            if (CollectTypesOnly)
                return true;
            if (const auto* Done = Args.find("_ue_implements_resolved", das::Type::tBool))
            {
                if (Done->at.fileInfo)
                    DSLFail(TEXT("Reserved implements argument"));
                return true;
            }
            auto* Wire = Args.find(DSLModelKey, das::Type::tString);
            FDasAuthoringModule Model;
            FString Issue;
            if (!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue), Model, Issue))
                DSLFail(Issue);
            auto* Declared = Model.Classes.FindByPredicate([&](const auto& C) { return C.Name == DSLText(St->name); });
            if (!Declared)
                DSLFail(TEXT("Missing current class declaration"));
            auto& C = *Declared;
            const bool LegacyInterfaces = C.Interfaces.Num() != 0;
            for (const auto* A : St->annotations)
                if (A->annotation->name == "ue_implements")
                {
                    const FString Path =
                        DSLResolveType(EDSLTypeKind::Interface, DSLString(A->arguments, "interface"),
                                       FDSLTypeScope{Model.Id, DSLString(Args, "type_folder"),
                                                     DSLString(Args, "type_registry"), &Group, St->module});
                    if (C.Interfaces.Contains(Path))
                        DSLFail(TEXT("Duplicate interface implementation: ")+ Path);
                    C.Interfaces.Add(Path);
                }
            for (const auto* A : St->annotations)
                if (A->annotation->name == "ue_component")
                {
                    FDasDeclaredComponent Component;
                    Component.Name = DSLString(A->arguments, "name");
                    Component.Id = DSLString(A->arguments, "id", C.Id + TEXT(".")+ Component.Name);
                    Component.ScriptFile = DSLString(A->arguments, "script");
                    Component.ClassName = DSLString(A->arguments, "type");
                    C.Components.Add(Component);
                }
            if (const auto* A = DSLAnnotation(St->annotations, "ue_async_physics"))
                C.AsyncPhysics = DSLAsyncPhysics(A->arguments);
            C.Components.Sort([](const auto& A, const auto& B) { return A.Id < B.Id; });
            // Legacy scalar adapters rename their single input before class annotations
            // are attached. Interface contracts must retain the source parameter name.
            if (!LegacyInterfaces && !C.Interfaces.IsEmpty())
                St->module->functions.foreach (
                    [&](const das::FunctionPtr& F)
                    {
                        if (F->classParent != St || !DSLAnnotation(F->annotations, "ue_function") ||
                            F->arguments.size() != 2)
                            return;
                        auto* M = C.Methods.FindByPredicate([&](const auto& Candidate)
                                                            { return Candidate.Name == DSLText(F->name); });
                        if (!M || M->Parameters.Num() != 1)
                            return;
                        auto& P = M->Parameters[0];
                        const FString K = DSLTypeName(F->arguments[1]->type);
                        if (P.Direction ==
                            TEXT("in") && P.Name == TEXT("Input") && K == M->ReturnType.Kind &&
                                                         (K == TEXT("int") || K == TEXT("float") || K == TEXT("bool")))
                        {
                            P.Name = DSLText(F->arguments[1]->name);
                            P.Id = M->Id + TEXT(".")+ P.Name;
                        }
                    });
            FString Json;
            if (!Model.ToJson(Json, Issue))
                DSLFail(Issue);
            FDasClassSchema Schema;
            if (!Model.ToActorSchema(C.Id, Schema, Issue))
                DSLFail(Issue);
            for (auto& Arg : Args)
                if (Arg.name == DSLModelKey)
                    Arg.sValue = DSLUtf8(Json);
            Args.push_back(das::AnnotationArgument("_ue_implements_resolved", true));
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
            FString NativeHint = DSLString(Args, "parent", TEXT("/Script/Engine.Actor"));
            auto* NativeRoot = St;
            while (NativeRoot->parent)
                NativeRoot = NativeRoot->parent;
            const auto Marker = DSLNativeBasePath(NativeRoot);
            if (!Marker.IsEmpty())
                NativeHint = Marker;
            else if (NativeRoot != St)
                if (const auto* ParentAnnotation = DSLAnnotation(NativeRoot->annotations, "ue_class"))
                    NativeHint = DSLString(ParentAnnotation->arguments, "parent", TEXT("/Script/Engine.Actor"));
            if (!NativeHint.StartsWith(TEXT("/")))
                NativeHint = DSLResolveType(
                    EDSLTypeKind::Class, NativeHint,
                    FDSLTypeScope{DSLString(Args, "module_id", FPaths::GetBaseFilename(DSLText(St->at.fileInfo->name))),
                                  DSLString(Args, "type_folder"), DSLString(Args, "type_registry"), &Group,
                                  St->module});
            TGuardValue<FString> NativeDefaultsHint(DSLNativeDefaultsParentHint, NativeHint);
            DSLExpandDeclarationGroups(St, true);
            DSLNormalizeInlineCallbacks(St, Group);
            DSLNormalizeMethodReferences(St, Group);
            DSLNormalizeIdentityDefaults(St, Group);
            DSLPrepareComputedDefaults(St);
            const auto ComponentDefaults = DSLCollectComponentDefaults(St);
            DSLCommentTooltip(St->at, Args);
            if (!CollectTypesOnly)
                DSLPrepareClassDefaults(St, ComponentDefaults);
            for (auto& Field : St->fields)
                if (!Field.classMethod && !Field.generated && Field.name != "__rtti")
                    DSLCommentTooltip(Field.at, Field.annotation);
            DSLAnnotationArguments(Args, {TEXT("implements")});
            if (CollectTypesOnly)
            {
                // Declaration collection runs before authored assets/registries exist.
                // Erase syntax markers only; no body runs in this compilation pass.
                for (auto& Field : St->fields)
                    if (DSLContainer(Field.type))
                        Field.init = nullptr;
                FDSLTypeCallVisitor SyntaxOnly(FDSLTypeScope{}, false);
                for (const auto& Field : St->fields)
                    if (Field.init)
                        Field.init->visit(SyntaxOnly);
                St->module->functions.foreach (
                    [&](const das::FunctionPtr& Function)
                    {
                        if (Function->classParent == St && Function->body &&
                            DSLAnnotation(Function->annotations, "ue_function"))
                            Function->body = Function->body->visit(SyntaxOnly);
                    });
                FDasDeclaredClass NativeHost;
                NativeHost.NativeParent = NativeHint;
                das::vector<das::FunctionPtr> NativeHelpers;
                TSet<FString> NativePaths;
                das::vector<das::FunctionPtr> NativeSources;
                St->module->functions.foreach (
                    [&](const das::FunctionPtr& F)
                    {
                        if (F->classParent == St && !F->generated && F->body && !F->arguments.empty())
                            NativeSources.push_back(F);
                    });
                for (const auto& F : NativeSources)
                {
                    DSLValidateConstSource(F, St);
                    FDSLNativeSourceVisitor Native(NativeHost, F->arguments[0]->name, St, FString(), FString(), Group,
                                                   NativeHelpers, NativePaths);
                    Native.NativeOnly = true;
                    F->body = F->body->visit(Native);
                    if (!Native.Error.IsEmpty())
                        DSLFail(Native.Error);
                }
                for (const auto& F : NativeHelpers)
                    if (!St->module->addFunction(F, true))
                        DSLFail(TEXT("Native array declaration helper collision"));
                return true;
            }
            if (const auto* Wire = Args.find(DSLModelKey, das::Type::tString))
            {
                if (Wire->at.fileInfo)
                    DSLFail(TEXT("Reserved internal annotation argument"));
                return true;
            }
            St->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent == St && !F->generated)
                        DSLValidateConstSource(F, St);
                });
            FDasAuthoringModule Model;
            const FString Script = FPaths::GetCleanFilename(DSLText(St->at.fileInfo->name));
            Model.Id = DSLString(Args, "module_id", FPaths::GetBaseFilename(Script));
            const FString TypeFolder = DSLString(Args, "type_folder"), TypeRegistry = DSLString(Args, "type_registry");
            if (!TypeFolder.IsEmpty() && !TypeRegistry.IsEmpty())
                DSLFail(TEXT("Choose type_registry or type_folder"));
            das::vector<das::FunctionPtr> HelpersGenerated;
            TSet<FString> Helpers;
            FDasDeclaredClass C;
            C.SourceScript = Script;
            C.Name = DSLText(St->name);
            C.Id = DSLString(Args, "id", Model.Id + TEXT(".")+ C.Name);
            const FDSLTypeScope TypeScope{Model.Id, TypeFolder, TypeRegistry, &Group, St->module};
            FDasDeclaredClass BaseEffective;
            FString BaseId;
            const FString NativeBase = DSLNativeBasePath(St->parent);
            const bool Derived = St->parent && NativeBase.IsEmpty();
            if (St->parent && Args.find("parent", das::Type::tString))
                DSLFail(TEXT("Use either class inheritance or parent annotation"));
            if (Derived)
            {
                if (Args.find("parent", das::Type::tString))
                    DSLFail(TEXT("Use either class inheritance or parent annotation"));
                const auto* A = DSLAnnotation(St->parent->annotations, "ue_class");
                const auto* Wire = A ? A->arguments.find(DSLModelKey, das::Type::tString) : nullptr;
                FDasAuthoringModule BaseModel;
                FString Issue;
                if (!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue), BaseModel, Issue))
                    DSLFail(TEXT("Authored inheritance requires a ue_class parent declaration"));
                const auto* Base = BaseModel.Classes.FindByPredicate([&](const auto& P)
                                                                     { return P.Name == DSLText(St->parent->name); });
                if (!Base || !BaseModel.ResolveClass(Base->Id, BaseEffective, Issue))
                    DSLFail(Issue.IsEmpty() ? TEXT("Missing authored parent"): Issue);
                BaseId = Base->Id;
                Model.Classes = BaseModel.Classes;
                C.NativeParent = BaseEffective.NativeParent;
                C.Properties = BaseEffective.Properties;
                C.Components = BaseEffective.Components;
                C.Interfaces = BaseEffective.Interfaces;
                C.Callbacks = BaseEffective.Callbacks;
            }
            else
                C.NativeParent = NativeBase.IsEmpty()
                    ? DSLResolveType(EDSLTypeKind::Class, DSLString(Args, "parent", TEXT("/Script/Engine.Actor")),
                                                                    TypeScope)
                                     : NativeBase;
            if (const auto* A = DSLAnnotation(St->annotations, "ue_async_physics"))
                C.AsyncPhysics = DSLAsyncPhysics(A->arguments);
            else if (Derived)
                C.AsyncPhysics = BaseEffective.AsyncPhysics;
            // Identity/host is available before resolving self-referential property/method types.
            DSLActiveRefDeclarations.Add(Script + TEXT("::")+ C.Name, C);
            for (const auto& A : Args)
            {
                if (A.name == "module_id" || A.name == "id" || A.name == "parent" || A.name == "type_folder" ||
                    A.name == "type_registry")
                    continue;
                if (A.name == "implements")
                {
                    TArray<FString> Items;
                    DSLString(Args, "implements").ParseIntoArray(Items, TEXT(","), false);
                    for (auto I : Items)
                    {
                        I = I.TrimStartAndEnd();
                        I = DSLResolveType(EDSLTypeKind::Interface, I, TypeScope);
                        if (C.Interfaces.Contains(I))
                            DSLFail(TEXT("Empty/duplicate interface path"));
                        C.Interfaces.Add(I);
                    }
                    if (C.Interfaces.IsEmpty())
                        DSLFail(TEXT("Expected interface paths"));
                    continue;
                }
                if (A.name == "Blueprintable")
                {
                    if (!DSLBool(A))
                        DSLFail(TEXT("Adapter requires Blueprintable"));
                    continue;
                }
                if (A.name == "primary_asset_type")
                {
                    if (A.type != das::Type::tString)
                        DSLFail(TEXT("primary_asset_type must be a string"));
                    C.Metadata.Add(TEXT("PrimaryAssetType"), DSLText(A.sValue));
                    continue;
                }
                if (DSLIsMetadata(A.name))
                    DSLMetadata(A, C.Metadata);
                else
                    DSLFail(TEXT("Unknown ue_class annotation: ")+ DSLText(A.name));
            }

            for (auto& Field : St->fields)
                if (!Field.classMethod && !Field.generated && Field.name != "__rtti")
                    Field.type = DSLObjectNormalize(Field.type, Group, St->module);
            St->module->functions.foreach (
                [&](const das::FunctionPtr& Method)
                {
                    if (Method->classParent == St && DSLAnnotation(Method->annotations, "ue_function"))
                    {
                        Method->result = DSLObjectNormalize(Method->result, Group, St->module);
                        for (size_t I = 1; I < Method->arguments.size(); ++I)
                            Method->arguments[I]->type =
                                DSLObjectNormalize(Method->arguments[I]->type, Group, St->module);
                    }
                });
            for (auto& F : St->fields)
            {
                // Pinned makeClassRtti adds __rtti without setting generated.
                const bool InheritedDefault =
                    F.inherited &&
                    ComponentDefaults.ContainsByPredicate(
                        [&](const auto& D) { return D.Component.IsEmpty() && D.Property == DSLText(F.name); });
                if (F.classMethod || F.generated || (F.inherited && !InheritedDefault) || F.name == "__rtti")
                    continue;
                const auto* InheritedProperty =
                    InheritedDefault ? BaseEffective.Properties.FindByPredicate([&](const auto& P)
                                                                                { return P.Name == DSLText(F.name); })
                                     : nullptr;
                if (InheritedDefault)
                {
                    if (!InheritedProperty || !InheritedProperty->bEditable)
                        DSLFail(TEXT("Inherited default requires an editable authored property: ")+ DSLText(F.name));
                    C.Properties.RemoveAll([&](const auto& P) { return P.Id == InheritedProperty->Id; });
                }
                if (DSLComponentField(F, C))
                    continue;
                FDasDeclaredProperty P;
                P.Name = DSLText(F.name);
                P.Id = C.Id + TEXT(".")+ P.Name;
                bool Marked = false, ReadWrite = false, ReadOnly = false, EditAnywhere = false;
                for (const auto& A : F.annotation)
                {
                    if (A.name == "ue_property")
                    {
                        Marked = true;
                        if (A.type == das::Type::tBool)
                        {
                            if (!A.bValue)
                                DSLFail(TEXT("ue_property must be enabled"));
                        }
                        else if (A.type == das::Type::tString)
                        {
                            if (A.sValue == "EditAnywhere")
                            {
                                P.bEditable = true;
                                EditAnywhere = true;
                            }
                            else if (A.sValue == "BlueprintReadWrite")
                            {
                                P.bReadOnly = false;
                                ReadWrite = true;
                            }
                            else if (A.sValue == "BlueprintReadOnly")
                            {
                                P.bReadOnly = true;
                                ReadOnly = true;
                            }
                            else if (A.sValue == "BlueprintAssignable" && DSLDelegate(F.type) &&
                                     DSLAnnotation(F.type->structType->annotations, "ue_multicast"))
                            {
                            }
                            else
                                DSLFail(TEXT("Unsupported property specifier: ")+ DSLText(A.sValue));
                        }
                        else
                            DSLFail(TEXT("ue_property requires flags or true"));
                    }
                    else if (A.name == "ue_class_base")
                    {
                        if (A.type != das::Type::tString || DSLTypeName(F.type) != TEXT("class"))
                            DSLFail(TEXT("ue_class_base requires a UEClassHandle field and native base path"));
                        P.Type.TypePath = DSLText(A.sValue);
                    }
                    else if (A.name == "ue_id")
                    {
                        if (A.type != das::Type::tString)
                            DSLFail(TEXT("ue_id must be string"));
                        P.Id = DSLText(A.sValue);
                    }
                    else if (DSLIsMetadata(A.name) || A.name == "AssetBundles")
                        DSLMetadata(A, P.Metadata);
                    else
                        DSLFail(TEXT("Unknown field metadata: ")+ DSLText(A.name));
                }
                if (ReadWrite && ReadOnly)
                    DSLFail(TEXT("Conflicting BlueprintReadOnly/BlueprintReadWrite specifiers"));
                if (!Marked)
                    DSLFail(TEXT("Script-private fields unsupported; add @ue_property: ")+ P.Name);
                if (F.init)
                {
                    FDSLTypeCallVisitor TypeCalls(TypeScope);
                    F.init->visit(TypeCalls);
                }
                if (DSLDelegate(F.type))
                {
                    const bool Multi = DSLAnnotation(F.type->structType->annotations, "ue_multicast") != nullptr;
                    if (EditAnywhere)
                        DSLFail(TEXT("Delegate fields do not support EditAnywhere; bind them at runtime"));
                    if (!Marked || F.init || (Multi && (ReadWrite || ReadOnly)))
                        DSLFail(
                            TEXT("Delegate fields require ue_property and no default; events use BlueprintAssignable"));
                    const auto* A = DSLDelegateAnnotation(F.type);
                    P.DelegateWire = DSLString(A->arguments, "_ue_delegate_wire");
                    P.Type.Kind = Multi ? TEXT("event"): TEXT("delegate");
                    P.Type.TypePath = TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*P.DelegateWire) + TEXT(".Type");
                    P.bEditable = false;
                    if (Multi)
                        P.bReadOnly = false;
                    C.Properties.Add(P);
                    auto* Init = new das::ExprMakeStruct(F.at);
                    Init->makeType = DSLCopyType(F.type);
                    Init->useInitializer = true;
                    Init->alwaysUseInitializer = true;
                    F.init = Init;
                    continue;
                }
                const FString FieldType = DSLTypeName(F.type);
                if (!F.type || F.type->ref || FieldType.IsEmpty() || FieldType == TEXT("void"))
                    DSLFail(TEXT("Adapter supports int/float/bool/string/double/name properties: ")+ P.Name);
                P.Type.Kind = FieldType;
                if (DSLSoft(F.type))
                {
                    P.Type.Kind = TEXT("struct");
                    P.Type.TypePath = DSLSoftPath(F.type);
                    if (F.init)
                    {
                        if (F.init->__rtti != das::string("ExprCallMacro"))
                            DSLFail(TEXT("Soft defaults require ue_soft(type<SoftType>, literal_path)"));
                        auto* Init = static_cast<das::ExprCallMacro*>(F.init);
                        if (Init->name != "ue_soft" || Init->arguments.size() != 2 ||
                            Init->arguments[1]->__rtti != das::string("ExprConstString"))
                            DSLFail(TEXT("Soft default path must be a string literal"));
                        if (!Init->arguments[0]->rtti_isTypeDecl())
                            DSLFail(TEXT("Soft default requires a type argument"));
                        auto DefaultType = DSLObjectNormalize(
                            static_cast<das::ExprTypeDecl*>(Init->arguments[0])->typeexpr, Group, St->module);
                        if (!DSLSoft(DefaultType) || DSLSoftPath(DefaultType) != P.Type.TypePath)
                            DSLFail(TEXT("Soft default type must match field type"));
                        auto* Text = static_cast<das::ExprConstString*>(Init->arguments[1]);
                        FString Issue;
                        if (!ValidateDasSoftPath(DSLText(Text->text), Issue))
                            DSLFail(Issue);
                        auto Object = MakeShared<FJsonObject>();
                        Object->SetStringField(TEXT("Values"), DSLText(Text->text));
                        P.Default.Kind = TEXT("string");
                        FJsonSerializer::Serialize(Object, TJsonWriterFactory<>::Create(&P.Default.String));
                    }
                    C.Properties.Add(P);
                    continue;
                }
                if (FieldType == TEXT("text"))
                {
                    P.Default = DSLTextDefault(F.init);
                    C.Properties.Add(P);
                    continue;
                }
                if (FieldType == TEXT("byte"))
                {
                    P.Default = DSLByteDefault(F.init);
                    C.Properties.Add(P);
                    continue;
                }
                if (FieldType == TEXT("class"))
                {
                    if (DSLTypedClass(F.type))
                    {
                        const FString Inferred = DSLTypedClassPath(F.type);
                        if (!P.Type.TypePath.IsEmpty() && P.Type.TypePath != Inferred)
                            DSLFail(TEXT("Conflicting typed class property constraint"));
                        P.Type.TypePath = Inferred;
                    }
                    auto* Base = FindObject<UClass>(nullptr, *P.Type.TypePath);
                    if (!P.Type.TypePath.StartsWith(TEXT("/Script/")) || !Base ||
                                                    Base->HasAnyClassFlags(CLASS_Interface))
                        DSLFail(TEXT("Class property requires ue_class_base with loaded native base"));
                    P.Default = DSLReferencePropertyDefault(F, true);
                    C.Properties.Add(MoveTemp(P));
                    continue;
                }
                if (DSLContainer(F.type))
                {
                    P.Type = DSLContainerType(F.type, TypeFolder, TypeRegistry);
                    const FString Path = DSLDirectPath(F.type, TypeFolder, TypeRegistry);
                    if (F.init)
                    {
                        P.Default.Kind = TEXT("string");
                        P.Default.String = DSLContainerDefault(F, Path, Group, St->module);
                    }
                    DSLEnsureStructHelpers(F.type, Path, Group, St->module, HelpersGenerated, Helpers);
                    C.Properties.Add(P);
                    continue;
                }
                if (!DSLMathPath(F.type).IsEmpty())
                {
                    P.Type.TypePath = DSLMathPath(F.type);
                    P.Default.Kind = TEXT("string");
                    P.Default.String = DSLMathPropertyDefault(F);
                    C.Properties.Add(P);
                    continue;
                }
                if (DSLDirectEnum(F.type))
                {
                    P.Type.TypePath = DSLDirectPath(F.type, TypeFolder, TypeRegistry);
                    P.Default.Kind = TEXT("string");
                    if (F.init && F.init->__rtti == das::string("ExprConstEnumeration"))
                        P.Default.String = DSLText(static_cast<das::ExprConstEnumeration*>(F.init)->text);
                    else if (F.init && F.init->rtti_isField())
                    {
                        auto* Init = static_cast<das::ExprField*>(F.init);
                        if (!Init->value || !Init->value->rtti_isVar() ||
                            static_cast<das::ExprVar*>(Init->value)->name != F.type->enumType->name)
                            DSLFail(TEXT("Direct enum initializer must name its declared enum"));
                        P.Default.String = DSLText(Init->name);
                    }
                    else
                        DSLFail(TEXT("Direct enum property requires named enum literal initializer: ")+
                                     (F.init ? DSLText(F.init->__rtti) : TEXT("none")));
                    if (P.Default.String.Contains(TEXT(".")))
                        P.Default.String = P.Default.String.RightChop(P.Default.String.Find(TEXT(".")) + 1);
                    if (P.Default.String.Contains(TEXT("::")))
                        P.Default.String = P.Default.String.RightChop(P.Default.String.Find(TEXT("::")) + 2);
                    FString Issue;
                    auto* E = ResolveDasEnum(P.Type.TypePath, Issue);
                    if (!E || !ValidateDasEnumNumber(P.Type.TypePath, E->GetValueByNameString(P.Default.String), Issue))
                        DSLFail(Issue);
                    C.Properties.Add(P);
                    continue;
                }
                if (DSLDirectStruct(F.type))
                {
                    P.Type.TypePath = DSLDirectPath(F.type, TypeFolder, TypeRegistry);
                    DSLEnsureStructHelpers(F.type, P.Type.TypePath, Group, St->module, HelpersGenerated, Helpers);
                    P.Default.Kind = TEXT("string");
                    P.Default.String = TEXT("");
                    if (F.init && F.init->rtti_isCall())
                    {
                        auto* DefaultCall = static_cast<das::ExprCall*>(F.init);
                        if (DefaultCall->name == "ue_default_text")
                        {
                            if (DefaultCall->arguments.size() != 1 ||
                                DefaultCall->arguments[0]->__rtti != das::string("ExprConstString"))
                                DSLFail(TEXT("ue_default_text requires a literal string"));
                            P.Default.String =
                                DSLText(static_cast<das::ExprConstString*>(DefaultCall->arguments[0])->text);
                            F.init = nullptr;
                            FInstancedStruct Value;
                            FString Issue;
                            if (!ParseDasStructDefault(P.Type.TypePath, P.Default.String, Value, Issue))
                                DSLFail(Issue);
                        }
                    }
                    if (F.init)
                    {
                        bool Empty = false;
                        if (F.init->rtti_isCall())
                        {
                            auto* I = static_cast<das::ExprCall*>(F.init);
                            Empty = I->name == F.type->structType->name && I->arguments.empty();
                        }
                        else if (F.init->rtti_isMakeStruct())
                        {
                            auto* I = static_cast<das::ExprMakeStruct*>(F.init);
                            Empty = I->structs.empty() && !I->block;
                        }
                        if (!Empty)
                            DSLFail(TEXT("Direct struct defaults require empty struct constructor; field overrides are not supported yet"));
                    }
                    C.Properties.Add(P);
                    continue;
                }
                if (FieldType == TEXT("enum"))
                {
                    if (!F.init || !F.init->rtti_isCall())
                        DSLFail(TEXT("Enum field requires ue_enum_value(path, member)"));
                    auto* Call = static_cast<das::ExprCall*>(F.init);
                    if (Call->name != "ue_enum_value" || Call->arguments.size() != 2 ||
                        Call->arguments[0]->__rtti != das::string("ExprConstString") ||
                        Call->arguments[1]->__rtti != das::string("ExprConstString"))
                        DSLFail(TEXT("Enum initializer requires literal path/member"));
                    P.Type.TypePath = DSLResolveType(
                        EDSLTypeKind::Enum, DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),
                        TypeScope);
                    static_cast<das::ExprConstString*>(Call->arguments[0])->text = DSLUtf8(P.Type.TypePath);
                    P.Default.Kind = TEXT("string");
                    P.Default.String = DSLText(static_cast<das::ExprConstString*>(Call->arguments[1])->text);
                    FString Issue;
                    auto* E = ResolveDasEnum(P.Type.TypePath, Issue);
                    if (!E || !ValidateDasEnumNumber(P.Type.TypePath, E->GetValueByNameString(P.Default.String), Issue))
                        DSLFail(Issue);
                    C.Properties.Add(P);
                    continue;
                }
                if (DSLTypedReference(F.type))
                {
                    P.Type.TypePath = DSLTypedReferencePath(F.type);
                    if (DSLAuthoredView(F.type))
                        P.Type.AuthoredId = DSLReferenceDeclaration(F.type).Id;
                    P.Default = DSLReferencePropertyDefault(F, false);
                    C.Properties.Add(MoveTemp(P));
                    continue;
                }
                if (FieldType == TEXT("object"))
                {
                    if (!F.init || !F.init->rtti_isCall())
                        DSLFail(TEXT("Object field requires literal ue_object_value(native_class_path) initializer"));
                    auto* Call = static_cast<das::ExprCall*>(F.init);
                    if (Call->name != "ue_object_value" || Call->arguments.size() != 1 ||
                        Call->arguments[0]->__rtti != das::string("ExprConstString"))
                        DSLFail(TEXT("Object field initializer requires one literal native class path"));
                    P.Type.TypePath = DSLResolveType(
                        EDSLTypeKind::Class, DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),
                        TypeScope);
                    static_cast<das::ExprConstString*>(Call->arguments[0])->text = DSLUtf8(P.Type.TypePath);
                    P.Default.Kind = TEXT("null");
                    C.Properties.Add(MoveTemp(P));
                    continue;
                }
                if (FieldType == TEXT("struct"))
                {
                    if (F.type->annotation->name != "UEStructValue" || !F.init || !F.init->rtti_isCall())
                        DSLFail(TEXT("Struct property requires UEStructValue and literal ue_struct_value/default initializer"));
                    auto* Call = static_cast<das::ExprCall*>(F.init);
                    const bool Full = Call->name == "ue_struct_default";
                    if ((!Full && Call->name != "ue_struct_value") || Call->arguments.size() != (Full ? 2 : 1))
                        DSLFail(TEXT("Invalid struct property initializer"));
                    for (auto* Arg : Call->arguments)
                        if (Arg->__rtti != das::string("ExprConstString"))
                            DSLFail(TEXT("Struct initializer arguments must be literal strings"));
                    P.Type.TypePath = DSLResolveType(
                        EDSLTypeKind::Struct, DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text),
                        TypeScope);
                    static_cast<das::ExprConstString*>(Call->arguments[0])->text = DSLUtf8(P.Type.TypePath);
                    P.Default.Kind = TEXT("string");
                    P.Default.String =
                        Full ? DSLText(static_cast<das::ExprConstString*>(Call->arguments[1])->text) : FString();
                    FInstancedStruct Default;
                    FString Issue;
                    if (!ParseDasStructDefault(P.Type.TypePath, P.Default.String, Default, Issue))
                        DSLFail(Issue);
                    C.Properties.Add(MoveTemp(P));
                    continue;
                }
                if (F.init)
                {
                    P.Default.Kind = TEXT("number");
                    if (F.init->__rtti == das::string("ExprConstInt"))
                        P.Default.Number = static_cast<das::ExprConstInt*>(F.init)->getValue();
                    else if (F.init->__rtti == das::string("ExprConstFloat"))
                        P.Default.Number = static_cast<das::ExprConstFloat*>(F.init)->getValue();
                    else if (F.init->__rtti == das::string("ExprConstBool"))
                    {
                        P.Default.Kind = TEXT("bool");
                        P.Default.Boolean = static_cast<das::ExprConstBool*>(F.init)->getValue();
                    }
                    else if (F.init->__rtti == das::string("ExprConstDouble"))
                        P.Default.Number = static_cast<das::ExprConstDouble*>(F.init)->getValue();
                    else if (F.init->__rtti == das::string("ExprConstString"))
                    {
                        P.Default.Kind = TEXT("string");
                        P.Default.String = DSLText(static_cast<das::ExprConstString*>(F.init)->text);
                    }
                    else if (P.Type.Kind == TEXT("name") && F.init->rtti_isCall())
                    {
                        auto* Call = static_cast<das::ExprCall*>(F.init);
                        if (Call->name != "ue_name_value" || Call->arguments.size() != 1 ||
                            Call->arguments[0]->__rtti != das::string("ExprConstString"))
                            DSLFail(TEXT("Name default requires ue_name_value with literal string"));
                        P.Default.Kind = TEXT("string");
                        P.Default.String = DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text);
                    }
                    else
                        DSLFail(TEXT("Only literal property defaults supported: ")+ P.Name);
                }
                C.Properties.Add(MoveTemp(P));
            }
            if (C.NativeParent != TEXT("/Script/Engine.Character") &&
                                       !C.Components.ContainsByPredicate([](const auto& V) { return V.bRoot; }))
                for (auto& V : C.Components)
                {
                    auto* Native = V.NativeClass.IsEmpty() ? nullptr : FindObject<UClass>(nullptr, *V.NativeClass);
                    if (Native && Native->IsChildOf(USceneComponent::StaticClass()) && V.Attach.IsEmpty())
                    {
                        V.bRoot = true;
                        break;
                    }
                }
            for (const auto& D : ComponentDefaults)
                if (D.Component.IsEmpty())
                    if (const auto* Base = BaseEffective.Properties.FindByPredicate([&](const auto& P)
                                                                                    { return P.Name == D.Property; }))
                    {
                        auto* Parsed =
                            C.Properties.FindByPredicate([&](const auto& P) { return P.Name == D.Property; });
                        if (!Parsed || Parsed->Type.Identity() != Base->Type.Identity())
                            DSLFail(TEXT("Inherited default cannot change the property type: ")+ D.Property);
                        FDasPropertyDefaultOverride Override;
                        Override.Id = Base->Id;
                        Override.Name = Base->Name;
                        Override.Default = Parsed->Default;
                        C.PropertyOverrides.Add(Override);
                        *Parsed = *Base;
                        Parsed->Default = Override.Default;
                    }
            if (auto* NativeBaseClass = DasNativeDefaultBase(C.NativeParent))
            {
                for (const auto& P : C.Properties)
                    if (NativeBaseClass->FindPropertyByName(*P.Name))
                        DSLFail(TEXT("Authored property shadows native self property: ")+ P.Name);
                for (const auto& P : C.Components)
                    if (NativeBaseClass->FindPropertyByName(*P.Name))
                        DSLFail(TEXT("Authored component shadows native self property: ")+ P.Name);
            }
            DSLApplyComponentDefaults(C, ComponentDefaults, BaseEffective.Components);
            const FString RefKey = Script + TEXT("::")+ C.Name;
            DSLActiveRefDeclarations.Add(RefKey, C);
            DSLRememberPropertySources(RefKey, St, C, TypeScope);
            DSLCollectWidgetLayoutUses(St, Group, C);
            das::vector<das::FunctionPtr> Functions, Generated;
            St->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent == St && DSLAnnotation(F->annotations, "ue_function"))
                        Functions.push_back(F);
                });
            for (const auto& F : Functions)
            {
                if (DSLAnnotation(F->annotations, "ue_lifecycle") || DSLAnnotation(F->annotations, "ue_event") ||
                    DSLStandardLifecycle(F))
                    DSLFail(TEXT("Conflicting UE callback/function annotations"));
                auto* A = const_cast<das::AnnotationDeclaration*>(DSLAnnotation(F->annotations, "ue_function"));
                DSLCommentTooltip(F->at, A->arguments, &F->annotations);
                DSLAnnotationArguments(A->arguments, {TEXT("out"), TEXT("ref")});
                FDasDeclaredMethod M;
                M.Name = DSLText(DSLMethodName(F));
                M.Id = C.Id + TEXT(".")+ M.Name;
                M.ScriptFile = Script;
                M.EntryPoint = TEXT("ue_")+ C.Name + TEXT("_")+ M.Name;
                M.bCallable = false;
                M.bConst = F->isConstClassMethod;
                TMap<FString, FString> ObjectClasses;
                TMap<FString, FString> Directions;
                for (const auto& V : A->arguments)
                {
                    if (V.name == "_ue_hidden_callback")
                    {
                        if (V.at.fileInfo || !DSLBool(V))
                            DSLFail(TEXT("Reserved callback compiler option"));
                        M.Metadata.Add(TEXT("BlueprintInternalUseOnly"), TEXT("true"));
                        continue;
                    }
                    if (V.name == "id")
                    {
                        M.Id = DSLString(A->arguments, "id");
                        continue;
                    }
                    if (V.name == "out" || V.name == "ref")
                    {
                        DSLAddDirections(V, Directions);
                        continue;
                    }
                    if (V.name == "BlueprintCallable")
                        M.bCallable = DSLBool(V);
                    else if (V.name == "BlueprintPure")
                        M.bPure = DSLBool(V);
                    else if (V.name == "BlueprintEvent")
                        M.bBlueprintEvent = DSLBool(V);
                    else if (V.name == "return_class" || V.name == "return_struct" || V.name == "return_enum" ||
                             (V.name.find("arg_") == 0 && V.name.size() > 10 &&
                              (V.name.substr(V.name.size() - 6) == "_class" ||
                               V.name.substr(V.name.size() - 7) == "_struct" ||
                               V.name.substr(V.name.size() - 5) == "_enum")))
                    {
                        ObjectClasses.Add(DSLText(V.name), DSLString(A->arguments, V.name.c_str()));
                    }
                    else if (DSLIsMetadata(V.name))
                        DSLMetadata(V, M.Metadata);
                    else
                        DSLFail(TEXT("Unsupported ue_function specifier: ")+ DSLText(V.name));
                }
                if (M.bPure)
                {
                    const auto* Callable = A->arguments.find("BlueprintCallable", das::Type::tBool);
                    if (Callable && !DSLBool(*Callable))
                        DSLFail(TEXT("BlueprintPure conflicts with BlueprintCallable=false"));
                    M.bCallable = true;
                }
                if (M.bBlueprintEvent && !A->arguments.find("BlueprintCallable", das::Type::tBool))
                    M.bCallable = true;
                const FString ResultType = DSLTypeName(F->result);
                if (!M.bCallable || F->isStaticClassMethod || !F->result || F->result->ref || ResultType.IsEmpty() ||
                    F->arguments.empty() || F->arguments.size() > 17)
                    DSLFail(TEXT("Adapter requires BlueprintCallable instance scalar/void method with up to 16 by-value inputs: ")+
                        M.Name);
                M.ReturnType.Kind = ResultType;
                if (ResultType == TEXT("delegate"))
                    M.ReturnType = DSLDelegateMethodType(F->result);
                if (DSLContainer(F->result))
                {
                    M.ReturnType = DSLContainerType(F->result, TypeFolder, TypeRegistry);
                    DSLEnsureStructHelpers(F->result, DSLDirectPath(F->result, TypeFolder, TypeRegistry), Group,
                                           St->module, HelpersGenerated, Helpers);
                }
                if (ResultType == TEXT("enum"))
                {
                    M.ReturnType.TypePath = DSLConstraint(
                        ObjectClasses,
                        TEXT("return_enum"), EDSLTypeKind::Enum,
                             DSLDirectEnum(F->result) ? DSLDirectPath(F->result, TypeFolder, TypeRegistry) : FString(),
                             TypeScope);
                    if (M.ReturnType.TypePath.IsEmpty())
                        DSLFail(TEXT("Enum result requires return_enum"));
                }
                if (ResultType == TEXT("struct"))
                {
                    if ((DSLDirectStruct(F->result) ||
                         (DSLMathPath(F->result) == TEXT("/Script/CoreUObject.Vector2D") ||
                                                         DSLMathPath(F->result) ==
                                                             TEXT("/Script/EnhancedInput.InputActionValue"))))
                    {
                        M.ReturnType.TypePath = DSLConstraint(
                            ObjectClasses, TEXT("return_struct"), EDSLTypeKind::Struct,
                                                DSLDirectPath(F->result, TypeFolder, TypeRegistry), TypeScope);
                        DSLEnsureStructHelpers(F->result, M.ReturnType.TypePath, Group, St->module, HelpersGenerated,
                                               Helpers);
                    }
                    else if (F->result->annotation->name == "UEStructValue")
                    {
                        M.ReturnType.TypePath = DSLConstraint(
                            ObjectClasses, TEXT("return_struct"), EDSLTypeKind::Struct, FString(), TypeScope);
                        if (M.ReturnType.TypePath.IsEmpty())
                            DSLFail(TEXT("UEStructValue result requires return_struct"));
                    }
                    else
                        M.ReturnType.TypePath =
                            DSLConstraint(ObjectClasses, TEXT("return_struct"), EDSLTypeKind::Struct,
                                                              DSLMathPath(F->result), TypeScope);
                }
                if (ResultType == TEXT("class"))
                {
                    M.ReturnType.TypePath = DSLConstraint(
                        ObjectClasses, TEXT("return_class"), EDSLTypeKind::Class,
                                            DSLTypedClass(F->result) ? DSLTypedClassPath(F->result) : FString(),
                                            TypeScope);
                    if (M.ReturnType.TypePath.IsEmpty())
                        M.ReturnType.TypePath = TEXT("/Script/CoreUObject.Object");
                }
                if (ResultType == TEXT("object"))
                {
                    M.ReturnType.TypePath = DSLConstraint(
                        ObjectClasses,
                        TEXT("return_class"),
                             DSLTypedInterface(F->result) ? EDSLTypeKind::Interface : EDSLTypeKind::Class,
                             DSLTypedReference(F->result) ? DSLTypedReferencePath(F->result) : FString(), TypeScope);
                    if (M.ReturnType.TypePath.IsEmpty())
                        M.ReturnType.TypePath = TEXT("/Script/CoreUObject.Object");
                }
                const bool LegacySingle =
                    C.Interfaces.IsEmpty() && Directions.IsEmpty() && F->arguments.size() == 2 &&
                    !F->arguments[1]->type->ref &&
                    (ResultType == TEXT("int") || ResultType == TEXT("float") || ResultType == TEXT("bool")) &&
                                                                     DSLTypeName(F->arguments[1]->type) == ResultType;
                for (size_t I = 1; I < F->arguments.size(); ++I)
                {
                    const auto& V = F->arguments[I];
                    const FString K = DSLTypeName(V->type);
                    const FString Direction = DSLParameterDirection(V, Directions.FindRef(DSLText(V->name)));
                    if (K.IsEmpty() || K == TEXT("void"))
                        DSLFail(TEXT("Unsupported parameter type/ref/default: ")+ DSLText(V->name));
                    FDasDeclaredParameter P;
                    P.Default = K == TEXT("text")
                                          ? DSLTextDefault(V->init)
                                          : K == TEXT("byte")? DSLByteDefault(V->init) : DSLParameterDefault(V->init);
                    P.Direction = Direction;
                    Directions.Remove(DSLText(V->name));
                    P.Name = LegacySingle ? TEXT("Input"): DSLText(V->name);
                    P.Id = M.Id + TEXT(".")+ (LegacySingle ? TEXT("input"): P.Name);
                    P.Type.Kind = K;
                    if (K == TEXT("delegate"))
                    {
                        if (Direction != TEXT("in"))
                            DSLFail(TEXT("Delegate-valued ref/out parameters are a later step"));
                        P.Type = DSLDelegateMethodType(V->type);
                    }
                    if (DSLContainer(V->type))
                    {
                        P.Type = DSLContainerType(V->type, TypeFolder, TypeRegistry);
                        DSLEnsureStructHelpers(V->type, DSLDirectPath(V->type, TypeFolder, TypeRegistry), Group,
                                               St->module, HelpersGenerated, Helpers);
                    }
                    if (K == TEXT("enum"))
                    {
                        const FString Key = TEXT("arg_")+ DSLText(V->name) + TEXT("_enum");
                        P.Type.TypePath = DSLConstraint(
                            ObjectClasses, Key, EDSLTypeKind::Enum,
                            DSLDirectEnum(V->type) ? DSLDirectPath(V->type, TypeFolder, TypeRegistry) : FString(),
                            TypeScope);
                        if (P.Type.TypePath.IsEmpty())
                            DSLFail(TEXT("Enum argument requires arg_name_enum"));
                    }
                    if (K == TEXT("struct"))
                    {
                        if ((DSLDirectStruct(V->type) || DSLMathPath(V->type) == TEXT("/Script/CoreUObject.Vector2D")))
                        {
                            const FString Key = TEXT("arg_")+ DSLText(V->name) + TEXT("_struct");
                            P.Type.TypePath =
                                DSLConstraint(ObjectClasses, Key, EDSLTypeKind::Struct,
                                              DSLDirectPath(V->type, TypeFolder, TypeRegistry), TypeScope);
                            DSLEnsureStructHelpers(V->type, P.Type.TypePath, Group, St->module, HelpersGenerated,
                                                   Helpers);
                        }
                        else if (V->type->annotation->name == "UEStructValue")
                        {
                            const FString Key = TEXT("arg_")+ DSLText(V->name) + TEXT("_struct");
                            P.Type.TypePath =
                                DSLConstraint(ObjectClasses, Key, EDSLTypeKind::Struct, FString(), TypeScope);
                            if (P.Type.TypePath.IsEmpty())
                                DSLFail(TEXT("UEStructValue parameter requires arg_name_struct"));
                        }
                        else
                        {
                            const FString Key = TEXT("arg_")+ DSLText(V->name) + TEXT("_struct");
                            P.Type.TypePath = DSLConstraint(ObjectClasses, Key, EDSLTypeKind::Struct,
                                                            DSLMathPath(V->type), TypeScope);
                        }
                    }
                    if (K == TEXT("class"))
                    {
                        const FString Key = TEXT("arg_")+ DSLText(V->name) + TEXT("_class");
                        P.Type.TypePath =
                            DSLConstraint(ObjectClasses, Key, EDSLTypeKind::Class,
                                          DSLTypedClass(V->type) ? DSLTypedClassPath(V->type) : FString(), TypeScope);
                        if (P.Type.TypePath.IsEmpty())
                            P.Type.TypePath = TEXT("/Script/CoreUObject.Object");
                    }
                    if (K == TEXT("object"))
                    {
                        const FString Key = TEXT("arg_")+ DSLText(V->name) + TEXT("_class");
                        P.Type.TypePath = DSLConstraint(
                            ObjectClasses, Key,
                            DSLTypedInterface(V->type) ? EDSLTypeKind::Interface : EDSLTypeKind::Class,
                            DSLTypedReference(V->type) ? DSLTypedReferencePath(V->type) : FString(), TypeScope);
                        if (P.Type.TypePath.IsEmpty())
                            P.Type.TypePath = TEXT("/Script/CoreUObject.Object");
                    }
                    M.Parameters.Add(P);
                }
                if (!ObjectClasses.IsEmpty())
                    DSLFail(TEXT("Unused object class annotation"));
                if (!Directions.IsEmpty())
                    DSLFail(TEXT("Unknown out/ref parameter name"));
                DSLRememberMethodSource(RefKey, M, F, TypeScope);
                C.Methods.Add(MoveTemp(M));
            }
            if (Derived)
            {
                for (const auto& M : BaseEffective.Methods)
                    if (!C.Methods.ContainsByPredicate([&](const auto& Own) { return Own.Name == M.Name; }))
                    {
                        C.Methods.Add(M);
                        const auto* Owner = Model.Classes.FindByPredicate(
                            [&](const auto& Cl)
                            {
                                return Cl.Methods.ContainsByPredicate([&](const auto& Method)
                                                                      { return Method.Id == M.Id; });
                            });
                        if (Owner)
                        {
                            const FString From = M.ScriptFile + TEXT("::")+ Owner->Name + TEXT("::")+ M.Name;
                            if (const auto* Descriptor = DSLMethodSourceSignatures.Find(From))
                                DSLMethodSourceSignatures.Add(RefKey + TEXT("::")+ M.Name, *Descriptor);
                        }
                    }
            }
            // Publish all method contracts before body rewriting opens mutually referring sources.
            DSLActiveRefDeclarations.Add(RefKey, C);
            das::TypeDeclPtr SelfEventReference = nullptr;
            if (C.Methods.ContainsByPredicate([&](const auto& M)
                                              { return DSLVirtualMethod(C, M) || M.ScriptFile != Script; }))
            {
                auto* Ref = new das::Structure();
                Ref->name = DSLUtf8(TEXT("__ue_self_events_")+ C.Name);
                Ref->module = St->module;
                Ref->at = St->at;
                das::Structure::FieldDeclaration Handle;
                Handle.flags = 0;
                Handle.name = "handle";
                Handle.type = DSLHandleType(Group, St->module, "UObjectHandle");
                Handle.at = St->at;
                Ref->fields.push_back(Handle);
                auto Annotations = Group.findAnnotation("ue_object_ref", St->module);
                if (Annotations.size() != 1)
                    DSLFail(TEXT("Missing authored reference annotation"));
                auto* Annotation = new das::AnnotationDeclaration();
                Annotation->flags = 0;
                Annotation->annotation = Annotations[0];
                Annotation->at = St->at;
                Annotation->arguments.push_back(das::AnnotationArgument("script", DSLUtf8(Script)));
                Annotation->arguments.push_back(das::AnnotationArgument("type", DSLUtf8(C.Name)));
                Ref->annotations.push_back(Annotation);
                if (!St->module->addStructure(Ref))
                    DSLFail(TEXT("Self event reference name collision"));
                SelfEventReference = new das::TypeDecl(das::Type::tStructure, St->at);
                SelfEventReference->structType = Ref;
            }
            das::TypeDeclPtr SuperReference = nullptr;
            if (Derived)
            {
                auto* Ref = new das::Structure();
                Ref->name = DSLUtf8(TEXT("__ue_super_")+ C.Name);
                Ref->module = St->module;
                Ref->at = St->at;
                das::Structure::FieldDeclaration Handle;
                Handle.flags = 0;
                Handle.name = "handle";
                Handle.type = DSLHandleType(Group, St->module, "UObjectHandle");
                Handle.at = St->at;
                Ref->fields.push_back(Handle);
                auto Annotations = Group.findAnnotation("ue_object_ref", St->module);
                if (Annotations.size() != 1)
                    DSLFail(TEXT("Missing super reference annotation"));
                auto* A = new das::AnnotationDeclaration();
                A->flags = 0;
                A->annotation = Annotations[0];
                A->at = St->at;
                const auto* Parent = Model.Classes.FindByPredicate([&](const auto& B) { return B.Id == BaseId; });
                if (!Parent || Parent->SourceScript.IsEmpty())
                    DSLFail(TEXT("Super parent source is unavailable"));
                A->arguments.push_back(das::AnnotationArgument("script", DSLUtf8(Parent->SourceScript)));
                A->arguments.push_back(das::AnnotationArgument("type", DSLUtf8(Parent->Name)));
                A->arguments.push_back(das::AnnotationArgument("_ue_super_id", DSLUtf8(C.Id)));
                Ref->annotations.push_back(A);
                if (!St->module->addStructure(Ref))
                    DSLFail(TEXT("Super reference collision"));
                SuperReference = new das::TypeDecl(das::Type::tStructure, St->at);
                SuperReference->structType = Ref;
            }
            // Script-only instance methods are lowered once into same-context
            // functions. They are not reflected, exported, or added to the IR.
            struct FLocalVariant
            {
                das::StructurePtr Owner;
                das::FunctionPtr Function;
                FString Name;
                das::string Target;
            };
            TArray<das::StructurePtr> LocalChain;
            for (auto Owner = St; Owner && DSLNativeBasePath(Owner).IsEmpty(); Owner = Owner->parent)
                LocalChain.Add(Owner);
            TMap<FString, das::string> QualifiedLocals;
            TArray<FLocalVariant> LocalVariants;
            TMap<FString, das::string> DynamicLocals, ParentLocals;
            TMap<FString, das::string> LocalRenames;
            auto VariantName = [&](const FString& Context, das::StructurePtr Owner, const FString& Name)
            {
                return DSLUtf8(TEXT("__ue_local_impl_")+ Context +
                                    TEXT("_")+ FMD5::HashAnsiString(*(DSLText(Owner->at.fileInfo->name) +
                                                                      TEXT("::")+ DSLText(Owner->name))) +
                                                                    TEXT("_")+ Name);
            };
            for (int32 Depth = 0; Depth < LocalChain.Num(); ++Depth)
            {
                auto Owner = LocalChain[Depth];
                Owner->module->functions.foreach (
                    [&](const das::FunctionPtr& F)
                    {
                        if (F->classParent != Owner || F->generated || DSLAnnotation(F->annotations, "ue_function") ||
                            DSLAnnotation(F->annotations, "ue_event") ||
                            DSLAnnotation(F->annotations, "ue_lifecycle") || DSLStandardLifecycle(F))
                            return;
                        if (F->isStaticClassMethod || F->arguments.empty() || !F->result || F->result->ref)
                            DSLFail(TEXT("Script-only helpers require instance methods with value/void results"));
                        const FString Name = DSLText(F->name.substr(
                            F->name.find_last_of(das::string(1, char(96)) + das::string(1, char(39))) + 1));
                        const auto Target = VariantName(C.Name, Owner, Name);
                        if (const auto* Selected = DynamicLocals.Find(Name))
                        {
                            const auto* DerivedVariant =
                                LocalVariants.FindByPredicate([&](const auto& V) { return V.Target == *Selected; });
                            auto* D = DerivedVariant->Function;
                            if (D->isConstClassMethod != F->isConstClassMethod ||
                                D->arguments.size() != F->arguments.size() ||
                                D->result->describe() != F->result->describe())
                                DSLFail(TEXT("Helper override requires unchanged signature: ")+ Name);
                            for (size_t I = 1; I < F->arguments.size(); ++I)
                                if (D->arguments[I]->type->describe() != F->arguments[I]->type->describe())
                                    DSLFail(TEXT("Helper override requires unchanged parameter types: ")+ Name);
                        }
                        else
                            DynamicLocals.Add(Name, Target);
                        if (Depth > 0 && !ParentLocals.Contains(Name))
                            ParentLocals.Add(Name, Target);
                        LocalVariants.Add({Owner, F, Name, Target});
                        for (auto Context : LocalChain)
                            LocalRenames.Add(DSLText(VariantName(DSLText(Context->name), Owner, Name)), Target);
                    });
            }
            for (const auto& V : LocalVariants)
            {
                QualifiedLocals.Add(DSLText(V.Function->name), DynamicLocals[V.Name]);
                QualifiedLocals.Add(DSLText(V.Owner->module->name + "::" + V.Function->name), DynamicLocals[V.Name]);
            }
            for (const auto& Variant : LocalVariants)
            {
                const auto& F = Variant.Function;
                auto Clone = F->clone();
                Clone->name = Variant.Target;
                Clone->classParent = nullptr;
                Clone->isClassMethod = false;
                Clone->annotations.clear();
                Clone->exports = false;
                Clone->generated = true;
                Clone->privateFunction = false;
                Clone->isConstClassMethod = false;
                TMap<FString, das::string> Supers;
                bool PastOwner = false;
                for (auto Owner : LocalChain)
                {
                    if (Owner == Variant.Owner)
                    {
                        PastOwner = true;
                        continue;
                    }
                    if (!PastOwner)
                        continue;
                    for (const auto& V : LocalVariants)
                        if (V.Owner == Owner && !Supers.Contains(V.Name))
                            Supers.Add(V.Name, V.Target);
                }
                FDSLTypeCallVisitor Types(TypeScope);
                Clone->body = Clone->body->visit(Types);
                FDSLBodyVisitor Rewrite(C, Clone->arguments[0]->name, St, TypeFolder, TypeRegistry, Group,
                                        HelpersGenerated, Helpers);
                Rewrite.SelfReference = SelfEventReference;
                Rewrite.SuperReference = SuperReference;
                Rewrite.LocalTargets = DynamicLocals;
                Rewrite.LocalSupers = Supers;
                Rewrite.LocalRenames = LocalRenames;
                Rewrite.QualifiedLocals = QualifiedLocals;
                Clone->body = Clone->body->visit(Rewrite);
                if (Variant.Owner == St && Derived)
                {
                    struct FHelperSuperSource : das::Visitor
                    {
                        FDSLBodyVisitor& Owner;
                        explicit FHelperSuperSource(FDSLBodyVisitor& In) : Owner(In) {}
                        virtual das::ExpressionPtr visit(das::ExprInvoke* E) override { return Owner.SuperCall(E); }
                    } SourceSupers(Rewrite);
                    F->body = F->body->visit(SourceSupers);
                }
                if (Variant.Owner == St)
                {
                    struct FSourceLocalCalls : das::Visitor
                    {
                        das::string Self;
                        TMap<FString, das::string> Names;
                        FSourceLocalCalls(const das::string& S) : Self(S) {}
                        virtual das::ExpressionPtr visit(das::ExprInvoke* E) override
                        {
                            if (!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())
                                return E;
                            auto* Field = static_cast<das::ExprField*>(E->arguments[0]);
                            if (!Field->value->rtti_isVar() || static_cast<das::ExprVar*>(Field->value)->name != Self)
                                return E;
                            const auto* Name = Names.Find(DSLText(Field->name));
                            if (!Name)
                                return E;
                            auto* Call = DSLCall(E->at, *Name, {Field->value});
                            for (size_t I = 2; I < E->arguments.size(); ++I)
                                Call->arguments.push_back(E->arguments[I]);
                            return Call;
                        }
                    } SourceCalls(F->arguments[0]->name);
                    for (const auto& V : LocalVariants)
                        if (!SourceCalls.Names.Contains(V.Name))
                            SourceCalls.Names.Add(V.Name, V.Owner->module == St->module
                                                              ? V.Function->name
                                                              : V.Owner->module->name + "::" + V.Function->name);
                    F->body = F->body->visit(SourceCalls);
                }
                Clone->arguments.erase(Clone->arguments.begin());
                if (!Rewrite.Error.IsEmpty())
                    DSLFail(Rewrite.Error);
                Generated.push_back(Clone);
            }
            for (const auto& F : Functions)
            {
                const FString MethodName = DSLText(DSLMethodName(F));
                const auto M = *C.Methods.FindByPredicate([&](const auto& Item) { return Item.Name == MethodName; });
                const FString ResultType = DSLTypeName(F->result);
                FDSLTypeCallVisitor TypeCalls(TypeScope);
                F->body = F->body->visit(TypeCalls);
                auto Clone = F->clone();
                Clone->name = DSLUtf8(M.EntryPoint);
                Clone->classParent = nullptr;
                Clone->isClassMethod = false;
                Clone->annotations.clear();
                Clone->exports = true;
                Clone->generated = true;
                Clone->privateFunction = false;
                Clone->isStaticClassMethod = false;
                Clone->isConstClassMethod = false;
                FDSLBodyVisitor Rewrite(C, Clone->arguments[0]->name, St, TypeFolder, TypeRegistry, Group,
                                        HelpersGenerated, Helpers);
                Rewrite.SelfReference = SelfEventReference;
                Rewrite.SuperReference = SuperReference;
                Rewrite.LocalTargets = DynamicLocals;
                Rewrite.LocalSupers = ParentLocals;
                Rewrite.LocalRenames = LocalRenames;
                Rewrite.QualifiedLocals = QualifiedLocals;
                Clone->body = Clone->body->visit(Rewrite);
                FDSLSourceTimerVisitor SourceTimers(Rewrite);
                F->body = F->body->visit(SourceTimers);
                if (Derived)
                {
                    struct FSuperSource : das::Visitor
                    {
                        FDSLBodyVisitor& Owner;
                        explicit FSuperSource(FDSLBodyVisitor& In) : Owner(In) {}
                        virtual das::ExpressionPtr visit(das::ExprInvoke* E) override { return Owner.SuperCall(E); }
                    } SuperSource(Rewrite);
                    F->body = F->body->visit(SuperSource);
                }
                Clone->arguments.erase(Clone->arguments.begin());
                if (!Rewrite.Error.IsEmpty())
                    DSLFail(Rewrite.Error);
                bool HasOutputs = false;
                for (const auto& P : M.Parameters)
                    HasOutputs |= P.Direction != TEXT("in");
                bool Direct = true;
                if (Direct)
                {
                    auto Wrapper = Clone->clone();
                    Clone->name = DSLUtf8(TEXT("__impl_")+ M.EntryPoint);
                    Clone->exports = false;
                    auto* Call = new das::ExprCall(F->at, Clone->name);
                    auto* Block = new das::ExprBlock();
                    Block->at = F->at;
                    auto Local = [&](const das::string& Name, const das::TypeDeclPtr& Type, das::ExpressionPtr Init)
                    {
                        auto* V = new das::Variable();
                        V->at = F->at;
                        V->name = Name;
                        V->type = DSLCopyType(Type);
                        V->type->ref = false;
                        V->type->constant = false;
                        V->init = Init;
                        V->init_via_move = DSLContainer(Type);
                        auto* Let = new das::ExprLet();
                        Let->at = F->at;
                        Let->variables.push_back(V);
                        Block->list.push_back(Let);
                    };
                    for (size_t I = 0; I < Wrapper->arguments.size(); ++I)
                    {
                        auto& Arg = Wrapper->arguments[I];
                        Arg->init = nullptr;
                        auto Original = F->arguments[I + 1]->type;
                        Arg->type = DSLCopyType(Arg->type);
                        Arg->type->ref = false;
                        Arg->type->constant = true;
                        das::ExpressionPtr Value = new das::ExprVar(F->at, Arg->name);
                        if (DSLDelegate(Original))
                        {
                            Arg->type = new das::TypeDecl(das::Type::tUInt64, F->at);
                            Value = DSLDirectUnpack(F->at, Original, Value, FString());
                        }
                        if (DSLTypedClass(Original) || DSLContainer(Original) || DSLDirectEnum(Original) ||
                            (DSLDirectStruct(Original) ||
                             (DSLMathPath(Original) == TEXT("/Script/CoreUObject.Vector2D") ||
                                                            DSLMathPath(Original) ==
                                                                TEXT("/Script/EnhancedInput.InputActionValue"))) ||
                             DSLTypedReference(Original))
                        {
                            Arg->type = DSLHandleType(Group, St->module,
                                                      DSLTypedClass(Original)       ? "UEClassHandle"
                                                      : DSLTypedReference(Original) ? "UObjectHandle"
                                                      : DSLDirectEnum(Original)     ? "UEEnumValue"
                                                                                    : "UEStructValue");
                            Value = DSLDirectUnpack(F->at, Original, Value,
                                                    DSLContainer(Original)
                                                        ? DSLDirectPath(Original, TypeFolder, TypeRegistry)
                                                        : M.Parameters[int32(I)].Type.TypePath);
                        }
                        if (M.Parameters[int32(I)].Direction != TEXT("in"))
                        {
                            const das::string Name = "__ue_ref_" + das::to_string(I);
                            Local(Name, Original, Value);
                            Value = new das::ExprVar(F->at, Name);
                        }
                        Call->arguments.push_back(Value);
                    }
                    das::ExpressionPtr Value = Call;
                    if (HasOutputs)
                    {
                        if (ResultType == TEXT("void"))
                        {
                            Block->list.push_back(Call);
                            Value = nullptr;
                        }
                        else
                        {
                            Local("__ue_return", F->result, Call);
                            Value = new das::ExprVar(F->at, "__ue_return");
                        }
                        for (int32 I = 0; I < M.Parameters.Num(); ++I)
                            if (M.Parameters[I].Direction != TEXT("in"))
                            {
                                auto Original = F->arguments[size_t(I) + 1]->type;
                                das::ExpressionPtr Updated = new das::ExprVar(F->at, "__ue_ref_" + das::to_string(I));
                                const FString Kind = M.Parameters[I].Type.Kind;
                                const das::string Math = DSLUtf8(DSLMathPath(Original));
                                const das::string Suffix =
                                    Kind ==
                                    TEXT("class")
                                         ? "class"
                                         : Kind ==
                                               TEXT("object")
                                                    ? "object"
                                                    : Kind ==
                                                          TEXT("enum")
                                                               ? "enum"
                                                               : Kind ==
                                                                     TEXT("struct")
                                                                          ? (Math == "/Script/CoreUObject.Vector"
                                                                                 ? "vector"
                                                                             : Math == "/Script/CoreUObject.Rotator"
                                                                                 ? "rotator"
                                                                             : Math == "/Script/CoreUObject.Transform"
                                                                                 ? "transform"
                                                                                 : "struct")
                                                                      : DSLContainer(Original) ? "struct"
                                                                                               : DSLScalarSuffix(Kind);
                                if (DSLTypedClass(Original) || DSLContainer(Original) ||
                                    (DSLDirectStruct(Original) ||
                                     (DSLMathPath(Original) ==
                                          TEXT("/Script/CoreUObject.Vector2D") ||
                                               DSLMathPath(Original) ==
                                                   TEXT("/Script/EnhancedInput.InputActionValue"))) ||
                                     DSLDirectEnum(Original) || DSLTypedReference(Original))
                                    Updated = DSLDirectPack(F->at, Original, Updated,
                                                            DSLContainer(Original)
                                                                ? DSLDirectPath(Original, TypeFolder, TypeRegistry)
                                                                : M.Parameters[I].Type.TypePath);
                                Block->list.push_back(DSLCall(F->at, "ue_frame_out_" + Suffix,
                                                              {new das::ExprConstInt(F->at, I), Updated}));
                            }
                    }
                    if (DSLDelegate(F->result))
                    {
                        Wrapper->result = new das::TypeDecl(das::Type::tUInt64, F->at);
                        Value = DSLDirectPack(F->at, F->result, Value, FString());
                    }
                    if (DSLTypedClass(F->result) || DSLContainer(F->result) || DSLDirectEnum(F->result) ||
                        (DSLDirectStruct(F->result) ||
                         (DSLMathPath(F->result) == TEXT("/Script/CoreUObject.Vector2D") ||
                                                         DSLMathPath(F->result) ==
                                                             TEXT("/Script/EnhancedInput.InputActionValue"))) ||
                         DSLTypedReference(F->result))
                    {
                        Wrapper->result = DSLHandleType(Group, St->module,
                                                        DSLTypedClass(F->result)       ? "UEClassHandle"
                                                        : DSLTypedReference(F->result) ? "UObjectHandle"
                                                        : DSLDirectEnum(F->result)     ? "UEEnumValue"
                                                                                       : "UEStructValue");
                        Value =
                            DSLDirectPack(F->at, F->result, Value,
                                          DSLContainer(F->result) ? DSLDirectPath(F->result, TypeFolder, TypeRegistry)
                                                                  : M.ReturnType.TypePath);
                    }
                    if (Value)
                        Block->list.push_back(
                            M.ReturnType.Kind ==
                            TEXT("void")? Value : static_cast<das::ExpressionPtr>(new das::ExprReturn(F->at, Value)));
                    Wrapper->body = Block;
                    Generated.push_back(Wrapper);
                }
                Generated.push_back(Clone);
            }
            das::vector<das::FunctionPtr> Callbacks;
            St->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent == St && (DSLAnnotation(F->annotations, "ue_lifecycle") ||
                                                 DSLAnnotation(F->annotations, "ue_event") || DSLStandardLifecycle(F)))
                        Callbacks.push_back(F);
                });
            TArray<FDasDeclaredCallback> OwnedCallbacks;
            std::sort(Callbacks.begin(), Callbacks.end(),
                      [](const das::FunctionPtr& A, const das::FunctionPtr& B) { return A->name < B->name; });
            das::ExprBlock* BindBlock = nullptr;
            das::FunctionPtr BindPrototype = nullptr;
            for (const auto& F : Callbacks)
            {
                const auto* Life = DSLAnnotation(F->annotations, "ue_lifecycle");
                const auto* Event = DSLAnnotation(F->annotations, "ue_event");
                if ((Life && Event) || DSLAnnotation(F->annotations, "ue_function"))
                    DSLFail(TEXT("Conflicting UE callback/function annotations"));
                FDasDeclaredCallback Callback;
                Callback.Name = DSLText(DSLMethodName(F));
                Callback.Kind = Event ? TEXT("event"): Callback.Name;
                if (F->result && F->result->baseType == das::Type::autoinfer)
                    F->result = new das::TypeDecl(das::Type::tVoid, F->at);
                if (F->isStaticClassMethod || !F->result || F->result->ref ||
                    DSLTypeName(F->result) != TEXT("void") || F->arguments.empty())
                    DSLFail(TEXT("UE callbacks must return void"));
                const bool MultiEvent = Event && F->arguments.size() > 2;
                FString Parameter =
                    Event ? (F->arguments.size() == 1   ? FString()
                             : F->arguments.size() == 2 ? DSLTypeName(F->arguments[1]->type)
                                                        : TEXT("invalid"))
                          : Callback.Kind == TEXT("Tick")? TEXT("float")
                                                         : Callback.Kind == TEXT("EndPlay")? TEXT("int"): FString();
                if (MultiEvent)
                {
                    Parameter = TEXT("struct");
                    Callback.ParameterKind = TEXT("struct");
                    Callback.ParameterTypePath = DSLCallbackTuple(F, TypeFolder, TypeRegistry)->GetPathName();
                    for (size_t I = 1; I < F->arguments.size(); ++I)
                    {
                        const auto T = F->arguments[I]->type;
                        const auto D = DSLCallbackField(T, TypeFolder, TypeRegistry);
                        if (D.Kind == TEXT("struct"))
                            DSLEnsureStructHelpers(T, D.TypePath, Group, St->module, HelpersGenerated, Helpers);
                    }
                }
                else if (Event)
                {
                    Parameter = F->arguments.size() == 1         ? FString()
                                : F->arguments.size() == 2       ? DSLCallbackKind(F->arguments[1]->type)
                                                                 : TEXT("invalid");
                    Callback.ParameterKind = Parameter.IsEmpty() ? TEXT("void"): Parameter;
                    if (!FindDasCallbackType(Callback.ParameterKind))
                        DSLFail(TEXT("Unsupported event callback payload"));
                    if (Callback.ParameterKind == TEXT("enum") || Callback.ParameterKind == TEXT("struct"))
                    {
                        const auto T = F->arguments[1]->type;
                        if (!DSLDirectEnum(T) && !DSLDirectStruct(T) && !DSLContainer(T))
                            DSLFail(TEXT("Event value requires a concrete registered type/carrier"));
                        Callback.ParameterTypePath = DSLDirectPath(T, TypeFolder, TypeRegistry);
                        FString Issue;
                        if (!ValidateDasCallbackTypePath(Callback.ParameterKind, Callback.ParameterTypePath, Issue))
                            DSLFail(Issue);
                    }
                    if (Parameter == TEXT("class"))
                    {
                        if (!DSLTypedClass(F->arguments[1]->type))
                            DSLFail(TEXT("Class events require UEClass<T>"));
                        Callback.ParameterTypePath = DSLTypedClassPath(F->arguments[1]->type);
                    }
                    if (Parameter == TEXT("object"))
                    {
                        if (!DSLTypedReference(F->arguments[1]->type))
                            DSLFail(TEXT("Object events require a typed object/interface wrapper"));
                        Callback.ParameterTypePath = DSLTypedReferencePath(F->arguments[1]->type);
                    }
                }
                if (!MultiEvent && F->arguments.size() != (Parameter.IsEmpty() ? 1 : 2))
                    DSLFail(TEXT("UE callback signature mismatch: ")+ Callback.Name);
                if (!MultiEvent && !Parameter.IsEmpty() &&
                    (!F->arguments[1]->type || F->arguments[1]->type->ref || F->arguments[1]->init ||
                     DSLTypeName(F->arguments[1]->type) !=
                         ((Parameter ==
                           TEXT("vector2d") ||
                                Parameter ==
                                    TEXT("vector") || Parameter == TEXT("rotator") || Parameter == TEXT("transform"))
                                         ? TEXT("struct")
                                         : Parameter.StartsWith(TEXT("array_"))
                                           ? TEXT("array")
                                           : (Parameter == TEXT("struct") && DSLContainer(F->arguments[1]->type))
                                              ? DSLTypeName(F->arguments[1]->type)
                                              : Parameter)))
                    DSLFail(TEXT("UE callback parameter must be by-value ")+ Parameter);
                if (!Event &&
                    Callback.Kind != TEXT("ConstructionScript") &&
                                          Callback.Kind != TEXT("Initialize") &&
                                                                Callback.Kind != TEXT("BeginPlay") &&
                                                                                      Callback.Kind !=
                                                                                          TEXT("Tick") &&
                                                                                               Callback.Kind !=
                                                                                                   TEXT("EndPlay"))
                    DSLFail(TEXT("Unknown lifecycle method; expected ConstructionScript, Initialize, BeginPlay, Tick or EndPlay"));
                const FString Entry =
                    Event ? TEXT("ue_")+ C.Name +
                                 TEXT("_")+ Callback.Name
                                 : Callback.Kind == TEXT("ConstructionScript")
                                       ? TEXT("__ue_construct")
                                       : Callback.Kind == TEXT("Initialize")
                                             ? TEXT("__ue_initialize")
                                             : Callback.Kind == TEXT("BeginPlay")
                                                   ? TEXT("begin_play")
                                                   : Callback.Kind == TEXT("Tick")? TEXT("tick"): TEXT("end_play");
                FDSLTypeCallVisitor TypeCalls(TypeScope);
                F->body = F->body->visit(TypeCalls);
                auto Clone = F->clone();
                Clone->name = DSLUtf8(Entry);
                Clone->classParent = nullptr;
                Clone->isClassMethod = false;
                Clone->annotations.clear();
                Clone->exports = true;
                Clone->generated = true;
                Clone->privateFunction = false;
                Clone->isConstClassMethod = false;
                FDSLBodyVisitor Rewrite(C, Clone->arguments[0]->name, St, TypeFolder, TypeRegistry, Group,
                                        HelpersGenerated, Helpers);
                Rewrite.SelfReference = SelfEventReference;
                Rewrite.SuperReference = SuperReference;
                Rewrite.LocalTargets = DynamicLocals;
                Rewrite.LocalSupers = ParentLocals;
                Rewrite.LocalRenames = LocalRenames;
                Rewrite.QualifiedLocals = QualifiedLocals;
                Clone->body = Clone->body->visit(Rewrite);
                FDSLSourceTimerVisitor SourceTimers(Rewrite);
                F->body = F->body->visit(SourceTimers);
                if (Derived)
                {
                    struct FSuperSource : das::Visitor
                    {
                        FDSLBodyVisitor& Owner;
                        explicit FSuperSource(FDSLBodyVisitor& In) : Owner(In) {}
                        virtual das::ExpressionPtr visit(das::ExprInvoke* E) override { return Owner.SuperCall(E); }
                    } SuperSource(Rewrite);
                    F->body = F->body->visit(SuperSource);
                }
                Clone->arguments.erase(Clone->arguments.begin());
                if (MultiEvent)
                    DSLCallbackUnpackArguments(Clone, TypeFolder, TypeRegistry, Group, St->module);
                if (!MultiEvent && Event && (Parameter == TEXT("object") || Parameter == TEXT("class")))
                {
                    auto Arg = Clone->arguments[0];
                    const auto OriginalName = Arg->name;
                    const auto SourceType = DSLCopyType(Arg->type);
                    Arg->name = "__ue_event_raw_object";
                    Arg->type =
                        DSLHandleType(Group, St->module, Parameter == TEXT("class")? "UEClassHandle" : "UObjectHandle");
                    auto* Local = new das::Variable();
                    Local->at = F->at;
                    Local->name = OriginalName;
                    Local->type = SourceType;
                    Local->type->ref = false;
                    Local->init = DSLDirectUnpack(F->at, SourceType, new das::ExprVar(F->at, Arg->name),
                                                  Parameter == TEXT("class")? DSLTypedClassPath(SourceType)
                                                                            : DSLTypedReferencePath(SourceType));
                    auto* Let = new das::ExprLet();
                    Let->at = F->at;
                    Let->variables.push_back(Local);
                    static_cast<das::ExprBlock*>(Clone->body)
                        ->list.insert(static_cast<das::ExprBlock*>(Clone->body)->list.begin(), Let);
                }
                if (!MultiEvent && Event &&
                    (Parameter.StartsWith(TEXT("array_")) ||
                                          Parameter == TEXT("vector2d") || Parameter == TEXT("struct") ||
                                                                                             Parameter == TEXT("enum")))
                {
                    auto Arg = Clone->arguments[0];
                    const auto OriginalName = Arg->name;
                    const auto SourceType = DSLCopyType(Arg->type);
                    const FString Carrier = Parameter == TEXT("vector2d")
                                                              ? DSLMathPath(SourceType)
                                                              : DSLDirectPath(SourceType, TypeFolder, TypeRegistry);
                    if (Parameter != TEXT("enum"))
                        DSLEnsureStructHelpers(SourceType, Carrier, Group, St->module, HelpersGenerated, Helpers);
                    Arg->name = "__ue_event_raw_array";
                    Arg->type =
                        DSLHandleType(Group, St->module, Parameter == TEXT("enum")? "UEEnumValue" : "UEStructValue");
                    auto* Local = new das::Variable();
                    Local->at = F->at;
                    Local->name = OriginalName;
                    Local->type = SourceType;
                    Local->type->ref = false;
                    Local->type->constant = false;
                    Local->init_via_move = true;
                    Local->init = DSLDirectUnpack(F->at, SourceType, new das::ExprVar(F->at, Arg->name), Carrier);
                    auto* Let = new das::ExprLet();
                    Let->at = F->at;
                    Let->variables.push_back(Local);
                    static_cast<das::ExprBlock*>(Clone->body)
                        ->list.insert(static_cast<das::ExprBlock*>(Clone->body)->list.begin(), Let);
                }
                if (!Rewrite.Error.IsEmpty())
                    DSLFail(Rewrite.Error);
                Generated.push_back(Clone);
                if (Event)
                {
                    if (!BindBlock)
                    {
                        BindBlock = new das::ExprBlock();
                        BindBlock->at = St->at;
                    }
                    Callback.Source = DSLString(Event->arguments, "source", TEXT("self"));
                    Callback.EventName = DSLString(Event->arguments, "name");
                    if (Callback.Source != TEXT("self") && Callback.Source !=
                                                               TEXT("owner") &&
                                                                    !C.Components.ContainsByPredicate(
                                                                        [&](const auto& Component)
                                                                        { return Component.Name == Callback.Source; }))
                        DSLFail(TEXT("Event source must be self, owner or a declared component"));
                    if (Callback.Source == TEXT("owner") && C.NativeParent != TEXT("/Script/Engine.ActorComponent"))
                        DSLFail(TEXT("Owner event source requires ActorComponent"));
                    // Native sources are known while compiling: check the delegate and its arity here
                    // instead of at subscription time. Authored sources keep the runtime check.
                    if (UClass* EventClass = DSLEventSourceClass(C, Callback.Source))
                    {
                        const bool AuthoredEvent = C.Properties.ContainsByPredicate(
                            [&](const auto& P) { return P.Name == Callback.EventName; });
                        const int32 Arity = int32(F->arguments.size()) - 1;
                        if (!AuthoredEvent && !DSLNativeEventMatches(EventClass, Callback.EventName, Arity))
                            DSLFail(FString::Printf(TEXT("ue_event %s: %s has no BlueprintAssignable multicast delegate %s with %d parameter(s)"),
                                *Callback.Name, *EventClass->GetName(), *Callback.EventName, Arity));
                    }
                    auto* SelfObject = DSLCall(F->at, "ue_self_object", {});
                    das::ExpressionPtr Source =
                        Callback.Source ==
                        TEXT("owner")
                             ? DSLCall(F->at, "ue_component_owner", {SelfObject})
                                               : Callback.Source ==
                                   TEXT("self")? static_cast<das::ExpressionPtr>(SelfObject)
                                               : static_cast<das::ExpressionPtr>(DSLCall(
                                                     F->at, "ue_get_self_object",
                                                     {new das::ExprConstString(F->at, DSLUtf8(Callback.Source))}));
                    auto* Subscribe =
                        DSLCall(F->at, DSLUtf8(TEXT("ue_subscribe_object_")+ Callback.ParameterKind),
                                               {Source, new das::ExprConstString(F->at, DSLUtf8(Callback.EventName)),
                                                new das::ExprConstString(F->at, DSLUtf8(Entry))});
                    if (!Callback.ParameterTypePath.IsEmpty())
                        Subscribe->arguments.push_back(
                            new das::ExprConstString(F->at, DSLUtf8(Callback.ParameterTypePath)));
                    BindBlock->list.push_back(Subscribe);
                    BindPrototype = Clone;
                }
                OwnedCallbacks.Add(Callback);
                C.Callbacks.RemoveAll([&](const auto& P)
                                      { return P.Kind == Callback.Kind && Callback.Kind != TEXT("event"); });
                C.Callbacks.Add(Callback);
            }
            if (Derived)
            {
                if (Script == BaseEffective.SourceScript && !C.Callbacks.IsEmpty())
                    DSLFail(TEXT("Lifecycle inheritance requires separate source files"));
                auto ParentHooks = BaseEffective.Callbacks;
                if (ParentHooks.ContainsByPredicate([](const auto& Hook) { return Hook.Kind == TEXT("event"); }))
                    for (const auto* Kind : {TEXT("BeginPlay"), TEXT("EndPlay")})
                        if (!ParentHooks.ContainsByPredicate([&](const auto& Hook) { return Hook.Kind == Kind; }))
                        {
                            FDasDeclaredCallback Hook;
                            Hook.Name = Kind;
                            Hook.Kind = Kind;
                            ParentHooks.Add(Hook);
                        }
                for (const auto& ParentHook : ParentHooks)
                {
                    if (ParentHook.Kind == TEXT("event"))
                        continue;
                    if (OwnedCallbacks.ContainsByPredicate([&](const auto& Hook)
                                                           { return Hook.Kind == ParentHook.Kind; }))
                        continue;
                    auto* Forward = new das::Function();
                    Forward->at = St->at;
                    Forward->generated = true;
                    Forward->privateFunction = false;
                    Forward->exports = true;
                    Forward->result = new das::TypeDecl(das::Type::tVoid, St->at);
                    const int Kind =
                        ParentHook.Kind ==
                        TEXT("BeginPlay")
                             ? 0
                                                                     : ParentHook.Kind ==
                                   TEXT("Tick")                      ? 1
                                                                     : ParentHook.Kind ==
                                                      TEXT("EndPlay")? 2 : ParentHook.Kind == TEXT("Initialize")? 3 : 4;
                    Forward->name = Kind == 0   ? "begin_play"
                                    : Kind == 1 ? "tick"
                                    : Kind == 2 ? "end_play"
                                    : Kind == 3 ? "__ue_initialize"
                                                : "__ue_construct";
                    if (Kind == 1 || Kind == 2)
                    {
                        auto* A = new das::Variable();
                        A->at = St->at;
                        A->name = "value";
                        A->type = new das::TypeDecl(Kind == 1 ? das::Type::tFloat : das::Type::tInt, St->at);
                        Forward->arguments.push_back(A);
                    }
                    auto* Body = new das::ExprBlock();
                    Body->at = St->at;
                    Body->list.push_back(
                        DSLCall(St->at, "__ue_parent_lifecycle",
                                {new das::ExprConstString(St->at, DSLUtf8(C.Id)), new das::ExprConstInt(St->at, Kind),
                                 Kind == 1 ? static_cast<das::ExpressionPtr>(new das::ExprVar(St->at, "value"))
                                           : static_cast<das::ExpressionPtr>(new das::ExprConstFloat(St->at, 0.f)),
                                 Kind == 2 ? static_cast<das::ExpressionPtr>(new das::ExprVar(St->at, "value"))
                                           : static_cast<das::ExpressionPtr>(new das::ExprConstInt(St->at, 0))}));
                    Forward->body = Body;
                    Generated.push_back(Forward);
                }
            }
            if (Derived && !BindPrototype &&
                BaseEffective.Callbacks.ContainsByPredicate([](const auto& Hook)
                                                            { return Hook.Kind == TEXT("event"); }))
            {
                // Do not resolve an imported parent's binder as this script's binder.
                auto* EmptyBind = new das::Function();
                EmptyBind->at = St->at;
                EmptyBind->name = "bind_events";
                EmptyBind->generated = true;
                EmptyBind->exports = true;
                EmptyBind->privateFunction = false;
                EmptyBind->result = new das::TypeDecl(das::Type::tVoid, St->at);
                auto* EmptyBody = new das::ExprBlock();
                EmptyBody->at = St->at;
                EmptyBind->body = EmptyBody;
                Generated.push_back(EmptyBind);
            }
            if (BindPrototype)
            {
                auto Bind = BindPrototype->clone();
                Bind->name = "bind_events";
                Bind->arguments.clear();
                Bind->body = BindBlock;
                Generated.push_back(Bind);
            }
            // Do this after cloning all runtime bodies. Source helpers retain normal
            // daScript self typing; only native fields lower to the checked runtime ABI.
            das::vector<das::FunctionPtr> NativeSources;
            St->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent == St && !F->generated && F->body && !F->arguments.empty())
                        NativeSources.push_back(F);
                });
            for (const auto& F : NativeSources)
            {
                FDSLNativeSourceVisitor Native(C, F->arguments[0]->name, St, TypeFolder, TypeRegistry, Group,
                                               HelpersGenerated, Helpers);
                Native.NativeOnly = true;
                F->body = F->body->visit(Native);
                if (!Native.Error.IsEmpty())
                    DSLFail(Native.Error);
            }
            DSLActiveRefDeclarations.Add(RefKey, C);
            auto Own = C;
            if (Derived)
            {
                if (!DSLAnnotation(St->annotations, "ue_async_physics"))
                    Own.AsyncPhysics.Reset();
                Own.NativeParent.Reset();
                Own.ParentId = BaseId;
                Own.Properties.RemoveAll(
                    [&](const auto& P)
                    {
                        return BaseEffective.Properties.ContainsByPredicate([&](const auto& Base)
                                                                            { return Base.Id == P.Id; });
                    });
                Own.Methods.RemoveAll(
                    [&](const auto& M)
                    {
                        return BaseEffective.Methods.ContainsByPredicate([&](const auto& Base)
                                                                         { return Base.Id == M.Id; });
                    });
                Own.Components.RemoveAll(
                    [&](const auto& P)
                    {
                        return BaseEffective.Components.ContainsByPredicate([&](const auto& Base)
                                                                            { return Base.Id == P.Id; });
                    });
                Own.Callbacks = OwnedCallbacks;
                Own.Interfaces.RemoveAll([&](const auto& P) { return BaseEffective.Interfaces.Contains(P); });
            }
            Model.Classes.Add(Own);
            FString Json, Issue;
            if (!Model.ToJson(Json, Issue))
                DSLFail(Issue);
            FDasClassSchema Supported;
            if ((!C.Methods.IsEmpty() || !C.Callbacks.IsEmpty() || C.AsyncPhysics.IsSet()) &&
                !Model.ToActorSchema(C.Id, Supported, Issue))
                DSLFail(Issue);
            for (const auto& F : HelpersGenerated)
                if (!St->module->addFunction(F, true))
                    DSLFail(TEXT("Direct struct helper collision"));
            for (const auto& F : Generated)
                if (!St->module->addFunction(F, true))
                    DSLFail(TEXT("Generated entry point collision"));
            Args.push_back(das::AnnotationArgument(DSLModelKey, DSLUtf8(Json)));
            return true;
        }
        catch (const std::exception& E)
        {
            Error = E.what();
            return false;
        }
    }
};
