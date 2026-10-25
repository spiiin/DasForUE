// Contextual value conversions preserve the weak capability and use the same
// checked factory as explicit casts. Mutable ref/out parameters stay invariant.
struct FUEReferenceUpcastVisitor : das::Visitor
{
    das::Program* Program;
    das::Module* Module;
    das::Function* Function;
    bool Changed = false;
    FUEReferenceUpcastVisitor(das::Program* P, das::Module* M, das::Function* F) : Program(P), Module(M), Function(F) {}
    static bool Same(const das::TypeDeclPtr& A, const das::TypeDeclPtr& B)
    {
        return A && B && A->isSameType(*B, das::RefMatters::no, das::ConstMatters::no, das::TemporaryMatters::no);
    }
    static int Distance(const das::TypeDeclPtr& From, const das::TypeDeclPtr& To)
    {
        if (DSLAdaptedReference(From) || DSLAdaptedReference(To))
        {
            const int D =
                (DSLAdaptedReference(From) ? DSLAdaptedReference(From)->Script->UpcastDistance(From, To) : -1);
            return D > 0 ? D : -1;
        }
        if (!DSLTypedReference(From) || !DSLTypedReference(To) || DSLAuthoredView(To) || DSLTypedInterface(From) ||
            DSLTypedInterface(To))
            return -1;
        const FString Source = DSLTypedReferencePath(From), Target = DSLTypedReferencePath(To);
        auto* Child = FindObject<UClass>(nullptr, *Source);
        auto* Parent = FindObject<UClass>(nullptr, *Target);
        if (!Child || !Parent || Child == Parent || !Child->IsChildOf(Parent))
            return -1;
        int N = 0;
        for (auto* C = Child; C && C != Parent; C = C->GetSuperClass())
            ++N;
        return N;
    }
    bool Convert(das::ExpressionPtr& Value, const das::TypeDeclPtr& Target)
    {
        if (!Value || !Target || Distance(Value->type, Target) < 1)
            return false;
        const auto At = Value->at;
        const auto Source = Value->type;
        if (DSLAdaptedReference(Source))
        {
            Value = DSLAdaptedReference(Source)->Script->Upcast(At, Source, Target, Value);
            Changed = true;
            return true;
        }
        Value = DSLReferenceValue(At, Target,
                                  DSLAuthoredView(Source)
                                      ? DSLAuthoredPack(At, Source, Value)
                                      : DSLDirectPack(At, Source, Value, DSLTypedReferencePath(Source)));
        Changed = true;
        return true;
    }
    virtual bool canVisitMakeBlockBody(das::ExprMakeBlock*) override { return false; }
    virtual das::ExpressionPtr visit(das::ExprLet* E) override
    {
        for (const auto& V : E->variables)
            if (!V->type->ref)
                Convert(V->init, V->type);
        return E;
    }
    virtual das::ExpressionPtr visit(das::ExprCopy* E) override
    {
        const bool Soft = DSLAdaptedReference(E->right->type);
        if (Convert(E->right, E->left->type) && Soft && E->left->rtti_isCall())
        {
            // Failed type inference may have lowered the property LHS to its getter.
            // Restore the checked setter instead of writing to the getter's temporary.
            auto* Getter = static_cast<das::ExprCall*>(E->left);
            if (Getter->name.find(".`") == 0 && Getter->name.find('`', 2) == das::string::npos &&
                Getter->arguments.size() == 1 && DSLTypedReference(Getter->arguments[0]->type))
                return DSLCall(E->at, Getter->name + "`clone", {Getter->arguments[0], E->right});
        }
        return E;
    }
    virtual das::ExpressionPtr visit(das::ExprMove* E) override
    {
        Convert(E->right, E->left->type);
        return E;
    }
    virtual das::ExpressionPtr visit(das::ExprReturn* E) override
    {
        if (!E->returnInBlock && (!E->returnFunc || E->returnFunc == Function) && !Function->result->ref)
            Convert(E->subexpr, Function->result);
        return E;
    }
    virtual das::ExpressionPtr visit(das::ExprInvoke* E) override
    {
        if (!E->isInvokeMethod || E->arguments.size() < 2 || !E->arguments[0]->rtti_isField())
            return E;
        auto* Field = static_cast<das::ExprField*>(E->arguments[0]);
        das::ExpressionPtr Receiver = Field->value;
        // daScript inserts a second, implicit receiver slot. After a failed infer
        // leg Field.value is type<Receiver> and slot 1 holds cast<auto>(deref(value)).
        if (Receiver->rtti_isTypeDecl())
        {
            Receiver = E->arguments[1];
            if (Receiver->rtti_isCast())
                Receiver = static_cast<das::ExprCast*>(Receiver)->subexpr;
            if (Receiver->rtti_isPtr2Ref() && Receiver->alwaysSafe)
                Receiver = static_cast<das::ExprPtr2Ref*>(Receiver)->subexpr;
        }
        if (!DSLTypedReference(Receiver->type))
            return E;
        auto* Call = DSLCall(E->at, Field->name, {Receiver});
        for (size_t I = 2; I < E->arguments.size(); ++I)
            Call->arguments.push_back(E->arguments[I]);
        const bool Previous = Changed;
        Changed = false;
        visit(Call);
        const bool Converted = Changed;
        Changed |= Previous;
        return Converted ? static_cast<das::ExpressionPtr>(Call) : static_cast<das::ExpressionPtr>(E);
    }
    virtual das::ExpressionPtr visit(das::ExprCall* E) override
    {
        if (E->func || E->arguments.empty())
            return E;
        // Only resolve fully typed, concrete signatures; generic functions and
        // unrelated language conversions remain the responsibility of daScript.
        das::FunctionPtr Best = nullptr;
        int BestScore = MAX_int32;
        bool Ambiguous = false, HasExact = false;
        const auto Separator = E->name.rfind("::");
        const auto Name = Separator == das::string::npos ? E->name : E->name.substr(Separator + 2);
        const auto Qualifier = Separator == das::string::npos ? das::string() : E->name.substr(0, Separator);
        Program->library.foreach (
            [&](das::Module* M)
            {
                if (!Module->isVisibleDirectly(M) || (!Qualifier.empty() && M->name != Qualifier))
                    return true;
                M->functions.foreach (
                    [&](const das::FunctionPtr& F)
                    {
                        if (F->name != Name || (F->privateFunction && M != Module) ||
                            F->arguments.size() < E->arguments.size())
                            return;
                        for (size_t I = E->arguments.size(); I < F->arguments.size(); ++I)
                            if (!F->arguments[I]->init)
                                return;
                        int Score = 0;
                        bool Any = false;
                        for (size_t I = 0; I < E->arguments.size(); ++I)
                        {
                            const auto& T = F->arguments[I]->type;
                            const auto& A = E->arguments[I]->type;
                            if (Same(A, T))
                                continue;
                            const int D = Distance(A, T);
                            if (D < 1 || (T->isRef() && !T->constant))
                                return;
                            Score += D;
                            Any = true;
                        }
                        if (!Any)
                        {
                            HasExact = true;
                            return;
                        }
                        if (Score < BestScore)
                        {
                            Best = F;
                            BestScore = Score;
                            Ambiguous = false;
                        }
                        else if (Score == BestScore)
                            Ambiguous = true;
                    });
                return true;
            },
            "*");
        if (Best && !Ambiguous && !HasExact)
            for (size_t I = 0; I < E->arguments.size(); ++I)
                Convert(E->arguments[I], Best->arguments[I]->type);
        return E;
    }
};
