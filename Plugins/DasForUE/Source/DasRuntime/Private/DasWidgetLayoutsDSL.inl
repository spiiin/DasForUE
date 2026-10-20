// Uses the existing nominal UObject reference wrappers; a layout adds no ownership.
FDasWidgetLayoutUse DSLWidgetLayout(const das::TypeDeclPtr& Type, das::ModuleGroup& Group, das::Module* Module,
                                    bool RequireMark = true)
{
    auto T = DSLObjectNormalize(Type, Group, Module);
    if (!T || T->baseType != das::Type::tStructure || !T->structType ||
        (RequireMark && !DSLAnnotation(T->structType->annotations, "ue_widget_layout")))
        DSLFail(TEXT("Expected a [ue_widget_layout] struct"));
    auto St = T->structType;
    if (St->isClass || St->parent)
        DSLFail(TEXT("ue_widget_layout requires a plain struct without inheritance"));
    FDasWidgetLayoutUse L;
    L.Name = DSLText(St->name);
    TSet<FName> Names;
    for (const auto& F : St->fields)
    {
        auto FT = DSLObjectNormalize(F.type, Group, Module);
        if (F.classMethod || F.generated || F.init || !FT || FT->ref || !DSLTypedObject(FT))
            DSLFail(TEXT("Widget layout field must be a typed widget reference without an initializer: ")+
                         DSLText(F.name));
        const FString Path = DSLTypedObjectPath(FT);
        auto* Class = FindObject<UClass>(nullptr, *Path);
        if (!Class || !Class->IsChildOf(UWidget::StaticClass()))
            DSLFail(TEXT("Widget layout field ")+ DSLText(F.name) + TEXT(" is not a UWidget type: ")+ Path);
        const FName N(UTF8_TO_TCHAR(F.name.c_str()));
        if (Names.Contains(N))
            DSLFail(TEXT("Widget layout names collide in Unreal: ")+ DSLText(F.name));
        Names.Add(N);
        L.Fields.Add(DSLText(F.name), Path);
    }
    if (L.Fields.IsEmpty() || L.Fields.Num() > 256)
        DSLFail(TEXT("Widget layout requires 1..256 fields"));
    return L;
}
struct FUEWidgetLayoutAnnotation : das::StructureAnnotation
{
    FUEWidgetLayoutAnnotation() : StructureAnnotation("ue_widget_layout") {}
    virtual bool look(const das::StructurePtr&, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool touch(const das::StructurePtr& St, das::ModuleGroup& G, das::AnnotationArgumentList& A,
                       das::string& Error) override
    {
        try
        {
            if (!A.empty())
                DSLFail(TEXT("ue_widget_layout takes no arguments"));
            auto* T = new das::TypeDecl(das::Type::tStructure, St->at);
            T->structType = St;
            DSLWidgetLayout(T, G, St->module, false);
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
struct FUEWidgetViewMacro : das::CallMacro
{
    FUEWidgetViewMacro() : CallMacro("ue_widget_view") {}
    virtual bool canVisitArguments(das::ExprCallMacro*, int I) override { return I != 0; }
    virtual das::ExpressionPtr visit(das::Program* P, das::Module* M, das::ExprCallMacro* E) override
    {
        try
        {
            if (E->arguments.size() != 2 || !E->arguments[0]->rtti_isTypeDecl())
                DSLFail(TEXT("Expected ue_widget_view(type<Layout>, widget)"));
            auto T =
                DSLObjectNormalize(static_cast<das::ExprTypeDecl*>(E->arguments[0])->typeexpr, *P->thisModuleGroup, M);
            auto L = DSLWidgetLayout(T, *P->thisModuleGroup, M);
            auto W = DSLObjectNormalize(E->arguments[1]->type, *P->thisModuleGroup, M);
            auto* WidgetClass = DSLTypedObject(W) ? FindObject<UClass>(nullptr, *DSLTypedObjectPath(W)) : nullptr;
            if (!WidgetClass || !WidgetClass->IsChildOf(UUserWidget::StaticClass()))
                DSLFail(TEXT("ue_widget_view requires a typed UserWidget reference"));
            const FString Spec = DasWidgetLayoutSpec(L);
            const das::string Name =
                "__ue_widget_view_" +
                DSLUtf8(FMD5::HashAnsiString(*FString(UTF8_TO_TCHAR(
                    (T->structType->module->name + "::" + T->structType->name + "|" + W->describe()).c_str()))));
            auto* Module = P->getThisModule();
            if (!Module->findUniqueFunction(Name))
            {
                das::FunctionPtr F = new das::Function();
                F->name = Name;
                F->at = E->at;
                F->generated = true;
                F->privateFunction = false;
                F->result = DSLCopyType(T);
                F->result->ref = false;
                F->result->constant = false;
                auto* A = new das::Variable();
                A->name = "widget";
                A->at = E->at;
                A->type = DSLCopyType(W);
                A->type->ref = false;
                A->type->constant = true;
                F->arguments.push_back(A);
                auto Root = [&]()
                {
                    return new das::ExprField(E->at, new das::ExprVar(E->at, "widget"), "handle");
                };
                auto* B = new das::ExprBlock();
                B->at = E->at;
                B->list.push_back(DSLCall(E->at, "__ue_widget_layout_resolve",
                                          {Root(), new das::ExprConstString(E->at, DSLUtf8(Spec))}));
                auto* Make = new das::ExprMakeStruct(E->at);
                Make->makeType = DSLCopyType(T);
                Make->makeType->ref = false;
                Make->makeType->constant = false;
                Make->useInitializer = true;
                Make->alwaysUseInitializer = true;
                auto* Fields = new das::MakeStruct();
                for (const auto& Field : T->structType->fields)
                {
                    auto FT = DSLObjectNormalize(Field.type, *P->thisModuleGroup, M);
                    auto* Value = new das::ExprMakeStruct(E->at);
                    Value->makeType = DSLCopyType(FT);
                    Value->makeType->ref = false;
                    Value->makeType->constant = false;
                    Value->useInitializer = true;
                    Value->alwaysUseInitializer = true;
                    auto* Handles = new das::MakeStruct();
                    Handles->push_back(
                        new das::MakeFieldDecl(E->at, "handle",
                                               DSLCall(E->at, "__ue_widget_layout_field",
                                                       {Root(), new das::ExprConstString(E->at, DSLUtf8(Spec)),
                                                        new das::ExprConstString(E->at, Field.name)}),
                                               false, false));
                    Value->structs.push_back(Handles);
                    Fields->push_back(new das::MakeFieldDecl(E->at, Field.name, Value, false, false));
                }
                Make->structs.push_back(Fields);
                B->list.push_back(new das::ExprReturn(E->at, Make));
                F->body = B;
                if (!Module->addFunction(F, true))
                    DSLFail(TEXT("Widget view helper collision"));
            }
            return DSLCall(E->at, Name, {E->arguments[1]});
        }
        catch (const std::exception& X)
        {
            P->error("Widget view failed", X.what(), "", E->at, das::CompilationError::invalid_type);
            return nullptr;
        }
    }
};
