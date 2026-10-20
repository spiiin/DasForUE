struct FUEGASCalculationAnnotation : das::StructureAnnotation
{
    FUEGASCalculationAnnotation() : StructureAnnotation("ue_gas_calculation") {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
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
                DSLFail(TEXT("GAS calculation requires a class without inheritance"));
            for (const auto& A : Args)
                if (A.name != "id" && A.name != "kind" && A.name != "attributes" && A.name != "output" &&
                    A.name != "operation")
                    DSLFail(TEXT("Unknown GAS calculation option"));
            auto O = MakeShared<FJsonObject>();
            const auto Name = DSLText(S->name);
            O->SetStringField(TEXT("kind"), TEXT("calculation"));
            O->SetStringField(TEXT("name"), Name);
            O->SetStringField(TEXT("id"), DSLString(Args, "id", FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name)) + TEXT(".")+ Name));
            O->SetStringField(TEXT("calculation_kind"), DSLString(Args, "kind", TEXT("Magnitude")));
            O->SetStringField(TEXT("attributes"), DSLString(Args, "attributes"));
            O->SetStringField(TEXT("output"), DSLString(Args, "output"));
            O->SetStringField(TEXT("operation"), DSLString(Args, "operation", TEXT("Add")));
            TArray<TSharedPtr<FJsonValue>> Fields;
            for (const auto& F : S->fields)
            {
                if (F.generated || F.classMethod || F.name == "__rtti")
                    continue;
                if (!F.type || DSLTypeName(F.type) != TEXT("float"))
                    DSLFail(TEXT("Calculation capture declarations must be float fields"));
                const auto V = DSLParameterDefault(F.init);
                if ((!V.Kind.IsEmpty() && V.Kind != TEXT("number")) || V.Number != 0)
                    DSLFail(TEXT("Capture field defaults must be zero"));
                auto C = MakeShared<FJsonObject>();
                C->SetStringField(TEXT("name"), DSLText(F.name));
                C->SetStringField(TEXT("attribute"), DSLText(F.name));
                C->SetStringField(TEXT("attributes"), DSLString(Args, "attributes"));
                C->SetStringField(TEXT("source"), TEXT("Source"));
                C->SetBoolField(TEXT("snapshot"), true);
                TSet<FString> Seen;
                for (const auto& A : F.annotation)
                {
                    const auto K = DSLText(A.name);
                    if (Seen.Contains(K))
                        DSLFail(TEXT("Repeated capture annotation"));
                    Seen.Add(K);
                    if (K == TEXT("snapshot"))
                    {
                        if (A.type != das::Type::tBool)
                            DSLFail(TEXT("snapshot requires bool"));
                        C->SetBoolField(K, A.bValue);
                    }
                    else if (K == TEXT("source")|| K == TEXT("attribute")|| K == TEXT("attributes"))
                    {
                        if (A.type != das::Type::tString)
                            DSLFail(TEXT("Capture options require names"));
                        C->SetStringField(K == TEXT("source")? TEXT("source"): K, DSLText(A.sValue));
                    }
                    else
                        DSLFail(TEXT("Unknown capture field option"));
                }
                Fields.Add(MakeShared<FJsonValueObject>(C));
            }
            O->SetArrayField(TEXT("fields"), Fields);
            das::FunctionPtr Method = nullptr;
            S->module->functions.foreach (
                [&](const das::FunctionPtr& F)
                {
                    if (F->classParent != S || F->generated)
                        return;
                    if (DSLText(F->name.substr(F->name.find_last_of('`') + 1)) != TEXT("Calculate"))
                        return;
                    if (Method || !F->isStaticClassMethod || F->arguments.size() != 1 ||
                        DSLTypeName(F->result) != TEXT("float")|| F->arguments[0]->type->ref ||
                                                       !DSLNativeStruct(F->arguments[0]->type) ||
                                                       DSLValidateNativeStruct(F->arguments[0]->type) !=
                                                           TEXT("/Script/DasRuntime.DasGASCalculationContext"))
                        DSLFail(
                            TEXT("Calculation requires unique static Calculate(context:GASCalculationContext):float"));
                    Method = F;
                });
            if (!Method)
                DSLFail(TEXT("Missing static Calculate(context:GASCalculationContext):float"));
            const auto Entry = TEXT("__ue_gas_calculate_")+ Name;
            O->SetStringField(TEXT("entry"), Entry);
            O->SetStringField(TEXT("script"), FPaths::GetCleanFilename(DSLText(S->at.fileInfo->name)));
            FString Contract;
            FJsonSerializer::Serialize(O, TJsonWriterFactory<>::Create(&Contract));
            O->SetStringField(TEXT("contract"), FMD5::HashAnsiString(*Contract));
            das::vector<das::FunctionPtr> Generated;
            TSet<FString> Helpers;
            const auto T = DSLCopyType(Method->arguments[0]->type);
            const auto Path = FString(TEXT("/Script/DasRuntime.DasGASCalculationContext"));
            DSLEnsureStructHelpers(T, Path, Group, S->module, Generated, Helpers);
            auto* F = new das::Function();
            F->at = Method->at;
            F->name = DSLUtf8(Entry);
            F->generated = true;
            F->exports = true;
            F->result = new das::TypeDecl(das::Type::tFloat, F->at);
            auto* A = new das::Variable();
            A->at = F->at;
            A->name = "__ue_calculation_context";
            A->type = DSLHandleType(Group, S->module, "UEStructValue");
            A->type->constant = true;
            F->arguments.push_back(A);
            auto* Call =
                DSLCall(F->at, Method->name, {DSLDirectUnpack(F->at, T, new das::ExprVar(F->at, A->name), Path)});
            auto* Return = new das::ExprReturn();
            Return->at=F->at;
            Return->subexpr = Call;
            auto* Body = new das::ExprBlock();
            Body->at = F->at;
            Body->list.push_back(Return);
            F->body = Body;
            Generated.push_back(F);
            for (auto* Fn : Generated)
                if (!S->module->addFunction(Fn, true))
                    DSLFail(TEXT("Calculation entry name collision"));
            FString Wire;
            FJsonSerializer::Serialize(O, TJsonWriterFactory<>::Create(&Wire));
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
