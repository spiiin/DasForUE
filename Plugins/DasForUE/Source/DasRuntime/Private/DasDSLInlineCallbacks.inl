// Normalize managed callback blocks before type inference, reusing ordinary ue_function lowering.
struct FDSLCallbackScopes : das::Visitor
{
    das::vector<TSet<FString>> Scopes;
    FDSLCallbackScopes() { Scopes.emplace_back(); }
    bool Declared(const das::string& Name) const
    {
        const auto N = DSLText(Name);
        for (const auto& Scope : Scopes)
            if (Scope.Contains(N))
                return true;
        return false;
    }
    TSet<FString> Visible() const
    {
        TSet<FString> Names;
        for (const auto& Scope : Scopes)
            Names.Append(Scope);
        return Names;
    }
    virtual void preVisit(das::ExprBlock* E) override
    {
        Scopes.emplace_back();
        for (const auto& A : E->arguments)
            Scopes.back().Add(DSLText(A->name));
    }
    virtual das::ExpressionPtr visit(das::ExprBlock* E) override
    {
        Scopes.pop_back();
        return E;
    }
    virtual das::VariablePtr visitLet(das::ExprLet*, const das::VariablePtr& V, bool) override
    {
        Scopes.back().Add(DSLText(V->name));
        return V;
    }
    virtual void preVisitForBody(das::ExprFor* E, das::Expression*) override
    {
        Scopes.emplace_back();
        for (const auto& N : E->iterators)
            Scopes.back().Add(DSLText(N));
    }
    virtual das::ExpressionPtr visit(das::ExprFor* E) override
    {
        Scopes.pop_back();
        return E;
    }
};
struct FDSLCallbackCaptureCheck : FDSLCallbackScopes
{
    const TSet<FString>& Outer;
    das::string Self;
    FDSLCallbackCaptureCheck(const TSet<FString>& O, const das::string& S) : Outer(O), Self(S) {}
    virtual void preVisit(das::ExprVar* E) override
    {
        if (E->name != Self && Outer.Contains(DSLText(E->name)) && !Declared(E->name))
            DSLFail(TEXT("Inline UE callback cannot capture local variable '")+ DSLText(E->name) +
                         TEXT("'; use timer payload (используй timer payload) or a self property"));
    }
};
void DSLNormalizeInlineCallbacks(const das::StructurePtr& St, das::ModuleGroup& Group)
{
    das::vector<das::FunctionPtr> Sources;
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St && F->body && !F->arguments.empty())
            {
                if (!F->generated && DSLMethodName(F).find("__ue_cb_") == 0)
                    DSLFail(TEXT("__ue_cb_ is reserved for generated inline callbacks"));
                Sources.push_back(F);
            }
        });
    for (size_t SourceIndex = 0; SourceIndex < Sources.size(); ++SourceIndex)
    {
        auto F = Sources[SourceIndex];
        struct FLift : FDSLCallbackScopes
        {
            const das::StructurePtr& St;
            const das::FunctionPtr& Owner;
            das::ModuleGroup& Group;
            das::vector<das::FunctionPtr>& Pending;
            int32 Position = 0;
            FLift(const das::StructurePtr& S, const das::FunctionPtr& F, das::ModuleGroup& G,
                  das::vector<das::FunctionPtr>& P)
                : St(S), Owner(F), Group(G), Pending(P)
            {
                for (const auto& A : F->arguments)
                    Scopes.back().Add(DSLText(A->name));
            }
            virtual void preVisit(das::ExprCall* E) override
            {
                const int32 Index = DSLCallbackArgument(E->name, E->arguments.size());
                if (Index < 0)
                    return;
                const int32 Ordinal = Position++;
                auto Arg = E->arguments[Index];
                if (!Arg || !Arg->rtti_isMakeBlock())
                    return;
                if (Owner->isStaticClassMethod)
                    DSLFail(TEXT("Inline UE callbacks require an instance method"));
                auto* Make = static_cast<das::ExprMakeBlock*>(Arg);
                if (!Make->capture.empty() || Make->isLambda || Make->isLocalFunction || !Make->block ||
                    !Make->block->rtti_isBlock())
                    DSLFail(TEXT("Use a $ block for an inline UE callback; explicit captures are forbidden; use timer payload"));
                auto* Block = static_cast<das::ExprBlock*>(Make->block);
                const auto Outer = Visible();
                FDSLCallbackCaptureCheck Capture(Outer, Owner->arguments[0]->name);
                Block->visit(Capture);
                das::TextWriter Printed;
                Printed << *Block;
                const FString Key =
                    DSLText(St->name) + TEXT("|")+ DSLText(DSLMethodName(Owner)) +
                                             TEXT("|")+ FString::FromInt(Ordinal) + TEXT("|")+ DSLText(Printed.str());
                const FTCHARToUTF8 KeyBytes(*Key);
                const das::string Name =
                    "__ue_cb_" +
                    DSLUtf8(FMD5::HashBytes(reinterpret_cast<const uint8*>(KeyBytes.Get()), KeyBytes.Length()));
                if (St->findField(Name))
                    DSLFail(TEXT("Inline callback generated name collision"));
                das::FunctionPtr Fn = new das::Function();
                Fn->name = St->name + "`" + Name;
                Fn->at = Arg->at;
                Fn->generated = true;
                Fn->privateFunction = false;
                if (Block->returnType && Block->returnType->baseType != das::Type::autoinfer &&
                    Block->returnType->baseType != das::Type::tVoid)
                    DSLFail(TEXT("Inline UE callback must return void"));
                Fn->result = new das::TypeDecl(das::Type::tVoid, Arg->at);
                for (const auto& A : Block->arguments)
                {
                    if (A->name == Owner->arguments[0]->name || !A->type || A->type->baseType == das::Type::autoinfer)
                        DSLFail(TEXT("Inline callback parameters require explicit types; self is implicit"));
                    Fn->arguments.push_back(A->clone());
                }
                auto* Body = static_cast<das::ExprBlock*>(Block->clone());
                Body->arguments.clear();
                Body->returnType = nullptr;
                Body->isClosure = false;
                Body->isLambdaBlock = false;
                Fn->body = Body;
                das::modifyToClassMember(Fn, St, false, false);
                auto Annotations = Group.findAnnotation("ue_function", St->module);
                if (Annotations.size() != 1)
                    DSLFail(TEXT("Missing ue_function annotation"));
                auto* A = new das::AnnotationDeclaration();
                A->flags = 0;
                A->annotation = Annotations[0];
                A->at = Arg->at;
                A->arguments.push_back(das::AnnotationArgument("BlueprintCallable", true));
                Fn->annotations.push_back(A);
                das::Structure::FieldDeclaration Member;
                Member.name = Name;
                Member.at = Arg->at;
                Member.classMethod = true;
                Member.type = new das::TypeDecl(das::Type::autoinfer, Arg->at);
                Member.init = new das::ExprAddr(Arg->at, Fn->name);
                St->fields.push_back(Member);
                if (!St->module->addFunction(Fn, true))
                    DSLFail(TEXT("Inline callback function collision"));
                Pending.push_back(Fn);
                E->arguments[Index] = new das::ExprConstString(Arg->at, Name);
            }
        };
        FLift Visitor(St, F, Group, Sources);
        F->body = F->body->visit(Visitor);
    }
}
