// Shared ABI adapter for reflected class methods and free-function delegate handlers.
das::FunctionPtr DSLHandlerABI(const das::FunctionPtr& SourceFunction, const das::FunctionPtr& Prototype,
                               const das::string& Target, const FDasDeclaredMethod& M, size_t Offset,
                               das::ModuleGroup& Group, das::Module* Module, const FString& TypeFolder,
                               const FString& TypeRegistry)
{
    auto Wrapper = Prototype->clone();
    bool HasOutputs = false;
    for (const auto& P : M.Parameters)
        HasOutputs |= P.Direction != TEXT("in");
    auto* Call = new das::ExprCall(SourceFunction->at, Target);
    auto* Block = new das::ExprBlock();
    Block->at = SourceFunction->at;
    auto Local = [&](const das::string& Name, const das::TypeDeclPtr& Type, das::ExpressionPtr Init)
    {
        auto* V = new das::Variable();
        V->at = SourceFunction->at;
        V->name = Name;
        V->type = DSLCopyType(Type);
        V->type->ref = false;
        V->type->constant = false;
        V->init = Init;
        V->init_via_move = DSLContainer(Type);
        auto* Let = new das::ExprLet();
        Let->at = SourceFunction->at;
        Let->variables.push_back(V);
        Block->list.push_back(Let);
    };
    for (size_t I = 0; I < Wrapper->arguments.size(); ++I)
    {
        auto& Arg = Wrapper->arguments[I];
        Arg->init = nullptr;
        auto Original = SourceFunction->arguments[I + Offset]->type;
        Arg->type = DSLCopyType(Arg->type);
        Arg->type->ref = false;
        Arg->type->constant = true;
        das::ExpressionPtr Value = new das::ExprVar(SourceFunction->at, Arg->name);
        if (DSLDelegate(Original))
        {
            Arg->type = new das::TypeDecl(das::Type::tUInt64, SourceFunction->at);
            Value = DSLDirectUnpack(SourceFunction->at, Original, Value, FString());
        }
        if (DSLTypedClass(Original) || DSLContainer(Original) || DSLDirectEnum(Original) ||
            (DSLDirectStruct(Original) ||
             (DSLMathPath(Original) == TEXT("/Script/CoreUObject.Vector2D") ||
                                            DSLMathPath(Original) == TEXT("/Script/EnhancedInput.InputActionValue"))) ||
             DSLTypedReference(Original))
        {
            Arg->type = DSLHandleType(Group, Module,
                                      DSLTypedClass(Original)       ? "UEClassHandle"
                                      : DSLTypedReference(Original) ? "UObjectHandle"
                                      : DSLDirectEnum(Original)     ? "UEEnumValue"
                                                                    : "UEStructValue");
            Value = DSLDirectUnpack(SourceFunction->at, Original, Value,
                                    DSLContainer(Original) ? DSLDirectPath(Original, TypeFolder, TypeRegistry)
                                                           : M.Parameters[int32(I)].Type.TypePath);
        }
        if (M.Parameters[int32(I)].Direction != TEXT("in"))
        {
            const das::string Name = "__ue_ref_" + das::to_string(I);
            Local(Name, Original, Value);
            Value = new das::ExprVar(SourceFunction->at, Name);
        }
        Call->arguments.push_back(Value);
    }
    das::ExpressionPtr Value = Call;
    if (HasOutputs)
    {
        if (M.ReturnType.Kind == TEXT("void"))
        {
            Block->list.push_back(Call);
            Value = nullptr;
        }
        else
        {
            Local("__ue_return", SourceFunction->result, Call);
            Value = new das::ExprVar(SourceFunction->at, "__ue_return");
        }
        for (int32 I = 0; I < M.Parameters.Num(); ++I)
            if (M.Parameters[I].Direction != TEXT("in"))
            {
                auto Original = SourceFunction->arguments[size_t(I) + Offset]->type;
                das::ExpressionPtr Updated = new das::ExprVar(SourceFunction->at, "__ue_ref_" + das::to_string(I));
                const FString Kind = M.Parameters[I].Type.Kind;
                const das::string Math = DSLUtf8(DSLMathPath(Original));
                const das::string Suffix =
                    Kind ==
                    TEXT("class")
                         ? "class"
                         : Kind == TEXT("object")
                                        ? "object"
                                        : Kind == TEXT("enum")
                                                       ? "enum"
                                                       : Kind == TEXT("struct")
                                                                      ? (Math == "/Script/CoreUObject.Vector" ? "vector"
                                                                         : Math == "/Script/CoreUObject.Rotator"
                                                                             ? "rotator"
                                                                         : Math == "/Script/CoreUObject.Transform"
                                                                             ? "transform"
                                                                             : "struct")
                                                                  : DSLContainer(Original) ? "struct"
                                                                                           : DSLScalarSuffix(Kind);
                if (DSLTypedClass(Original) || DSLContainer(Original) ||
                    (DSLDirectStruct(Original) ||
                     (DSLMathPath(Original) == TEXT("/Script/CoreUObject.Vector2D") ||
                                                    DSLMathPath(Original) ==
                                                        TEXT("/Script/EnhancedInput.InputActionValue"))) ||
                     DSLDirectEnum(Original) || DSLTypedReference(Original))
                    Updated = DSLDirectPack(SourceFunction->at, Original, Updated,
                                            DSLContainer(Original) ? DSLDirectPath(Original, TypeFolder, TypeRegistry)
                                                                   : M.Parameters[I].Type.TypePath);
                Block->list.push_back(DSLCall(SourceFunction->at, "ue_frame_out_" + Suffix,
                                              {new das::ExprConstInt(SourceFunction->at, I), Updated}));
            }
    }
    if (DSLDelegate(SourceFunction->result))
    {
        Wrapper->result = new das::TypeDecl(das::Type::tUInt64, SourceFunction->at);
        Value = DSLDirectPack(SourceFunction->at, SourceFunction->result, Value, FString());
    }
    if (DSLTypedClass(SourceFunction->result) || DSLContainer(SourceFunction->result) ||
        DSLDirectEnum(SourceFunction->result) ||
        (DSLDirectStruct(SourceFunction->result) ||
         (DSLMathPath(SourceFunction->result) == TEXT("/Script/CoreUObject.Vector2D") ||
                                                      DSLMathPath(SourceFunction->result) ==
                                                          TEXT("/Script/EnhancedInput.InputActionValue"))) ||
         DSLTypedReference(SourceFunction->result))
    {
        Wrapper->result = DSLHandleType(Group, Module,
                                        DSLTypedClass(SourceFunction->result)       ? "UEClassHandle"
                                        : DSLTypedReference(SourceFunction->result) ? "UObjectHandle"
                                        : DSLDirectEnum(SourceFunction->result)     ? "UEEnumValue"
                                                                                    : "UEStructValue");
        Value = DSLDirectPack(SourceFunction->at, SourceFunction->result, Value,
                              DSLContainer(SourceFunction->result)
                                  ? DSLDirectPath(SourceFunction->result, TypeFolder, TypeRegistry)
                                  : M.ReturnType.TypePath);
    }
    if (Value)
        Block->list.push_back(
            M.ReturnType.Kind ==
            TEXT("void")? Value : static_cast<das::ExpressionPtr>(new das::ExprReturn(SourceFunction->at, Value)));
    Wrapper->body = Block;
    return Wrapper;
}
