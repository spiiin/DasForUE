// Reuse canonical authored/native struct validation and field-by-field unpack helpers.
struct FUEDataTableRowFactory : das::CallMacro
{
    FUEDataTableRowFactory() : CallMacro("ue_data_table_row") {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int Index) override { return Index != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if ((E->arguments.size() != 3 && E->arguments.size() != 4) || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_data_table_row(type<Row>, table, name [, type_folder_or_registry])"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            if (!DSLDirectStruct(T) || DSLAdaptedReference(T))
                DSLFail(TEXT("DataTable rows require an authored or canonical native struct"));
            FString Scope;
            if (E->arguments.size() == 4)
            {
                if (E->arguments[3]->__rtti != das::string("ExprConstString"))
                    DSLFail(TEXT("DataTable type scope must be a literal folder or registry path"));
                Scope = DSLText(static_cast<das::ExprConstString*>(E->arguments[3])->text);
            }
            const FString Path = DSLDirectPath(T, Scope.Contains(TEXT(".")) ? FString() : Scope,
                                                                 Scope.Contains(TEXT(".")) ? Scope : FString());
            das::vector<das::FunctionPtr> Generated;
            TSet<FString> Helpers;
            DSLEnsureStructHelpers(T, Path, *P->thisModuleGroup, M, Generated, Helpers);
            for (auto& F : Generated)
                if (!M->addFunction(F, true))
                    DSLFail(TEXT("DataTable struct helper collision"));
            das::ExpressionPtr Table = E->arguments[1];
            if (Table->type && DSLTypedReference(Table->type))
                Table = DSLDirectPack(E->at, Table->type, Table, DSLTypedReferencePath(Table->type));
            return DSLDirectUnpack(E->at, T,
                                   DSLCall(E->at, "__ue_data_table_row",
                                           {Table, E->arguments[2], new das::ExprConstString(E->at, DSLUtf8(Path))}),
                                   Path);
        }
        catch (const std::exception& X)
        {
            P->error("Typed DataTable read failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
