struct FUEGASCueAnnotation : das::StructureAnnotation
{
    FUEGASCueAnnotation() : StructureAnnotation("ue_gameplay_cue") {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(S->at, UTF8_TO_TCHAR(S->name.c_str()));
        try
        {
            if (const auto* Done = Args.find(DSLGASKey, das::Type::tString))
            {
                if (Done->at.fileInfo)
                    DSLFail(TEXT("Reserved GAS declaration argument"));
                return true;
            }
            DSLAnnotationArguments(Args);
            if (!S->isClass || S->parent)
                DSLFail(TEXT("GameplayCue declarations require a class without DAS inheritance"));

            const FString Module = FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name));
            auto Object = MakeShared<FJsonObject>();
            Object->SetStringField(TEXT("kind"), TEXT("cue"));
            Object->SetStringField(TEXT("name"), DSLText(S->name));
            Object->SetStringField(TEXT("id"), DSLString(Args, "id", Module + TEXT(".") + DSLText(S->name)));
            Object->SetStringField(TEXT("tag"), DSLString(Args, "tag"));
            Object->SetArrayField(TEXT("fields"), {});
            for (const auto& A : Args)
            {
                if (A.name == "id" || A.name == "tag")
                    continue;
                if (A.name != "min_level" && A.name != "max_level")
                    DSLFail(TEXT("Unknown GameplayCue option"));
                if (A.type != das::Type::tInt && A.type != das::Type::tFloat)
                    DSLFail(TEXT("Cue bounds require numeric literals"));
                Object->SetNumberField(DSLText(A.name),
                                       A.type == das::Type::tInt ? double(A.iValue) : double(A.fValue));
            }
            TMap<FString, das::FunctionPtr> Hooks;
            S->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent != S || F->generated)
                        return;
                    if (DSLAnnotation(F->annotations, "ue_function") || DSLAnnotation(F->annotations, "ue_event") ||
                        DSLAnnotation(F->annotations, "ue_lifecycle"))
                        DSLFail(TEXT("Cue methods are ordinary DAS methods; reflected UE method annotations are not supported here"));
                    const FString Name = DSLText(
                        F->name.substr(F->name.find_last_of(das::string(1, char(96)) + das::string(1, char(39))) + 1));
                    if (Name != TEXT("Execute") && Name != TEXT("OnActive") && Name != TEXT("WhileActive") &&
                                                                                            Name != TEXT("Removed"))
                        return;
                    if (F->result && F->result->baseType == das::Type::autoinfer)
                        F->result = new das::TypeDecl(das::Type::tVoid, F->at);
                    if (Hooks.Contains(Name) || F->isStaticClassMethod || F->arguments.size() != 2 || !F->result ||
                        DSLTypeName(F->result) != TEXT("void") || F->arguments[1]->type->ref ||
                                                       !DSLNativeStruct(F->arguments[1]->type) ||
                                                       DSLValidateNativeStruct(F->arguments[1]->type) !=
                                                           TEXT("/Script/DasRuntime.DasGASCueEvent"))
                        DSLFail(TEXT("Cue hooks require a unique instance method (context:GASCueEvent):void"));
                    Hooks.Add(Name, F);
                });
            if (!Hooks.IsEmpty())
            {
                const auto InstanceName = DSLUtf8(TEXT("__ue_cue_instance_") + DSLText(S->name));
                auto ClassType = new das::TypeDecl(das::Type::tStructure, S->at);
                ClassType->structType = S;
                das::VariablePtr Instance = new das::Variable();
                Instance->at = S->at;
                Instance->name = InstanceName;
                Instance->generated = true;
                Instance->private_variable = true;
                Instance->type = new das::TypeDecl(das::Type::tPointer, S->at);
                Instance->type->firstType = ClassType;
                Instance->init = new das::ExprNew(S->at, ClassType, true);
                if (!S->module->addVariable(Instance))
                    DSLFail(TEXT("Cue instance name collision"));
                das::vector<das::Function*> Generated;
                TSet<FString> Helpers;
                for (const auto& Pair : Hooks)
                {
                    const auto& F = Pair.Value;
                    const auto ContextType = DSLCopyType(F->arguments[1]->type);
                    const FString Path = TEXT("/Script/DasRuntime.DasGASCueEvent");
                    DSLEnsureStructHelpers(ContextType, Path, Group, S->module, Generated, Helpers);
                    const FString Entry = TEXT("__ue_cue_") + DSLText(S->name) + TEXT("_") + Pair.Key;
                    Object->SetStringField(Pair.Key.ToLower(), Entry);
                    das::FunctionPtr Wrapper = new das::Function();
                    Wrapper->at = F->at;
                    Wrapper->name = DSLUtf8(Entry);
                    Wrapper->generated = true;
                    Wrapper->exports = true;
                    Wrapper->privateFunction = false;
                    Wrapper->result = new das::TypeDecl(das::Type::tVoid, F->at);
                    auto* Arg = new das::Variable();
                    Arg->at = F->at;
                    Arg->name = "__ue_cue_context";
                    Arg->type = DSLHandleType(Group, S->module, "UEStructValue");
                    Arg->type->constant = true;
                    Wrapper->arguments.push_back(Arg);
                    auto* Call = das::makeInvokeMethod(F->at, new das::ExprVar(F->at, InstanceName), DSLUtf8(Pair.Key));
                    Call->arguments.push_back(
                        DSLDirectUnpack(F->at, ContextType, new das::ExprVar(F->at, Arg->name), Path));
                    auto* Body = new das::ExprBlock();
                    Body->at = F->at;
                    Body->list.push_back(Call);
                    Wrapper->body = Body;
                    Generated.push_back(Wrapper);
                }
                for (const auto& F : Generated)
                    if (!S->module->addFunction(F, true))
                        DSLFail(TEXT("Cue hook entry collision"));
                Object->SetStringField(TEXT("script"), FPaths::GetCleanFilename(DSLText(S->at.fileInfo->name)));
                FString Contract;
                FJsonSerializer::Serialize(Object, TJsonWriterFactory<>::Create(&Contract));
                Object->SetStringField(TEXT("contract"), FMD5::HashAnsiString(*Contract));
            }
            else
                for (const auto& Field : S->fields)
                    if (!Field.generated && !Field.classMethod && Field.name != "__rtti")
                        DSLFail(TEXT("Cue fields require at least one cue hook"));
            FString Wire;
            FJsonSerializer::Serialize(Object, TJsonWriterFactory<>::Create(&Wire));
            Args.push_back(das::AnnotationArgument(DSLGASKey, DSLUtf8(Wire)));
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
        FDSLDiagnosticLocationScope SourceDiagnostic(S->at, UTF8_TO_TCHAR(S->name.c_str()));
        try
        {
            DSLDeclarationKind(S->annotations);
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
