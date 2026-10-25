// Enum declaration annotations, included after DSLTypeArgs and the enum facet.
struct FUENativeEnumAnnotation : das::EnumerationAnnotation
{
    FUENativeEnumAnnotation() : EnumerationAnnotation("ue_native_enum") {}
    virtual bool touch(const das::EnumerationPtr& E, das::ModuleGroup& Group, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            DSLAnnotationArguments(Args);
            if (Args.size() != 1 || Args[0].name != "path" || Args[0].type != das::Type::tString ||
                DSLAnnotation(E->annotations, "ue_enum"))
                DSLFail(TEXT("ue_native_enum requires a single native path and no ue_enum"));
            const FString Path = DSLString(Args, "path");
            FString Issue;
            auto* Native = Path.StartsWith(TEXT("/Script/")) ? ResolveDasEnum(Path, Issue) : nullptr;
            if (!Native)
                DSLFail(TEXT("Expected loaded native enum path"));
            TSet<int64> Values;
            for (const auto& V : E->list)
            {
                if (!V.value || V.value->__rtti != das::string("ExprConstInt"))
                    DSLFail(TEXT("Native enum declarations require explicit integer values"));
                const int64 Number = static_cast<das::ExprConstInt*>(V.value)->getValue();
                const int64 Actual = Native->GetValueByNameString(DSLText(V.name));
                const int32 Index = Native->GetIndexByValue(Actual);
                if (Number < 0 || Number > 254 || Index == INDEX_NONE || Actual != Number ||
                    !Native->GetNameStringByIndex(Index).Equals(DSLText(V.name), ESearchCase::CaseSensitive) ||
                    Values.Contains(Number) || Native->GetNameStringByIndex(Index).EndsWith(TEXT("_MAX")))
                    DSLFail(TEXT("Native enum member name/value differs from Unreal"));
                Values.Add(Number);
            }
            for (int32 I = 0; I < Native->NumEnums(); ++I)
            {
                const int64 Number = Native->GetValueByIndex(I);
                if (Number >= 0 && Number <= 254 &&
                    !Native->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")) && !Values.Contains(Number))
                    DSLFail(TEXT("Native enum declaration omits a native value"));
            }
            auto* F = new das::Function();
            F->at = E->at;
            F->name = "__ue_pack_native_enum_" + E->name;
            F->generated = true;
            F->privateFunction = false;
            F->result = DSLKindFacet(EDasTypeKind::Enum)->MakeView(Group, E->module, E->at);
            auto* Arg = new das::Variable();
            Arg->at = E->at;
            Arg->name = "value";
            Arg->type = new das::TypeDecl(das::Type::tEnumeration, E->at);
            Arg->type->enumType = E;
            Arg->type->constant = true;
            F->arguments.push_back(Arg);
            auto* Body = new das::ExprBlock();
            Body->at = E->at;
            Body->list.push_back(
                new das::ExprReturn(E->at, DSLCall(E->at, "ue_enum_from_number",
                                                   {new das::ExprConstString(E->at, DSLUtf8(Path)),
                                                    DSLCall(E->at, "int", {new das::ExprVar(E->at, "value")})})));
            F->body = Body;
            if (!E->module->addFunction(F, true))
                DSLFail(TEXT("Native enum pack helper collision"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEEnumAnnotation : das::EnumerationAnnotation
{
    FUEEnumAnnotation() : EnumerationAnnotation("ue_enum") {}
    virtual bool touch(const das::EnumerationPtr& E, das::ModuleGroup&, das::AnnotationArgumentList& Args,
                       das::string& Error) override
    {
        try
        {
            if (const auto* Wire = Args.find(DSLTypesKey, das::Type::tString))
            {
                if (Wire->at.fileInfo)
                    DSLFail(TEXT("Reserved type annotation argument"));
                return true;
            }
            DSLCommentTooltip(E->at, Args);
            FDasReflectedTypes M;
            M.Id = DSLString(Args, "module_id", FPaths::GetBaseFilename(DSLText(E->at.fileInfo->name)));
            FDasEnumDeclaration D;
            D.Name = DSLText(E->name);
            D.Id = DSLString(Args, "id", M.Id + TEXT(".")+ D.Name);
            das::AnnotationArgumentList TypeArgs;
            for (const auto& A : Args)
                if (A.name.find("member_id_") != 0)
                    TypeArgs.push_back(A);
            DSLTypeArgs(TypeArgs, D.Metadata);
            for (const auto& A : Args)
                if (A.name.find("member_id_") == 0)
                {
                    bool Found = false;
                    for (const auto& V : E->list)
                        Found |= A.name == "member_id_" + V.name;
                    if (!Found || A.type != das::Type::tString)
                        DSLFail(TEXT("Unknown member or invalid member ID annotation"));
                }
            for (const auto& V : E->list)
            {
                if (!V.value || V.value->__rtti != das::string("ExprConstInt"))
                    DSLFail(TEXT("ue_enum requires explicit int literal values"));
                D.Entries.Add({DSLText(V.name), static_cast<das::ExprConstInt*>(V.value)->getValue(),
                               DSLString(Args, ("member_id_" + V.name).c_str(), D.Id + TEXT(".")+ DSLText(V.name))});
            }
            M.Enums.Add(D);
            FString Wire, Issue;
            if (!M.ToJson(Wire, Issue))
                DSLFail(Issue);
            Args.push_back(das::AnnotationArgument(DSLTypesKey, DSLUtf8(Wire)));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
