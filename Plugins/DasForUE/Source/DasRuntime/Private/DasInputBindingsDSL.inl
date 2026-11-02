// Input annotations expand to the ordinary checked Enhanced Input callback path.
struct FUEInputAnnotation : das::FunctionAnnotation
{
    FUEInputAnnotation() : FunctionAnnotation("ue_input") {}
    virtual bool apply(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList&,
                       das::string& Error) override
    {
        if (!F->classParent || F->isStaticClassMethod)
        {
            Error = "ue_input requires an instance method of ue_class";
            return false;
        }
        return true;
    }
    virtual bool finalize(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string& Error) override
    {
        if (!F->classParent || !DSLAnnotation(F->classParent->annotations, "ue_class"))
        {
            Error = "ue_input requires ue_class";
            return false;
        }
        return true;
    }
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string& Error) override
    {
        Error = "ue_input cannot annotate a block";
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
};

void DSLNormalizeInputBindings(const das::StructurePtr& St, das::ModuleGroup& Group)
{
    struct FBinding
    {
        das::string Action, Trigger, Method;
    };
    das::vector<FBinding> Bindings;
    TSet<FString> Keys;
    DSLVisitHandlerMethods(
        St,
        [&](const das::FunctionPtr& F)
        {
            const auto* A = DSLAnnotation(F->annotations, "ue_input");
            if (!A)
                return;
            FDSLDiagnosticLocationScope Diagnostic(F->at, DSLDiagnosticFunctionEntity(F));
            const FString Action = DSLString(A->arguments, "action");
            const auto* Field = St->findField(DSLUtf8(Action));
            if (!Field || Field->classMethod || Field->generated)
                DSLFail(TEXT("ue_input action must name an InputActionRef property: ") + Action);
            auto Type = DSLObjectNormalize(Field->type, Group, St->module);
            if (!DSLTypedObject(Type) ||
                DSLTypedObjectPath(Type) != TEXT("/Script/EnhancedInput.InputAction") ||
                                                 !std::any_of(Field->annotation.begin(), Field->annotation.end(),
                                                              [](const auto& Arg)
                                                              { return Arg.name == "ue_property"; }))
                DSLFail(TEXT("ue_input action must name an InputActionRef property: ") + Action);
            if (F->isStaticClassMethod || !F->result || F->result->baseType != das::Type::tVoid ||
                F->arguments.size() != 2 || F->arguments[1]->type->ref || F->arguments[1]->init)
                DSLFail(TEXT("ue_input requires a void handler with one by-value input argument"));
            const auto Input = DSLObjectNormalize(F->arguments[1]->type, Group, St->module);
            const auto Kind = DSLTypeName(Input);
            const auto Path = DSLMathPath(Input);
            if (Kind != TEXT("bool") &&
                             Kind != TEXT("float") &&
                                          Path != TEXT("/Script/EnhancedInput.InputActionValue") &&
                                                       Path != TEXT("/Script/CoreUObject.Vector") &&
                                                                    Path != TEXT("/Script/CoreUObject.Vector2D"))
                DSLFail(TEXT("ue_input requires InputActionValue/bool/float/UEVector2D/UEVector input"));
            int32 Triggers = 0;
            TSet<FString> Arguments;
            for (const auto& Arg : A->arguments)
            {
                const FString Name = DSLText(Arg.name);
                if (Arguments.Contains(Name))
                    DSLFail(TEXT("Repeated ue_input argument: ") + Name);
                Arguments.Add(Name);
                if (Name == TEXT("action") && Arg.type == das::Type::tString)
                    continue;
                if (Arg.type != das::Type::tBool || !Arg.bValue ||
                    (Name != TEXT("Triggered") && Name != TEXT("Started") &&
                                                               Name != TEXT("Ongoing") &&
                                                                            Name != TEXT("Canceled") &&
                                                                                         Name != TEXT("Completed")))
                    DSLFail(TEXT("ue_input expects action=Property and Triggered/Started/Ongoing/Canceled/Completed flags"));
                const FString Key = Action.ToLower() + TEXT(":") + Name;
                if (Keys.Contains(Key))
                    DSLFail(TEXT("Duplicate ue_input action/trigger binding: ") + Key);
                Keys.Add(Key);
                Bindings.push_back({DSLUtf8(Action), Arg.name, DSLMethodName(F)});
                ++Triggers;
            }
            if (!Triggers)
                DSLFail(TEXT("ue_input requires at least one trigger flag"));
        });
    // Stable expansion regardless of module hash-table iteration order.
    std::sort(Bindings.begin(), Bindings.end(),
              [](const auto& A, const auto& B)
              {
                  if (A.Action != B.Action)
                      return A.Action < B.Action;
                  if (A.Trigger != B.Trigger)
                      return A.Trigger < B.Trigger;
                  return A.Method < B.Method;
              });
    struct FExpand : das::Visitor
    {
        const das::vector<FBinding>& Bindings;
        das::string Self;
        bool Static;
        das::EnumerationPtr TriggerEnum;
        FExpand(const das::vector<FBinding>& B, das::EnumerationPtr Enum) : Bindings(B), TriggerEnum(Enum) {}
        virtual das::ExpressionPtr visit(das::ExprCall* E) override
        {
            if (E->name != "ue_bind_inputs" && E->name != "ue_enhanced_input::ue_bind_inputs")
                return E;
            if (!E->arguments.empty() || Static || Bindings.empty())
                DSLFail(TEXT("ue_bind_inputs() requires an instance method and at least one ue_input handler"));
            auto* Block = new das::ExprBlock();
            Block->at = E->at;
            for (const auto& B : Bindings)
            {
                auto* Enum = new das::TypeDecl(das::Type::tEnumeration, E->at);
                Enum->enumType = TriggerEnum;
                Block->list.push_back(DSLCall(E->at, "ue_enhanced_input::ue_enhanced_bind",
                                              {new das::ExprField(E->at, new das::ExprVar(E->at, Self), B.Action),
                                               new das::ExprConstEnumeration(E->at, B.Trigger, Enum),
                                               new das::ExprField(E->at, new das::ExprVar(E->at, Self), B.Method)}));
            }
            return Block;
        }
    };
    das::EnumerationPtr TriggerEnum = nullptr;
    if (!Bindings.empty())
    {
        const auto Enums = Group.findEnum("ue_enhanced_input::TriggerEvent", St->module);
        if (Enums.size() != 1)
            DSLFail(TEXT("ue_input requires ue_enhanced_input"));
        TriggerEnum = Enums[0];
    }
    FExpand Expand(Bindings, TriggerEnum);
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent != St || !F->body || F->arguments.empty())
                return;
            Expand.Self = F->arguments[0]->name;
            Expand.Static = F->isStaticClassMethod;
            F->body = F->body->visit(Expand);
        });
}
