// Part of DasDSL.inl: ue_function, callback, ue_implements, async physics, component and native base annotations.
// Included in place; declaration order matters.
struct FDSLNativeSourceVisitor : FDSLBodyVisitor
{
    using FDSLBodyVisitor::FDSLBodyVisitor;
    virtual das::ExpressionPtr visit(das::ExprCall* E) override
    {
        return ContainerOperation(E->name) ? FDSLBodyVisitor::visit(E) : E;
    }
    virtual das::ExpressionPtr visit(das::ExprInvoke* E) override { return NativeSelfCall(E); }
    virtual das::ExpressionPtr visit(das::ExprWith* E) override { return E; }
};
struct FUEFunctionAnnotation : das::FunctionAnnotation
{
    FUEFunctionAnnotation() : FunctionAnnotation("ue_function") {}
    virtual bool apply(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList&,
                       das::string& E) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(F->at, DSLDiagnosticFunctionEntity(F));
        if (!F->classParent)
        {
            E = "ue_function must be a member of ue_class or ue_interface";
            return false;
        }
        return true;
    }
    virtual bool finalize(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(F->at, DSLDiagnosticFunctionEntity(F));
        try
        {
            DSLAnnotation(F->annotations, "ue_function");
            if (!F->classParent || (!DSLAnnotation(F->classParent->annotations, "ue_class") &&
                                    !DSLAnnotation(F->classParent->annotations, "ue_interface") &&
                                    !DSLAnnotation(F->classParent->annotations, "ue_library")))
                DSLFail(TEXT("ue_function requires ue_class or ue_interface"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string& E) override
    {
        E = "ue_function cannot annotate block";
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
};
bool DSLStandardLifecycle(const das::FunctionPtr& F)
{
    const FString Name = DSLText(DSLMethodName(F));
    return Name == TEXT("ConstructionScript") ||
                        Name == TEXT("Deinitialize") || Name == TEXT("Initialize") ||
                                                                     Name == TEXT("BeginPlay") ||
                                                                                  Name == TEXT("Tick") ||
                                                                                               Name == TEXT("EndPlay");
}
struct FUECallbackAnnotation : das::FunctionAnnotation
{
    bool Event;
    FUECallbackAnnotation(bool InEvent) : FunctionAnnotation(InEvent ? "ue_event" : "ue_lifecycle"), Event(InEvent) {}
    virtual bool apply(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(F->at, DSLDiagnosticFunctionEntity(F));
        try
        {
            if (!F->classParent || F->isStaticClassMethod)
                DSLFail(TEXT("UE callbacks require an instance method of ue_class"));
            DSLAnnotationArguments(Args);
            if (!Event && !Args.empty())
                DSLFail(TEXT("ue_lifecycle takes no arguments; name the method ConstructionScript, Initialize, Deinitialize, BeginPlay, Tick or EndPlay"));
            if (Event)
            {
                for (const auto& A : Args)
                    if ((A.name != "source" && A.name != "name" && A.name != "source_type") ||
                        A.type != das::Type::tString)
                        DSLFail(TEXT("Expected ue_event(source=self|owner, name=DelegateName)"));
                if (DSLString(Args, "name").IsEmpty())
                    DSLFail(TEXT("ue_event requires delegate name"));
            }
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool finalize(const das::FunctionPtr& F, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(F->at, DSLDiagnosticFunctionEntity(F));
        try
        {
            DSLAnnotation(F->annotations, Event ? "ue_event" : "ue_lifecycle");
            if (!F->classParent || !DSLAnnotation(F->classParent->annotations, "ue_class"))
                DSLFail(TEXT("UE callbacks require ue_class"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string& Error) override
    {
        Error = "UE callbacks cannot annotate blocks";
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
};
struct FUEImplementsAnnotation : das::StructureAnnotation
{
    FUEImplementsAnnotation() : StructureAnnotation("ue_implements") {}
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            if (!St->isClass)
                DSLFail(TEXT("ue_implements requires ue_class"));
            if (Args.size() != 1 || Args[0].name != "interface" || Args[0].type != das::Type::tString ||
                DSLString(Args, "interface").IsEmpty())
                DSLFail(TEXT("Expected ue_implements(interface=NameOrPath)"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        if (!DSLAnnotation(St->annotations, "ue_class"))
        {
            Error = "ue_implements requires ue_class";
            return false;
        }
        return true;
    }
};
FDasDeclaredAsyncPhysics DSLAsyncPhysics(const das::AnnotationArgumentList& Args)
{
    FDasDeclaredAsyncPhysics Spec;
    TSet<FString> Seen;
    for (const auto& A : Args)
    {
        const FString Name = DSLText(A.name);
        if (Seen.Contains(Name) || A.type != das::Type::tString)
            DSLFail(TEXT("Duplicate/non-string ue_async_physics argument"));
        Seen.Add(Name);
        if (Name == TEXT("script"))
        {
            Spec.ScriptFile = DSLText(A.sValue);
            continue;
        }
        if (Name.Len() != 6 || !Name.StartsWith(TEXT("body_")) || Name[5] < TCHAR('0') || Name[5] > TCHAR('7'))
            DSLFail(TEXT("Expected ue_async_physics(script=File.das, body_0=Component, ... body_7=Component)"));
        Spec.Bodies.Add(Name[5] - TCHAR('0'), DSLText(A.sValue));
    }
    if (Spec.ScriptFile.IsEmpty())
        DSLFail(TEXT("ue_async_physics requires script"));
    return Spec;
}
struct FUEAsyncPhysicsAnnotation : das::StructureAnnotation
{
    FUEAsyncPhysicsAnnotation() : StructureAnnotation("ue_async_physics") {}
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            if (!St->isClass)
                DSLFail(TEXT("ue_async_physics requires ue_class"));
            DSLAsyncPhysics(Args);
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            DSLAnnotation(St->annotations, "ue_async_physics");
            if (!DSLAnnotation(St->annotations, "ue_class"))
                DSLFail(TEXT("ue_async_physics requires ue_class"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEComponentAnnotation : das::StructureAnnotation
{
    FUEComponentAnnotation() : StructureAnnotation("ue_component") {}
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            if (!St->isClass)
                DSLFail(TEXT("ue_component requires ue_class"));
            DSLAnnotationArguments(Args, {});
            for (const auto& A : Args)
                if ((A.name != "name" && A.name != "script" && A.name != "type" && A.name != "id") ||
                    A.type != das::Type::tString)
                    DSLFail(TEXT("Expected ue_component(name=Name, script=File.das, type=ClassName)"));
            if (DSLString(Args, "name").IsEmpty() || DSLString(Args, "script").IsEmpty() ||
                DSLString(Args, "type").IsEmpty())
                DSLFail(TEXT("Component name, script and type are required"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            if (!DSLAnnotation(St->annotations, "ue_class"))
                DSLFail(TEXT("ue_component requires ue_class"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
// Source methods are type-checked as well as exported clones. Lower timer
// captures there too, so array payloads use the same snapshot ABI.
struct FDSLSourceTimerVisitor : das::Visitor
{
    FDSLBodyVisitor& Rewrite;
    explicit FDSLSourceTimerVisitor(FDSLBodyVisitor& In) : Rewrite(In) {}
    virtual das::ExpressionPtr visit(das::ExprCall* E) override
    {
        if (E->name == "ue_effect_play" || E->name == "ue_effects::ue_effect_play" || E->name == "ue_effect_at" ||
            E->name == "ue_effects::ue_effect_at" || E->name == "ue_sound_play" ||
            E->name == "ue_effects::ue_sound_play" || E->name == "ue_niagara_spawn" ||
            E->name == "ue_effects::ue_niagara_spawn")
            return Rewrite.RewriteEffect(E);
        if (E->name == "ue_ai_move_to_actor" || E->name == "ue_ai::ue_ai_move_to_actor" ||
            E->name == "ue_ai_move_to_location" || E->name == "ue_ai::ue_ai_move_to_location")
            return Rewrite.RewriteAIMove(E);
        if (E->name == "ue_montage_play" || E->name == "ue_animation::ue_montage_play")
            return Rewrite.RewriteMontage(E);
        if (E->name == "ue_timer_start" || E->name == "ue_foundation::ue_timer_start" ||
            E->name == "ue_timer_next_tick" || E->name == "ue_foundation::ue_timer_next_tick")
            return Rewrite.RewriteTimer(E);
        return E;
    }
};
// Native host declarations carry identity only; they are not authored UE classes.
FString DSLNativeBasePath(const das::StructurePtr& St)
{
    const auto* A = St ? DSLAnnotation(St->annotations, "ue_native_base") : nullptr;
    return A ? DSLString(A->arguments, "path") : FString();
}
struct FUENativeBaseAnnotation : das::StructureAnnotation
{
    FUENativeBaseAnnotation() : StructureAnnotation("ue_native_base") {}
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            DSLAnnotationArguments(Args);
            if (Args.size() != 1 || Args[0].name != "path")
                DSLFail(TEXT("Expected ue_native_base(path=NativeClassPath)"));
            const FString Path = DSLString(Args, "path");
            if (!DasActorNativeParent(Path) &&
                Path != TEXT("/Script/Engine.WorldSubsystem") &&
                             Path != TEXT("/Script/Engine.GameInstanceSubsystem") &&
                                          Path != TEXT("/Script/Engine.ActorComponent") &&
                                                       Path !=
                                                           TEXT("/Script/Engine.SaveGame") &&
                                                                Path !=
                                                                    TEXT("/Script/Engine.DataAsset") &&
                                                                         Path !=
                                                                             TEXT("/Script/Engine.PrimaryDataAsset"))
                DSLFail(TEXT("Unsupported native DSL base; expected Actor, Pawn, Character, ActorComponent, SaveGame, DataAsset or PrimaryDataAsset"));
            auto* Native = FindObject<UClass>(nullptr, *Path);
            if (!Native || !Native->HasAnyClassFlags(CLASS_Native) || Native->HasAnyClassFlags(CLASS_Interface))
                DSLFail(TEXT("Native DSL base class is not loaded"));
            if (!St->isClass || St->parent)
                DSLFail(TEXT("Native DSL base requires an empty root class"));
            for (const auto& F : St->fields)
                if (!F.generated && F.name != "__rtti")
                    DSLFail(TEXT("Native DSL base cannot declare fields or methods"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    virtual bool look(const das::StructurePtr& St, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        FDSLDiagnosticLocationScope SourceDiagnostic(St->at, UTF8_TO_TCHAR(St->name.c_str()));
        try
        {
            DSLDeclarationKind(St->annotations);
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
void DSLValidateConstSource(const das::FunctionPtr& Function, const das::StructurePtr& Source)
{
    if (!Function->isConstClassMethod || !Function->body || Function->arguments.empty())
        return;
    struct FCheck : das::Visitor
    {
        das::string Self;
        das::StructurePtr Source;
        FCheck(const das::string& Name, const das::StructurePtr& St) : Self(Name), Source(St) {}
        bool SelfPath(das::Expression* E)
        {
            for (int Depth = 0; E && Depth < 64; ++Depth)
            {
                if (E->rtti_isVar())
                    return static_cast<das::ExprVar*>(E)->name == Self;
                if (E->rtti_isField())
                    E = static_cast<das::ExprField*>(E)->value;
                else if (E->rtti_isAt())
                    E = static_cast<das::ExprAt*>(E)->subexpr;
                else
                    return false;
            }
            return false;
        }
        void Write(das::Expression* E)
        {
            if (SelfPath(E))
                DSLFail(TEXT("Const method cannot modify self fields"));
        }
        virtual void preVisit(das::ExprCopy* E) override { Write(E->left); }
        virtual void preVisit(das::ExprMove* E) override { Write(E->left); }
        virtual void preVisit(das::ExprClone* E) override { Write(E->left); }
        virtual void preVisit(das::ExprOp2* E) override
        {
            if (E->op == "+=" || E->op == "-=" || E->op == "*=" || E->op == "/=" || E->op == "%=" || E->op == "|=" ||
                E->op == "&=" || E->op == "^=" || E->op == "<<=" || E->op == ">>=")
                Write(E->left);
        }
        virtual void preVisit(das::ExprOp1* E) override
        {
            if (E->op == "++" || E->op == "--" || E->op == "+++")
                Write(E->subexpr);
        }
        void Method(const das::string& Name, bool Super)
        {
            bool Found = false, IsConst = false;
            for (auto Owner = Super ? Source->parent : Source; Owner && !Found; Owner = Owner->parent)
                Owner->module->functions.foreach (
                    [&](const das::FunctionPtr& F)
                    {
                        if (F->classParent == Owner && F->name == Owner->name + "`" + Name)
                        {
                            Found = true;
                            IsConst = F->isConstClassMethod;
                        }
                    });
            if (Found && !IsConst)
                DSLFail(TEXT("Const method cannot call a non-const self/Super method: ")+ DSLText(Name));
        }
        virtual void preVisit(das::ExprInvoke* E) override
        {
            if (!E->isInvokeMethod || E->arguments.empty() || !E->arguments[0]->rtti_isField())
                return;
            auto* F = static_cast<das::ExprField*>(E->arguments[0]);
            if (!F->value->rtti_isVar())
                return;
            const auto Name = static_cast<das::ExprVar*>(F->value)->name;
            if (Name == Self || Name == "Super")
                Method(F->name, Name == "Super");
        }
        virtual void preVisit(das::ExprCall* E) override
        {
            const auto Name = E->name.substr(E->name.find_last_of(':') + 1);
            if ((Name == "push" || Name == "push_clone" || Name == "insert" || Name == "insert_clone" ||
                 Name == "erase" || Name == "clear" || Name == "pop" || Name == "resize") &&
                !E->arguments.empty())
                Write(E->arguments[0]);
            if (Name.find("ue_set_self_") == 0)
                DSLFail(TEXT("Const method cannot use self property setters"));
        }
    } Check(Function->arguments[0]->name, Source);
    Function->body->visit(Check);
}
