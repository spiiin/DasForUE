// Compiler-generated tuple bridges; public syntax remains ordinary typed parameters.
struct FUECallbackFrameAnnotation : das::FunctionAnnotation
{
    FUECallbackFrameAnnotation() : FunctionAnnotation("__ue_callback_frame") {}
    virtual bool apply(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool finalize(const das::FunctionPtr&, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool apply(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
    virtual bool finalize(das::ExprBlock*, das::ModuleGroup&, das::AnnotationArgumentList&,
                          const das::AnnotationArgumentList&, das::string&) override
    {
        return false;
    }
};
FDasDeclaredType DSLCallbackField(const das::TypeDeclPtr& T, const FString& Folder, const FString& Registry)
{
    if (!T || T->ref || !FindDasCallbackType(DSLCallbackKind(T)) || DSLTypeName(T) == TEXT("void"))
        DSLFail(TEXT("Callback arguments require supported by-value types"));
    FDasDeclaredType D;
    D.Kind = DSLTypeName(T);
    if (DSLContainer(T))
    {
        D.Kind = TEXT("struct");
        D.TypePath = DSLDirectPath(T, Folder, Registry);
    }
    else if (D.GetKind() == EDasTypeKind::Struct)
    {
        if (!DSLDirectStruct(T) && DSLMathPath(T).IsEmpty())
            DSLFail(TEXT("Callback structs require registered concrete types"));
        D.TypePath = DSLDirectPath(T, Folder, Registry);
    }
    else if (D.GetKind() == EDasTypeKind::Enum)
    {
        if (!DSLTypedValue(T, EDasTypeKind::Enum))
            DSLFail(TEXT("Callback enums require concrete types"));
        D.TypePath = DSLDirectPath(T, Folder, Registry);
    }
    else if (D.GetKind() == EDasTypeKind::Class)
    {
        if (!DSLTypedClass(T))
            DSLFail(TEXT("Callback classes require UEClass<T>"));
        D.TypePath = DSLTypedClassPath(T);
    }
    else if (D.GetKind() == EDasTypeKind::Object)
    {
        if (!DSLTypedReference(T))
            DSLFail(TEXT("Callback objects require typed wrappers"));
        D.TypePath = DSLTypedReferencePath(T);
    }
    FString Error;
    if (!ValidateDasPropertyType(D, EDasPropertyUsage::Value, Error))
        DSLFail(Error);
    return D;
}
UScriptStruct* DSLCallbackTuple(const das::FunctionPtr& F, const FString& Folder, const FString& Registry)
{
    TArray<FDasDeclaredType> Fields;
    for (size_t I = 1; I < F->arguments.size(); ++I)
    {
        if (F->arguments[I]->init || DSLParameterDirection(F->arguments[I], FString()) != TEXT("in"))
            DSLFail(TEXT("Callbacks do not support defaults/ref/out"));
        Fields.Add(DSLCallbackField(F->arguments[I]->type, Folder, Registry));
    }
    FString Error;
    auto* Frame = DasCallbackFrame(Fields, Error);
    if (!Frame)
        DSLFail(Error);
    return Frame;
}
das::ExpressionPtr DSLCallbackFieldGet(const das::LineInfo& At, const das::TypeDeclPtr& T, const FString& Folder,
                                       const FString& Registry, const das::string& Frame, int32 Index)
{
    const auto D = DSLCallbackField(T, Folder, Registry);
    auto Value = [&]()
    {
        return new das::ExprVar(At, Frame);
    };
    auto Name = [&]()
    {
        return new das::ExprConstString(At, DSLUtf8(FString::Printf(TEXT("arg_%d"), Index)));
    };
    if (D.GetKind() == EDasTypeKind::Struct)
        return DSLDirectUnpack(
            At, T, DSLCall(At, "ue_struct_nested_get", {Value(), Name(), new das::ExprConstInt(At, -1)}), D.TypePath);
    if (D.GetKind() == EDasTypeKind::Enum)
        return DSLDirectUnpack(At, T,
                               DSLCall(At, "__ue_struct_get_enum_value",
                                       {Value(), Name(), new das::ExprConstString(At, DSLUtf8(D.TypePath))}),
                               D.TypePath);
    if (D.GetKind() == EDasTypeKind::Class)
        return DSLClassUnpack(At, T, DSLCall(At, "__ue_struct_get_class", {Value(), Name()}));
    if (D.GetKind() == EDasTypeKind::Object)
        return DSLDirectUnpack(At, T, DSLCall(At, "ue_struct_get_object", {Value(), Name()}), D.TypePath);
    auto* Get = DSLCall(At, das::string("ue_struct_get_") + DSLScalarSuffix(D.Kind), {Value(), Name()});
    return D.GetKind() == EDasTypeKind::Byte ? DSLDirectUnpack(At, T, Get, FString())
                                             : static_cast<das::ExpressionPtr>(Get);
}
void DSLCallbackUnpackArguments(const das::FunctionPtr& Clone, const FString& Folder, const FString& Registry,
                                das::ModuleGroup& Group, das::Module* Module)
{
    const auto Original = Clone->arguments;
    Clone->arguments.clear();
    auto* Frame = new das::Variable();
    Frame->at = Clone->at;
    Frame->name = "__ue_callback_values";
    Frame->type = DSLHandleType(Group, Module, "UEStructValue");
    Frame->type->constant = true;
    Clone->arguments.push_back(Frame);
    das::vector<das::ExpressionPtr> Locals;
    for (size_t I = 0; I < Original.size(); ++I)
    {
        auto* V = new das::Variable();
        V->at = Clone->at;
        V->name = Original[I]->name;
        V->type = DSLCopyType(Original[I]->type);
        V->type->ref = false;
        V->type->constant = false;
        V->init = DSLCallbackFieldGet(Clone->at, V->type, Folder, Registry, Frame->name, int32(I));
        V->init_via_move = DSLContainer(V->type) || DSLDirectStruct(V->type);
        auto* L = new das::ExprLet();
        L->at = Clone->at;
        L->variables.push_back(V);
        Locals.push_back(L);
    }
    auto* Body = static_cast<das::ExprBlock*>(Clone->body);
    Body->list.insert(Body->list.begin(), Locals.begin(), Locals.end());
}
das::ExpressionPtr DSLMultiTimer(das::ExprCall* E, const das::StructurePtr& Source, const FDasDeclaredClass& Decl,
                                 const FString& Folder, const FString& Registry, das::ModuleGroup& Group,
                                 das::vector<das::FunctionPtr>& Generated, TSet<FString>& Helpers, bool Next)
{
    const size_t Base = Next ? 2 : 4, HandlerIndex = Next ? 1 : 2;
    if (E->arguments.size() > Base + 16 || E->arguments[HandlerIndex]->__rtti != das::string("ExprConstString"))
        DSLFail(TEXT("Class timer requires a literal method and at most 16 captures"));
    auto* Literal = static_cast<das::ExprConstString*>(E->arguments[HandlerIndex]);
    if (!Literal)
        DSLFail(TEXT("Class timer requires a literal callback name"));
    das::FunctionPtr Method = nullptr;
    const FString Name = DSLText(Literal->text);
    DSLVisitHandlerMethods(
        Source,
        [&](const das::FunctionPtr& F)
        {
            const FString N =
                DSLText(F->name.substr(F->name.find_last_of(das::string(1, char(96)) + das::string(1, char(39))) + 1));
            if (N == Name && DSLAnnotation(F->annotations, "ue_function") && !F->isStaticClassMethod && F->result &&
                DSLTypeName(F->result) == TEXT("void") && F->arguments.size() == E->arguments.size() - Base + 1)
                Method = F;
        });
    if (!Method)
        DSLFail(TEXT("Timer requires an annotated void method matching every capture"));
    auto* Type = DSLCallbackTuple(Method, Folder, Registry);
    const FString Path = Type->GetPathName();
    for (size_t I = 1; I < Method->arguments.size(); ++I)
    {
        const auto T = Method->arguments[I]->type;
        const auto D = DSLCallbackField(T, Folder, Registry);
        if (D.GetKind() == EDasTypeKind::Struct)
            DSLEnsureStructHelpers(T, D.TypePath, Group, Source->module, Generated, Helpers);
    }
    const das::string Key = DSLUtf8(FMD5::HashAnsiString(*(Decl.Id + TEXT(".")+ Name + Path))),
                                    Bridge = Name.StartsWith(
                                        TEXT("__ue_cb_")) ? DSLUtf8(Name) + "_frame_" + Key : "__ue_timer_frame_" + Key,
                                        Helper = "__ue_capture_" + Key + (Next ? "_next" : "_interval");
    auto AddArg = [&](das::Function* F, const das::string& N, const das::TypeDeclPtr& T)
    {
        auto* A = new das::Variable();
        A->at = E->at;
        A->name = N;
        A->type = DSLCopyType(T);
        A->type->ref = false;
        A->type->constant = true;
        F->arguments.push_back(A);
    };
    if (!Helpers.Contains(DSLText(Bridge)))
    {
        Helpers.Add(DSLText(Bridge));
        auto* Fn = new das::Function();
        Fn->at = E->at;
        Fn->name = Bridge;
        Fn->generated = true;
        Fn->exports = true;
        Fn->privateFunction = false;
        Fn->result = new das::TypeDecl(das::Type::tVoid, E->at);
        auto* Body = new das::ExprBlock();
        Body->at = E->at;
        auto* Invoke = DSLCall(E->at, "__impl_" + DSLCallbackEntry(Decl, Name), {});
        for (size_t I = 1; I < Method->arguments.size(); ++I)
        {
            AddArg(Fn, Method->arguments[I]->name, Method->arguments[I]->type);
            Invoke->arguments.push_back(new das::ExprVar(E->at, Method->arguments[I]->name));
        }
        Body->list.push_back(Invoke);
        Fn->body = Body;
        DSLCallbackUnpackArguments(Fn, Folder, Registry, Group, Source->module);
        auto A = Group.findAnnotation("__ue_callback_frame", Source->module);
        if (A.size() != 1)
            DSLFail(TEXT("Missing callback frame annotation"));
        auto* Annotation = new das::AnnotationDeclaration();
        Annotation->flags = 0;
        Annotation->annotation = A[0];
        Annotation->at = E->at;
        Annotation->arguments.push_back(das::AnnotationArgument("path", DSLUtf8(Path)));
        Fn->annotations.push_back(Annotation);
        Generated.push_back(Fn);
    }
    if (!Helpers.Contains(DSLText(Helper)))
    {
        Helpers.Add(DSLText(Helper));
        auto* Fn = new das::Function();
        Fn->at = E->at;
        Fn->name = Helper;
        Fn->generated = true;
        Fn->privateFunction = false;
        Fn->result = new das::TypeDecl(das::Type::tVoid, E->at);
        AddArg(Fn, "name", new das::TypeDecl(das::Type::tString, E->at));
        if (!Next)
        {
            AddArg(Fn, "seconds", new das::TypeDecl(das::Type::tFloat, E->at));
            AddArg(Fn, "repeat", new das::TypeDecl(das::Type::tBool, E->at));
        }
        auto* Body = new das::ExprBlock();
        Body->at = E->at;
        auto* Frame = new das::Variable();
        Frame->at = E->at;
        Frame->name = "__frame";
        Frame->type = DSLHandleType(Group, Source->module, "UCallHandle");
        Frame->init = DSLCall(E->at, "__ue_begin_callback_frame", {new das::ExprConstString(E->at, DSLUtf8(Path))});
        auto* L = new das::ExprLet();
        L->at = E->at;
        L->variables.push_back(Frame);
        Body->list.push_back(L);
        for (size_t I = 1; I < Method->arguments.size(); ++I)
        {
            const auto T = Method->arguments[I]->type;
            const auto D = DSLCallbackField(T, Folder, Registry);
            const das::string Arg = "capture_" + das::to_string(I);
            AddArg(Fn, Arg, T);
            das::ExpressionPtr Value = new das::ExprVar(E->at, Arg);
            if (D.GetKind() == EDasTypeKind::Byte || D.GetKind() == EDasTypeKind::Struct ||
                D.GetKind() == EDasTypeKind::Enum || D.GetKind() == EDasTypeKind::Class ||
                D.GetKind() == EDasTypeKind::Object)
                Value = DSLDirectPack(E->at, T, Value, D.TypePath);
            const das::string Prefix = (D.GetKind() == EDasTypeKind::Struct || D.GetKind() == EDasTypeKind::Enum ||
                                        D.GetKind() == EDasTypeKind::Name || D.GetKind() == EDasTypeKind::Text)
                                           ? "__ue_method_arg_"
                                           : "ue_arg_";
            Body->list.push_back(DSLCall(E->at, Prefix + DSLUtf8(D.Kind),
                                         {new das::ExprVar(E->at, Frame->name),
                                          new das::ExprConstString(E->at, "arg_" + das::to_string(I - 1)), Value}));
        }
        auto* Start = DSLCall(E->at, Next ? "__ue_timer_next_tick_frame" : "__ue_timer_start_frame",
                              {new das::ExprVar(E->at, "name")});
        if (!Next)
            Start->arguments.push_back(new das::ExprVar(E->at, "seconds"));
        Start->arguments.push_back(new das::ExprConstString(E->at, Bridge));
        if (!Next)
            Start->arguments.push_back(new das::ExprVar(E->at, "repeat"));
        Start->arguments.push_back(new das::ExprVar(E->at, Frame->name));
        Body->list.push_back(Start);
        Body->finalList.push_back(DSLCall(E->at, "ue_release_call", {new das::ExprVar(E->at, Frame->name)}));
        Fn->body = Body;
        Generated.push_back(Fn);
    }
    auto* Result = DSLCall(E->at, Helper, {E->arguments[0]});
    if (!Next)
    {
        Result->arguments.push_back(E->arguments[1]);
        Result->arguments.push_back(E->arguments[3]);
    }
    for (size_t I = Base; I < E->arguments.size(); ++I)
        Result->arguments.push_back(E->arguments[I]);
    return Result;
}
