// Capture layout contracts before method lowering erases authored field origins.
void DSLCollectWidgetLayoutUses(const das::StructurePtr& St, das::ModuleGroup& Group, FDasDeclaredClass& Decl)
{
    struct FOrigin
    {
        FString ClassField, KnownClass;
        bool operator==(const FOrigin& O) const { return ClassField == O.ClassField && KnownClass == O.KnownClass; }
    };
    struct FOrigins : das::Visitor
    {
        das::string Self;
        FString Scope;
        TMap<FString, FOrigin>& Origins;
        FOrigins(const das::string& S, const FString& N, TMap<FString, FOrigin>& O) : Self(S), Scope(N), Origins(O) {}
        FString Field(das::ExpressionPtr E)
        {
            if (!E || !E->rtti_isField())
                return {};
            auto* F = static_cast<das::ExprField*>(E);
            return F->value && F->value->rtti_isVar() && static_cast<das::ExprVar*>(F->value)->name == Self
                       ? DSLText(F->name)
                       : FString();
        }
        static das::ExprLooksLikeCall* Call(das::ExpressionPtr E)
        {
            return E && (E->rtti_isCall() || E->__rtti == das::string("ExprCallMacro"))
                       ? static_cast<das::ExprLooksLikeCall*>(E)
                       : nullptr;
        }
        static das::string Name(das::ExprLooksLikeCall* E)
        {
            auto I = E->name.rfind("::");
            return I == das::string::npos ? E->name : E->name.substr(I + 2);
        }
        void Write(const FString& Target, das::ExpressionPtr Value)
        {
            auto* C = Call(Value);
            if (Target.IsEmpty() || !C || Name(C) != "ue_widget_create" || C->arguments.size() != 2)
                return;
            FOrigin Origin;
            Origin.ClassField = Field(C->arguments[0]);
            if (auto* Kind = Call(C->arguments[0]))
            {
                const auto N = Name(Kind);
                if ((N == "ue_class_loaded" || N == "ue_class_default") && !Kind->arguments.empty() &&
                    Kind->arguments.back()->__rtti == das::string("ExprConstString"))
                    Origin.KnownClass = DSLText(static_cast<das::ExprConstString*>(Kind->arguments.back())->text);
            }
            if (auto* Old = Origins.Find(Target); Old && !(*Old == Origin))
                Origins.Add(Target, FOrigin{});
            else
                Origins.Add(Target, Origin);
        }
        virtual void preVisit(das::ExprCopy* E) override
        {
            const auto N = Field(E->left);
            if (!N.IsEmpty())
                Write(TEXT("self.")+ N, E->right);
        }
        virtual void preVisit(das::ExprMove* E) override
        {
            const auto N = Field(E->left);
            if (!N.IsEmpty())
                Write(TEXT("self.")+ N, E->right);
        }
        virtual void preVisit(das::ExprLet* E) override
        {
            for (const auto& V : E->variables)
                Write(Scope + TEXT(":")+ DSLText(V->name), V->init);
        }
    };
    TMap<FString, FOrigin> Origins;
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St && F->body && !F->arguments.empty())
            {
                FOrigins V(F->arguments[0]->name, DSLText(F->name), Origins);
                F->body->visit(V);
            }
        });
    struct FUses : das::Visitor
    {
        das::string Self;
        FString Scope;
        das::ModuleGroup& Group;
        das::Module* Module;
        TMap<FString, FOrigin>& Origins;
        TArray<FDasWidgetLayoutUse>& Uses;
        FUses(const das::string& S, const FString& N, das::ModuleGroup& G, das::Module* M, TMap<FString, FOrigin>& O,
              TArray<FDasWidgetLayoutUse>& U)
            : Self(S), Scope(N), Group(G), Module(M), Origins(O), Uses(U)
        {
        }
        virtual void preVisit(das::ExprCallMacro* E) override
        {
            if (E->name != "ue_widget_view" && E->name != "ue_foundation::ue_widget_view")
                return;
            if (E->arguments.size() != 2 || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_widget_view(type<Layout>, widget)"));
            auto L = DSLWidgetLayout(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, Group, Module);
            FString Key;
            auto Root = E->arguments[1];
            if (Root && Root->rtti_isField())
            {
                auto* F = static_cast<das::ExprField*>(Root);
                if (F->value && F->value->rtti_isVar() && static_cast<das::ExprVar*>(F->value)->name == Self)
                {
                    L.WidgetField = DSLText(F->name);
                    Key = TEXT("self.")+ L.WidgetField;
                }
            }
            else if (Root && Root->rtti_isVar())
                Key = Scope + TEXT(":")+ DSLText(static_cast<das::ExprVar*>(Root)->name);
            if (auto* Origin = Origins.Find(Key))
            {
                L.ClassField = Origin->ClassField;
                L.KnownClassPath = Origin->KnownClass;
            }
            Uses.AddUnique(L);
        }
    };
    Decl.WidgetLayouts.Empty();
    St->module->functions.foreach (
        [&](const das::FunctionPtr& F)
        {
            if (F->classParent == St && F->body && !F->arguments.empty())
            {
                FUses V(F->arguments[0]->name, DSLText(F->name), Group, St->module, Origins, Decl.WidgetLayouts);
                F->body->visit(V);
            }
        });
}
