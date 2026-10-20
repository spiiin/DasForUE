// Compile-time names select the existing checked frame wrappers. No second marshaller.
// Generator metadata survives dependency compilation and AST optimization.
struct FUEReflectedCallAnnotation : das::FunctionAnnotation
{
    FUEReflectedCallAnnotation() : FunctionAnnotation("ue_reflected_call") {}
    virtual bool finalize(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string& Error) override
    {
        Error = "ue_reflected_call is generated function metadata";
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
    virtual bool apply(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (Args.size() != 3 || DSLString(Args, "owner").IsEmpty() || DSLString(Args, "method").IsEmpty() ||
                DSLString(Args, "signature").IsEmpty())
                DSLFail(TEXT("Expected generated ue_reflected_call(owner, method, signature) metadata"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
das::ExpressionPtr DSLReflectionCall(const das::LineInfo& At, das::Function* F,
                                     const das::vector<das::ExpressionPtr>& Args, size_t Offset)
{
    auto* Call = DSLCall(At, F->module->name.empty() ? F->name : F->module->name + "::" + F->name, {});
    for (size_t I = Offset; I < Args.size(); ++I)
        Call->arguments.push_back(Args[I]);
    return Call;
}
struct FUEReflectionMacro : das::CallMacro
{
    int Operation;
    FUEReflectionMacro(const char* N, int Op) : CallMacro(N), Operation(Op) {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int I) override
    {
        return Operation == 2 ? I == 0 : I >= (Operation == 0 ? 1 : 2);
    }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            const auto& Args = E->arguments;
            if (Operation == 2)
            {
                if (Args.size() != 2 || !Args[1]->rtti_isVar())
                    DSLFail(TEXT("Expected ue_struct_get(value, Field)"));
                const auto T = DSLObjectNormalize(Args[0]->type, *P->thisModuleGroup, M);
                const auto Name = static_cast<das::ExprVar*>(Args[1])->name;
                if (!DSLDirectStruct(T) || DSLSoft(T) || T->baseType != das::Type::tStructure || !T->structType)
                    DSLFail(TEXT("ue_struct_get requires a typed Unreal value struct; use a native/ue_struct type instead of UEStructValue"));
                const auto* Field = T->structType->findField(Name);
                if (!Field || Field->classMethod || Field->generated)
                    DSLFail(TEXT("Unknown Unreal struct field ") + DSLText(T->structType->name) +
                                 TEXT(".") + DSLText(Name));
                return new das::ExprField(E->at, Args[0], Name);
            }
            das::TypeDeclPtr T = nullptr;
            das::string Method;
            if (Operation == 0)
            {
                if (Args.empty() || !Args[0]->rtti_isField())
                    DSLFail(TEXT("Expected ue_static(Library.Function, arguments)"));
                auto* Field = static_cast<das::ExprField*>(Args[0]);
                if (!Field->value->rtti_isVar())
                    DSLFail(TEXT("ue_static requires a library type name"));
                auto* Alias = new das::TypeDecl(das::Type::alias, E->at);
                Alias->alias = static_cast<das::ExprVar*>(Field->value)->name;
                T = DSLObjectNormalize(Alias, *P->thisModuleGroup, M);
                Method = Field->name;
                if (!DSLTypedObject(T))
                    DSLFail(TEXT("ue_static requires a generated native library type; require its bindings module"));
            }
            else
            {
                if (Args.size() < 3 || !Args[0]->rtti_isTypeDecl() || !Args[1]->rtti_isVar())
                    DSLFail(TEXT("Expected ue_call(type<InterfaceOrObjectRef>, Method, target, arguments)"));
                T = DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(Args[0])->typeexpr, *P->thisModuleGroup, M);
                Method = static_cast<das::ExprVar*>(Args[1])->name;
                if (!DSLTypedReference(T))
                    DSLFail(TEXT("ue_call requires a typed Unreal object/interface reference"));
                // Reuses the normal typed member-call emitter, including authored contracts and ref/out.
                DSLEmitReferenceMethod(P, P->getThisModule(), T, Method, E->at);
                das::Function* Found = nullptr;
                P->library.foreach (
                    [&](das::Module* Mod)
                    {
                        Mod->functions.foreach (
                            [&](das::Function* F)
                            {
                                if (F->name == Method && !F->arguments.empty() &&
                                    F->arguments[0]->type->structType == T->structType)
                                {
                                    if (Found && Found != F)
                                        DSLFail(TEXT("Ambiguous ue_call wrapper"));
                                    Found = F;
                                }
                            });
                        return true;
                    },
                    "*");
                if (!Found)
                    DSLFail(TEXT("Missing typed method wrapper: ") + DSLText(Method));
                return DSLReflectionCall(E->at, Found, Args, 2);
            }
            const FString Owner = DSLTypedObjectPath(T);
            auto* Class = FindObject<UClass>(nullptr, *Owner);
            auto* Function = Class ? Class->FindFunctionByName(FName(*DSLText(Method))) : nullptr;
            if (!Function || !Function->HasAllFunctionFlags(FUNC_Static | FUNC_BlueprintCallable))
                DSLFail(TEXT("Unknown callable static function: ") + Owner + TEXT(".") + DSLText(Method));
            das::Function* Found = nullptr;
            P->library.foreach (
                [&](das::Module* Mod)
                {
                    Mod->functions.foreach (
                        [&](das::Function* F)
                        {
                            const auto* A = DSLAnnotation(F->annotations, "ue_reflected_call");
                            if (A && DSLString(A->arguments, "owner") == Owner &&
                                DSLString(A->arguments, "method") == DSLText(Method))
                            {
                                if (DSLString(A->arguments, "signature") != CallSignatureKey(Function))
                                    DSLFail(TEXT("Reflected signature changed; regenerate bindings: ") + Owner +
                                                 TEXT(".") + DSLText(Method));
                                if (Found && Found != F)
                                    DSLFail(
                                        TEXT("Ambiguous static bindings; require one API package for this function"));
                                Found = F;
                            }
                        });
                    return true;
                },
                "*");
            if (!Found)
                DSLFail(TEXT("No generated binding for ") + Owner +
                             TEXT(".") + DSLText(Method) + TEXT("; generate and require its API package"));
            return DSLReflectionCall(E->at, Found, Args, 1);
        }
        catch (const std::exception& X)
        {
            P->error("Unreal reflection macro failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
