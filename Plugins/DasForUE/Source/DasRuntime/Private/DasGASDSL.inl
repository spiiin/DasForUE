// Declaration-only authoring. No DAS object layout is used as UAttributeSet storage.
const char* DSLGASKey = "_ue_gas_declaration";
struct FUEGASDeclarationAnnotation : das::StructureAnnotation
{
    bool Effect;
    explicit FUEGASDeclarationAnnotation(bool IsEffect)
        : StructureAnnotation(IsEffect ? "ue_gameplay_effect" : "ue_attribute_set"), Effect(IsEffect)
    {
    }
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
            DSLAnnotationArguments(Args, Effect ? TSet<FString>{TEXT("cue")} : TSet<FString>{});
            if (!S->isClass || S->parent)
                DSLFail(TEXT("GAS declarations require a class without DAS inheritance"));
            for (const auto& A : Args)
                if (A.name != "id" &&
                    (!Effect ||
                     (A.name != "cue" && A.name != "attributes" && A.name != "execution" && A.name != "duration" &&
                      A.name != "period" && A.name != "execute_on_application" && A.name != "stacking" &&
                      A.name != "stack_limit" && A.name != "stack_duration" && A.name != "stack_period" &&
                      A.name != "stack_expiration" && A.name != "deny_overflow")))
                    DSLFail(TEXT("Unknown GAS declaration option"));
            const FString Name = DSLText(S->name), Module = FPaths::GetBaseFilename(DSLText(S->at.fileInfo->name));
            auto Object = MakeShared<FJsonObject>();
            Object->SetStringField(TEXT("kind"), Effect ? TEXT("effect"): TEXT("attributes"));
            Object->SetStringField(TEXT("name"), Name);
            Object->SetStringField(TEXT("id"), DSLString(Args, "id", Module + TEXT(".")+ Name));
            if (Effect)
            {
                Object->SetStringField(TEXT("attributes"), DSLString(Args, "attributes"));
                TArray<TSharedPtr<FJsonValue>> Cues;
                for (const auto& A : Args)
                {
                    const FString Key = DSLText(A.name);
                    if (Key == TEXT("id") || Key == TEXT("attributes"))
                        continue;
                    if (Key == TEXT("cue"))
                    {
                        if (A.type != das::Type::tString)
                            DSLFail(TEXT("cue requires a declaration name"));
                        Cues.Add(MakeShared<FJsonValueString>(DSLText(A.sValue)));
                    }
                    else if (Key == TEXT("duration") && (A.type == das::Type::tInt || A.type == das::Type::tFloat))
                    {
                        Object->SetStringField(TEXT("duration"), TEXT("HasDuration"));
                        Object->SetNumberField(TEXT("seconds"),
                                                    A.type == das::Type::tInt ? double(A.iValue) : double(A.fValue));
                    }
                    else if (Key == TEXT("period") || Key == TEXT("stack_limit"))
                    {
                        if ((Key == TEXT("stack_limit") && A.type != das::Type::tInt) ||
                             (A.type != das::Type::tInt && A.type != das::Type::tFloat))
                            DSLFail(TEXT("GAS period requires a number; stack_limit requires an integer"));
                        Object->SetNumberField(Key, A.type == das::Type::tInt ? double(A.iValue) : double(A.fValue));
                    }
                    else if (Key == TEXT("execute_on_application") || Key == TEXT("deny_overflow"))
                    {
                        if (A.type != das::Type::tBool)
                            DSLFail(TEXT("GAS execution and overflow options require booleans"));
                        Object->SetBoolField(Key, A.bValue);
                    }
                    else
                    {
                        if (A.type != das::Type::tString)
                            DSLFail(TEXT("GAS duration/stacking policies require strings"));
                        Object->SetStringField(Key, DSLText(A.sValue));
                    }
                }
                Object->SetArrayField(TEXT("cues"), Cues);
            }
            TArray<TSharedPtr<FJsonValue>> Fields;
            for (const auto& F : S->fields)
            {
                if (F.generated || F.name == "__rtti")
                    continue;
                if (F.classMethod || !F.type || DSLTypeName(F.type) != TEXT("float"))
                    DSLFail(TEXT("GAS declarations support only float fields; unsupported member: ") + DSLText(F.name));
                auto Value = DSLParameterDefault(F.init);
                if (!Value.Kind.IsEmpty() && Value.Kind != TEXT("number"))
                    DSLFail(TEXT("GAS defaults require numeric literals"));
                if (!FMath::IsFinite(Value.Number) || FMath::Abs(Value.Number) > MAX_flt)
                    DSLFail(TEXT("GAS literal exceeds finite float range"));
                auto Field = MakeShared<FJsonObject>();
                Field->SetStringField(TEXT("name"), DSLText(F.name));
                Field->SetNumberField(TEXT("value"), Value.Number);
                TSet<FString> Seen;
                for (const auto& A : F.annotation)
                {
                    const FString Key = DSLText(A.name);
                    if (Seen.Contains(Key))
                        DSLFail(TEXT("Repeated GAS field annotation"));
                    Seen.Add(Key);
                    if (!Effect && (Key == TEXT("min") || Key == TEXT("max")))
                    {
                        if (Key == TEXT("max") && A.type == das::Type::tString)
                            Field->SetStringField(TEXT("max_attribute"), DSLText(A.sValue));
                        else if (A.type == das::Type::tInt || A.type == das::Type::tFloat)
                            Field->SetNumberField(Key, A.type == das::Type::tInt ? double(A.iValue) : double(A.fValue));
                        else
                            DSLFail(TEXT("Attribute min/max requires a number; max also accepts a field name"));
                    }
                    else if (Effect &&
                             (Key == TEXT("operation") || Key == TEXT("set_by_caller") || Key == TEXT("calculation")))
                    {
                        if (A.type != das::Type::tString)
                            DSLFail(TEXT("Modifier annotations require strings"));
                        Field->SetStringField(Key, DSLText(A.sValue));
                    }
                    else
                        DSLFail(TEXT("Unknown GAS field annotation"));
                }
                Fields.Add(MakeShared<FJsonValueObject>(Field));
            }
            Object->SetArrayField(TEXT("fields"), Fields);
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
