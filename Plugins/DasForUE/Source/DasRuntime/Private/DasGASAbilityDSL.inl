struct FUEGASAbilityAnnotation : das::StructureAnnotation
{
    FUEGASAbilityAnnotation() : StructureAnnotation("ue_gameplay_ability") {}
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList& Args,
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
                DSLFail(TEXT("GameplayAbility declarations require a class without DAS inheritance"));
            for (const auto& F : S->fields)
                if (!F.generated && F.name != "__rtti")
                    DSLFail(TEXT("Declarative GameplayAbilities do not support fields or methods yet"));
            const FString Module = FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name));
            auto Object = MakeShared<FJsonObject>();
            Object->SetStringField(TEXT("kind"), TEXT("ability"));
            Object->SetStringField(TEXT("name"), DSLText(S->name));
            Object->SetStringField(TEXT("id"), DSLString(Args, "id", Module + TEXT(".") + DSLText(S->name)));
            Object->SetStringField(TEXT("effect"), DSLString(Args, "effect"));
            Object->SetStringField(TEXT("cost"), DSLString(Args, "cost"));
            Object->SetStringField(TEXT("cooldown_tag"), DSLString(Args, "cooldown_tag"));
            Object->SetArrayField(TEXT("fields"), {});
            for (const auto& A : Args)
            {
                if (A.name == "id" || A.name == "effect" || A.name == "cost" || A.name == "cooldown_tag")
                    continue;
                if (A.name == "auto_end")
                    Object->SetBoolField(TEXT("auto_end"), DSLBool(A));
                else if (A.name == "cooldown")
                {
                    if (A.type != das::Type::tInt && A.type != das::Type::tFloat)
                        DSLFail(TEXT("Ability cooldown requires seconds as a number"));
                    Object->SetNumberField(TEXT("cooldown"),
                                                A.type == das::Type::tInt ? double(A.iValue) : double(A.fValue));
                }
                else
                    DSLFail(TEXT("Unknown GameplayAbility declaration option"));
            }
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
