FDasDeclaredType DSLDelegateDeclaredType(const das::TypeDeclPtr& T, const FString& Folder, const FString& Registry)
{
    FDasDeclaredType D;
    D.Kind = DSLTypeName(T);
    if (!T)
        DSLFail(TEXT("Missing delegate signature type"));
    if (DSLTypedClass(T))
        D.TypePath = DSLTypedClassPath(T);
    else if (DSLTypedReference(T))
        D.TypePath = DSLTypedReferencePath(T);
    else if (D.Kind == TEXT("object"))
        D.TypePath = TEXT("/Script/CoreUObject.Object");
    else if (DSLContainer(T))
        D = DSLContainerType(T, Folder, Registry);
    else if (DSLDirectEnum(T) || DSLDirectStruct(T))
        D.TypePath = DSLDirectPath(T, Folder, Registry);
    else if (!DSLMathPath(T).IsEmpty())
        D.TypePath = DSLMathPath(T);
    else if (D.Kind == TEXT("struct") || D.Kind == TEXT("enum"))
        DSLFail(TEXT("Delegate snapshot types require a concrete authored/native type"));
    return D;
}
bool DSLDelegate(const das::TypeDeclPtr& T)
{
    return T && T->baseType == das::Type::tStructure && T->structType &&
           (DSLAnnotation(T->structType->annotations, "ue_delegate") ||
            DSLAnnotation(T->structType->annotations, "ue_multicast"));
}
const das::AnnotationDeclaration* DSLDelegateAnnotation(const das::TypeDeclPtr& T)
{
    auto* A = DSLAnnotation(T->structType->annotations, "ue_delegate");
    return A ? A : DSLAnnotation(T->structType->annotations, "ue_multicast");
}
das::ExpressionPtr DSLDelegateView(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr Source,
                                   const FString& Name)
{
    auto* V = new das::ExprMakeStruct(At);
    V->useInitializer = true;
    V->alwaysUseInitializer = true;
    V->makeType = DSLCopyType(T);
    V->makeType->ref = false;
    V->makeType->constant = false;
    const auto* A = DSLDelegateAnnotation(T);
    auto* Fields = new das::MakeStruct();
    Fields->push_back(new das::MakeFieldDecl(
        At, "__delegate_token",
        DSLCall(At, "__ue_delegate_event_view",
                {new das::ExprConstString(At, DSLUtf8(DSLString(A->arguments, "_ue_delegate_wire"))), Source,
                 new das::ExprConstString(At, DSLUtf8(Name))}),
        false, false));
    V->structs.push_back(Fields);
    return V;
}
das::ExpressionPtr DSLDelegateSnapshot(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr Token)
{
    auto* V = new das::ExprMakeStruct(At);
    V->makeType = DSLCopyType(T);
    V->makeType->ref = false;
    V->makeType->constant = false;
    V->useInitializer = true;
    V->alwaysUseInitializer = true;
    auto* Fields = new das::MakeStruct();
    Fields->push_back(new das::MakeFieldDecl(At, "__delegate_token", Token, false, false));
    V->structs.push_back(Fields);
    return V;
}
das::ExpressionPtr DSLDelegateFieldRead(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr Source,
                                        const FString& Name)
{
    const auto* A = DSLDelegateAnnotation(T);
    return DSLDelegateSnapshot(
        At, T,
        DSLCall(At, "__ue_delegate_get_field",
                {new das::ExprConstString(At, DSLUtf8(DSLString(A->arguments, "_ue_delegate_wire"))), Source,
                 new das::ExprConstString(At, DSLUtf8(Name))}));
}
das::ExpressionPtr DSLDelegateFieldWrite(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr Source,
                                         const FString& Name, das::ExpressionPtr Value)
{
    const auto* A = DSLDelegateAnnotation(T);
    return DSLCall(At, "__ue_delegate_set_field",
                   {new das::ExprConstString(At, DSLUtf8(DSLString(A->arguments, "_ue_delegate_wire"))), Source,
                    new das::ExprConstString(At, DSLUtf8(Name)), new das::ExprField(At, Value, "__delegate_token")});
}
FDasDeclaredType DSLDelegateMethodType(const das::TypeDeclPtr& T)
{
    if (!DSLDelegate(T) || !DSLAnnotation(T->structType->annotations, "ue_delegate"))
        DSLFail(TEXT("Reflected delegate values require a single-cast signature"));
    FDasDeclaredType D;
    D.Kind = TEXT("delegate");
    D.DelegateWire = DSLString(DSLDelegateAnnotation(T)->arguments, "_ue_delegate_wire");
    D.TypePath = TEXT("/DasDelegates/")+ FMD5::HashAnsiString(*D.DelegateWire) + TEXT(".Type");
    return D;
}
struct FUEDelegateAnnotation : das::StructureAnnotation
{
    virtual bool look(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList&,
                      das::string& Error) override
    {
        try
        {
            DSLDeclarationKind(S->annotations);
            DSLAnnotation(S->annotations, name.c_str());
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
    bool Multi;
    explicit FUEDelegateAnnotation(bool InMulti)
        : StructureAnnotation(InMulti ? "ue_multicast" : "ue_delegate"), Multi(InMulti)
    {
    }
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (Args.find("_ue_delegate_wire", das::Type::tString))
                return true;
            DSLAnnotationArguments(Args);
            for (const auto& A : Args)
                if (A.name != "type_folder" && A.name != "type_registry")
                    DSLFail(TEXT("Unknown delegate declaration option"));
            if (S->isClass || S->parent || S->fields.size() != 1 || S->fields[0].name != "signature")
                DSLFail(TEXT("Delegate requires a struct with one signature function type or empty signature method"));
            das::TypeDeclPtr Signature;
            das::FunctionPtr Stub = nullptr;
            TArray<FString> Directions;
            if (S->fields[0].classMethod)
            {
                S->module->functions.foreach (
                    [&](const das::FunctionPtr& F)
                    {
                        if (F->classParent == S && F->name == S->name + "`signature")
                        {
                            if (Stub)
                                DSLFail(TEXT("Delegate signature overloads are unsupported"));
                            Stub = F;
                        }
                    });
                if (!Stub || Stub->isStaticClassMethod || !Stub->annotations.empty() || Stub->arguments.empty() ||
                    !Stub->result || !Stub->body)
                    DSLFail(TEXT("Delegate signature method must be an empty instance declaration without method annotations"));
                das::ExpressionPtr Body = Stub->body;
                if (Body->rtti_isBlock())
                {
                    auto* Outer = static_cast<das::ExprBlock*>(Body);
                    if (Outer->generated && Outer->list.size() == 1 && Outer->finalList.empty() &&
                        Outer->list[0]->rtti_isWith())
                    {
                        auto* With = static_cast<das::ExprWith*>(Outer->list[0]);
                        if (With->generated)
                            Body = With->body;
                    }
                }
                if (!Body || !Body->rtti_isBlock() || !static_cast<das::ExprBlock*>(Body)->list.empty() ||
                    !static_cast<das::ExprBlock*>(Body)->finalList.empty() ||
                    !static_cast<das::ExprBlock*>(Body)->arguments.empty())
                    DSLFail(TEXT("Delegate signature body must be empty"));
                Signature = new das::TypeDecl(das::Type::tFunction, S->at);
                Signature->firstType = DSLCopyType(Stub->result);
                for (size_t I = 1; I < Stub->arguments.size(); ++I)
                {
                    const auto& A = Stub->arguments[I];
                    if (A->init)
                        DSLFail(TEXT("Delegate signature defaults are unsupported"));
                    Directions.Add(DSLParameterDirection(A, FString()));
                    Signature->argNames.push_back(A->name);
                    Signature->argTypes.push_back(DSLCopyType(A->type));
                }
            }
            else
            {
                if (S->fields[0].init || !S->fields[0].type || S->fields[0].type->baseType != das::Type::tFunction)
                    DSLFail(TEXT("Expected signature:function<(args):result> or empty def signature(...)"));
                Signature = DSLCopyType(S->fields[0].type);
                for (const auto& T : Signature->argTypes)
                {
                    if (T->ref)
                        DSLFail(TEXT("Delegate ref/out requires an empty signature method so parameter annotations are retained"));
                    Directions.Add(TEXT("in"));
                }
            }
            Signature->firstType = DSLObjectNormalize(Signature->firstType, Group, S->module);
            for (auto& T : Signature->argTypes)
                T = DSLObjectNormalize(T, Group, S->module);
            if (Signature->firstType->ref)
                DSLFail(TEXT("Delegate results must be by value"));
            if (Signature->argTypes.size() > 16 || Signature->argNames.size() != Signature->argTypes.size())
                DSLFail(TEXT("Delegate requires named inputs, up to 16"));
            FDasDeclaredMethod Method;
            Method.Name = TEXT("Invoke");
            Method.Id = TEXT("delegate.contract.Invoke");
            Method.ScriptFile = TEXT("delegate.das");
            Method.EntryPoint = TEXT("invoke");
            const FString Folder = DSLString(Args, "type_folder"), Registry = DSLString(Args, "type_registry");
            auto Declared = [&](const das::TypeDeclPtr& T)
            {
                return DSLDelegateDeclaredType(T, Folder, Registry);
            };
            Method.ReturnType = Declared(Signature->firstType);
            if (Multi && Method.ReturnType.Kind != TEXT("void"))
                DSLFail(TEXT("Multicast events cannot return a value"));
            for (size_t I = 0; I < Signature->argTypes.size(); ++I)
            {
                FDasDeclaredParameter P;
                P.Name = DSLText(Signature->argNames[I]);
                P.Id = Method.Id + TEXT(".")+ P.Name;
                P.Direction = Directions[int32(I)];
                auto ValueType = DSLCopyType(Signature->argTypes[I]);
                ValueType->ref = false;
                ValueType->constant = false;
                P.Type = Declared(ValueType);
                Method.Parameters.Add(P);
            }
            FDasAuthoringModule Model;
            Model.Id = TEXT("delegate");
            FDasDeclaredClass C;
            C.Id = TEXT("delegate.contract");
            C.Name = TEXT("Contract");
            C.NativeParent = TEXT("/Script/Engine.Actor");
            C.Methods.Add(Method);
            Model.Classes.Add(C);
            FString Wire, Issue;
            if (!Model.ToJson(Wire, Issue))
                DSLFail(Issue);
            // Check native signature construction now, so unsupported shapes fail at compile.
            TStrongObjectPtr<UFunction> Checked(
                MakeDasDelegateSignature(Method, Multi, GetTransientPackage(), NAME_None, Issue));
            if (!Checked)
                DSLFail(Issue);
            if (Stub)
            {
                auto* Trap = new das::ExprBlock();
                Trap->at = Stub->at;
                Trap->generated = true;
                Trap->list.push_back(DSLCall(
                    Stub->at, "panic",
                    {new das::ExprConstString(Stub->at, "Delegate signature is a declaration and cannot be called")}));
                if (Method.ReturnType.Kind != TEXT("void"))
                {
                    auto* Default = new das::ExprMakeStruct(Stub->at);
                    Default->makeType = DSLCopyType(Signature->firstType);
                    Default->alwaysUseInitializer = true;
                    Trap->list.push_back(new das::ExprReturn(Stub->at, Default));
                }
                Stub->body = Trap;
                Stub->classParent = nullptr;
                Stub->isClassMethod = false;
                Stub->isConstClassMethod = false;
                Stub->generated = true;
                Stub->privateFunction = true;
            }
            S->fields.clear();
            das::Structure::FieldDeclaration Token;
            Token.flags = 0;
            Token.name = "__delegate_token";
            Token.type = new das::TypeDecl(das::Type::tUInt64, S->at);
            Token.at = S->at;
            if (Multi)
                Token.init = DSLCall(S->at, "__ue_delegate_create", {new das::ExprConstString(S->at, DSLUtf8(Wire))});
            S->fields.push_back(Token);
            Args.push_back(das::AnnotationArgument("_ue_delegate_wire", DSLUtf8(Wire)));
            auto Receiver = new das::TypeDecl(das::Type::tStructure, S->at);
            Receiver->structType = S;
            auto Str = [&](const FString& V)
            {
                return new das::ExprConstString(S->at, DSLUtf8(V));
            };
            auto Bool = [&](bool V)
            {
                return new das::ExprConstBool(S->at, V);
            };
            auto TokenExpr = [&]()
            {
                return new das::ExprField(S->at, new das::ExprVar(S->at, "receiver"), "__delegate_token");
            };
            das::vector<das::FunctionPtr> Helpers;
            TSet<FString> Paths;
            auto Ensure = [&](const das::TypeDeclPtr& T, const FDasDeclaredType& D)
            {
                if (DSLContainer(T) || DSLDirectStruct(T))
                    DSLEnsureStructHelpers(T, DSLContainer(T) ? DasContainerCarrier(D)->GetPathName() : D.TypePath,
                                           Group, S->module, Helpers, Paths);
            };
            Ensure(Signature->firstType, Method.ReturnType);
            for (size_t I = 0; I < Signature->argTypes.size(); ++I)
                Ensure(Signature->argTypes[I], Method.Parameters[int32(I)].Type);
            auto Function = [&](const das::string& Name, const das::TypeDeclPtr& Result, bool Mutable)
            {
                auto* F = new das::Function();
                F->name = Name;
                F->at = S->at;
                F->generated = true;
                F->privateFunction = false;
                F->result = DSLCopyType(Result);
                auto* R = new das::Variable();
                R->name = "receiver";
                R->at = S->at;
                R->type = DSLCopyType(Receiver);
                R->type->ref = Mutable;
                R->type->constant = !Mutable;
                F->arguments.push_back(R);
                auto* B = new das::ExprBlock();
                B->at = S->at;
                F->body = B;
                return F;
            };
            auto Add = [&](das::Function* F, const das::string& N, const das::TypeDeclPtr& T)
            {
                auto* A = new das::Variable();
                A->name = N;
                A->at = S->at;
                A->type = DSLCopyType(T);
                A->type->ref = false;
                A->type->constant = true;
                F->arguments.push_back(A);
            };
            auto Publish = [&](das::Function* F)
            {
                if (!S->module->addFunction(F, true))
                    DSLFail(TEXT("Delegate method overload collision"));
            };
            auto Void = new das::TypeDecl(das::Type::tVoid, S->at);
            auto String = new das::TypeDecl(das::Type::tString, S->at);
            for (const auto* Name : Multi ? std::initializer_list<const char*>{"AddUFunction", "RemoveUFunction"}
                                          : std::initializer_list<const char*>{"BindUFunction"})
            {
                auto* F = Function(Name, Void, !Multi);
                Add(F, "target", DSLHandleType(Group, S->module, "UObjectHandle"));
                Add(F, "method", String);
                auto* B = static_cast<das::ExprBlock*>(F->body);
                auto* Op = DSLCall(
                    S->at, Name == das::string("RemoveUFunction") ? "__ue_delegate_remove" : "__ue_delegate_bind",
                    {TokenExpr(), Str(Wire), Bool(Multi), new das::ExprVar(S->at, "target"),
                     new das::ExprVar(S->at, "method")});
                B->list.push_back(Multi ? static_cast<das::ExpressionPtr>(Op)
                                        : static_cast<das::ExpressionPtr>(new das::ExprCopy(S->at, TokenExpr(), Op)));
                Publish(F);
            }
            for (const auto* Name :
                 Multi ? std::initializer_list<const char*>{"__ue_bind_script_value", "__ue_remove_script_value"}
                       : std::initializer_list<const char*>{"__ue_bind_script_value"})
            {
                auto* F = Function(Name, Void, !Multi);
                Add(F, "target", DSLHandleType(Group, S->module, "UObjectHandle"));
                Add(F, "entry", String);
                auto* Op =
                    DSLCall(S->at, "__ue_delegate_bind_script",
                            {TokenExpr(), Str(Wire), Bool(Multi), new das::ExprVar(S->at, "target"),
                             new das::ExprVar(S->at, "entry"), Bool(Name == das::string("__ue_remove_script_value"))});
                static_cast<das::ExprBlock*>(F->body)->list.push_back(
                    Multi ? static_cast<das::ExpressionPtr>(Op)
                          : static_cast<das::ExpressionPtr>(new das::ExprCopy(S->at, TokenExpr(), Op)));
                Publish(F);
            }
            auto* Bound = Function("IsBound", new das::TypeDecl(das::Type::tBool, S->at), false);
            static_cast<das::ExprBlock*>(Bound->body)
                ->list.push_back(new das::ExprReturn(
                    S->at, DSLCall(S->at, "__ue_delegate_is_bound", {TokenExpr(), Str(Wire), Bool(Multi)})));
            Publish(Bound);
            auto* Clear = Function(Multi ? "Clear" : "Unbind", Void, !Multi);
            auto* ClearOp = DSLCall(S->at, "__ue_delegate_clear", {TokenExpr(), Str(Wire), Bool(Multi)});
            static_cast<das::ExprBlock*>(Clear->body)
                ->list.push_back(Multi
                                     ? static_cast<das::ExpressionPtr>(ClearOp)
                                     : static_cast<das::ExpressionPtr>(new das::ExprCopy(S->at, TokenExpr(), ClearOp)));
            Publish(Clear);
            for (const auto* Name : Multi ? std::initializer_list<const char*>{"Broadcast"}
                                          : std::initializer_list<const char*>{"Execute", "ExecuteIfBound"})
            {
                const bool Optional = Name == das::string("ExecuteIfBound");
                if (Optional && Method.ReturnType.Kind != TEXT("void"))
                    continue;
                auto* F = Function(Name, Signature->firstType, false);
                auto* B = static_cast<das::ExprBlock*>(F->body);
                auto Local = [&](const das::string& N, const das::TypeDeclPtr& T, das::ExpressionPtr Init)
                {
                    auto* V = new das::Variable();
                    V->name = N;
                    V->at = S->at;
                    V->type = DSLCopyType(T);
                    V->type->ref = false;
                    V->type->constant = false;
                    V->init = Init;
                    V->init_via_move = DSLContainer(T) || DSLDirectStruct(T);
                    auto* L = new das::ExprLet();
                    L->at = S->at;
                    L->variables.push_back(V);
                    B->list.push_back(L);
                };
                if (Optional)
                {
                    auto* Return = new das::ExprReturn(S->at, nullptr);
                    auto* NoCall = new das::ExprBlock();
                    NoCall->at = S->at;
                    NoCall->list.push_back(Return);
                    B->list.push_back(
                        new das::ExprIfThenElse(S->at,
                                                new das::ExprOp1(S->at, "!",
                                                                 DSLCall(S->at, "__ue_delegate_is_bound",
                                                                         {TokenExpr(), Str(Wire), Bool(Multi)})),
                                                NoCall, nullptr));
                }
                Local(
                    "call", DSLHandleType(Group, S->module, "UCallHandle"),
                    DSLCall(S->at, "__ue_delegate_begin_call", {TokenExpr(), Str(Wire), Bool(Multi), Bool(Optional)}));
                auto Handle = [&]()
                {
                    return new das::ExprVar(S->at, "call");
                };
                for (size_t I = 0; I < Signature->argTypes.size(); ++I)
                {
                    const auto& T = Signature->argTypes[I];
                    const auto& D = Method.Parameters[int32(I)];
                    const auto N = "arg_" + das::to_string(I);
                    Add(F, N, T);
                    auto& Argument = F->arguments.back();
                    Argument->type->ref = D.Direction != TEXT("in");
                    Argument->type->constant = D.Direction == TEXT("in");
                    if (D.Direction == TEXT("out"))
                        continue;
                    const das::string Kind = DSLMethodAdapterKind(D.Type.Kind, T);
                    const das::string Prefix =
                        (Kind == "text" || Kind == "name" || Kind == "enum" || Kind == "struct" || Kind == "vector" ||
                         Kind == "rotator" || Kind == "transform")
                            ? "__ue_method_arg_"
                            : "ue_arg_";
                    auto* V = new das::ExprVar(S->at, N);
                    B->list.push_back(
                        DSLCall(S->at, Prefix + Kind,
                                {Handle(), Str(D.Name),
                                 T->baseType == das::Type::tUInt8 || DSLTypedClass(T) || DSLTypedReference(T) ||
                                     DSLDirectEnum(T) || DSLDirectStruct(T) || DSLContainer(T) ||
                                     DSLMathPath(T) == TEXT("/Script/CoreUObject.Vector2D")
                                                            ? DSLDirectPack(S->at, T, V, DSLMethodValuePath(D.Type))
                                                            : static_cast<das::ExpressionPtr>(V)}));
                }
                B->list.push_back(DSLCall(S->at, "ue_invoke_call", {Handle()}));
                B->finalList.push_back(DSLCall(S->at, "__ue_release_authored_call", {Handle()}));
                if (Method.ReturnType.Kind != TEXT("void"))
                {
                    const auto& T = Signature->firstType;
                    const auto Kind = DSLMethodAdapterKind(Method.ReturnType.Kind, T);
                    const das::string Prefix =
                        (Kind == "text" || Kind == "name" || Kind == "enum" || Kind == "struct" || Kind == "vector" ||
                         Kind == "rotator" || Kind == "transform")
                            ? "__ue_method_result_"
                            : "ue_result_";
                    das::ExpressionPtr Value = DSLCall(S->at, Prefix + Kind, {Handle(), Str(TEXT("ReturnValue"))});
                    if (T->baseType == das::Type::tUInt8 || DSLTypedClass(T) || DSLTypedReference(T) ||
                        DSLDirectEnum(T) || DSLDirectStruct(T) || DSLContainer(T) ||
                        DSLMathPath(T) == TEXT("/Script/CoreUObject.Vector2D"))
                        Value = DSLDirectUnpack(S->at, T, Value, DSLMethodValuePath(Method.ReturnType));
                    Local("result", T, Value);
                }
                for (size_t I = 0; I < Signature->argTypes.size(); ++I)
                {
                    const auto& D = Method.Parameters[int32(I)];
                    if (D.Direction == TEXT("in"))
                        continue;
                    const auto& T = Signature->argTypes[I];
                    const auto Kind = DSLMethodAdapterKind(D.Type.Kind, T);
                    const das::string Prefix =
                        (Kind == "text" || Kind == "name" || Kind == "enum" || Kind == "struct" || Kind == "vector" ||
                         Kind == "rotator" || Kind == "transform")
                            ? "__ue_method_result_"
                            : "ue_result_";
                    das::ExpressionPtr Value = DSLCall(S->at, Prefix + Kind, {Handle(), Str(D.Name)});
                    if (T->baseType == das::Type::tUInt8 || DSLTypedClass(T) || DSLTypedReference(T) ||
                        DSLDirectEnum(T) || DSLDirectStruct(T) || DSLContainer(T) ||
                        DSLMathPath(T) == TEXT("/Script/CoreUObject.Vector2D"))
                        Value = DSLDirectUnpack(S->at, T, Value, DSLMethodValuePath(D.Type));
                    Local("out_" + das::to_string(I), T, Value);
                }
                for (size_t I = 0; I < Signature->argTypes.size(); ++I)
                    if (Method.Parameters[int32(I)].Direction != TEXT("in"))
                    {
                        auto* L = new das::ExprVar(S->at, "arg_" + das::to_string(I));
                        auto* R = new das::ExprVar(S->at, "out_" + das::to_string(I));
                        const auto& T = Signature->argTypes[I];
                        B->list.push_back(DSLContainer(T) || DSLDirectStruct(T)
                                              ? static_cast<das::ExpressionPtr>(new das::ExprMove(S->at, L, R))
                                              : static_cast<das::ExpressionPtr>(new das::ExprCopy(S->at, L, R)));
                    }
                if (Method.ReturnType.Kind != TEXT("void"))
                {
                    const auto& T = Signature->firstType;
                    auto* R = new das::ExprReturn(S->at, new das::ExprVar(S->at, "result"));
                    R->moveSemantics = DSLContainer(T) || DSLDirectStruct(T);
                    B->list.push_back(R);
                }
                Publish(F);
            }
            if (Multi)
            {
                auto* F = new das::Function();
                F->name = S->name + "_from_property";
                F->at = S->at;
                F->generated = true;
                F->result = DSLCopyType(Receiver);
                Add(F, "source", DSLHandleType(Group, S->module, "UObjectHandle"));
                Add(F, "property", String);
                auto* B = new das::ExprBlock();
                B->at = S->at;
                F->body = B;
                auto* Make = new das::ExprMakeStruct(S->at);
                Make->useInitializer = true;
                Make->alwaysUseInitializer = true;
                Make->makeType = DSLCopyType(Receiver);
                auto* E = new das::MakeFieldDecl(
                    S->at, "__delegate_token",
                    DSLCall(S->at, "__ue_delegate_event_view",
                            {Str(Wire), new das::ExprVar(S->at, "source"), new das::ExprVar(S->at, "property")}),
                    false, false);
                auto* Fields = new das::MakeStruct();
                Fields->push_back(E);
                Make->structs.push_back(Fields);
                B->list.push_back(new das::ExprReturn(S->at, Make));
                Publish(F);
            }
            for (const auto& H : Helpers)
            {
                H->privateFunction = true;
                if (!S->module->addFunction(H, true))
                    DSLFail(TEXT("Delegate value helper collision"));
            }
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};

das::ExpressionPtr DSLDelegateReferenceExpansion(das::Program*, das::Module*, das::ExprCallMacro*);
struct FUEDelegateReferenceMacro : das::CallMacro
{
    FUEDelegateReferenceMacro() : CallMacro("__ue_delegate_reference") {}
    virtual bool canVisitArguments(das::ExprCallMacro* E, int I) override
    {
        return I != 3 && (I != 0 || !E->arguments[0]->rtti_isTypeDecl());
    }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            return DSLDelegateReferenceExpansion(P, M, E);
        }
        catch (const std::exception& X)
        {
            P->error("Delegate handler reference failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
das::ExprCallMacro* DSLHandlerCall(const das::LineInfo& At, const das::vector<das::ExpressionPtr>& Args)
{
    auto* Foundation = das::Module::require("ue_foundation");
    auto Factory = Foundation->callThis.find("__ue_delegate_reference");
    if (Factory == Foundation->callThis.end())
        DSLFail(TEXT("Missing handler reference macro"));
    auto* E = static_cast<das::ExprCallMacro*>(Factory->second(At));
    E->arguments = Args;
    return E;
}
struct FUEBoundDelegateFactory : das::CallMacro
{
    FUEBoundDelegateFactory() : CallMacro("ue_bind") {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int Index) override { return Index != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (E->arguments.size() != 3 || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_bind(type<Delegate>, target, method)"));
            if (E->arguments[2]->rtti_isAddr())
                return DSLHandlerCall(E->at, {E->arguments[0], E->arguments[1], E->arguments[2],
                                              new das::ExprTypeDecl(E->at, new das::TypeDecl(das::Type::tVoid, E->at)),
                                              new das::ExprConstInt(E->at, 0)});
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (!DSLDelegate(T) || !DSLAnnotation(T->structType->annotations, "ue_delegate"))
                DSLFail(TEXT("ue_bind requires a single delegate type"));
            const auto* A = DSLDelegateAnnotation(T);
            auto* V = new das::ExprMakeStruct(E->at);
            V->makeType = DSLCopyType(T);
            V->useInitializer = true;
            V->alwaysUseInitializer = true;
            auto* Fields = new das::MakeStruct();
            Fields->push_back(new das::MakeFieldDecl(
                E->at, "__delegate_token",
                DSLCall(E->at, "__ue_delegate_bind",
                        {new das::ExprConstUInt64(E->at, 0),
                         new das::ExprConstString(E->at, DSLUtf8(DSLString(A->arguments, "_ue_delegate_wire"))),
                         new das::ExprConstBool(E->at, false), DSLCall(E->at, "ue_delegate_target", {E->arguments[1]}),
                         DSLCall(E->at, "ue_delegate_name", {E->arguments[2]})}),
                false, false));
            V->structs.push_back(Fields);
            return V;
        }
        catch (const std::exception& X)
        {
            P->error("Bound delegate construction failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
