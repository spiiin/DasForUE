// Script facet of the GameplayTag adapters (DasGameplayTags.cpp): canonical opaque views from
// ue_gameplay_tags over the existing owned UEStructValue table.
class FDasGameplayTagScriptFacet final : public IDasScriptTypeFacet
{
public:
    explicit FDasGameplayTagScriptFacet(bool bInContainer) : bContainer(bInContainer) {}
    virtual bool MatchesScriptType(const das::TypeDeclPtr& T) const override
    {
        return T && T->baseType == das::Type::tStructure && T->structType &&
               DSLAnnotation(T->structType->annotations, "ue_gameplay_value") &&
               T->structType->name == (bContainer ? "GameplayTagContainer" : "GameplayTag");
    }
    virtual das::ExpressionPtr Pack(const das::LineInfo& At, das::ExpressionPtr V) const override
    {
        return DSLCall(At, "__ue_tag_check",
                       {new das::ExprField(At, V, "value"), new das::ExprConstBool(At, bContainer)});
    }
    virtual das::ExpressionPtr Unpack(const das::LineInfo& At, const das::TypeDeclPtr& T,
                                      das::ExpressionPtr V) const override
    {
        auto* Make = new das::ExprMakeStruct(At);
        Make->makeType = DSLObjectCopyType(T);
        Make->makeType->ref = false;
        Make->makeType->constant = false;
        Make->useInitializer = true;
        Make->alwaysUseInitializer = true;
        auto* Fields = new das::MakeStruct();
        Fields->push_back(new das::MakeFieldDecl(
            At, "value", DSLCall(At, "__ue_tag_check", {V, new das::ExprConstBool(At, bContainer)}), false, false));
        Make->structs.push_back(Fields);
        return Make;
    }
    virtual EDasScriptViewUse GetViewUses() const override { return EDasScriptViewUse::NativeProperty; }
    virtual das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module,
                                      const das::LineInfo& At) const override
    {
        auto Views = Group.findStructure(
            bContainer ? "ue_gameplay_tags::GameplayTagContainer" : "ue_gameplay_tags::GameplayTag", Module);
        if (Views.size() != 1)
            return nullptr;
        auto* T = new das::TypeDecl(das::Type::tStructure, At);
        T->structType = Views[0];
        return T;
    }
    virtual const TCHAR* GetViewModule() const override { return TEXT("ue_gameplay_tags"); }

private:
    bool bContainer;
};
struct FUEGameplayValueAnnotation : das::StructureAnnotation
{
    FUEGameplayValueAnnotation() : StructureAnnotation("ue_gameplay_value") {}
    virtual bool look(const das::StructurePtr&, das::ModuleGroup&, das::AnnotationArgumentList&, das::string&) override
    {
        return true;
    }
    virtual bool touch(const das::StructurePtr& S, das::ModuleGroup&, das::AnnotationArgumentList& A,
                       das::string& Error) override
    {
        try
        {
            if (!S->module || S->module->name != "ue_gameplay_tags" ||
                (S->name != "GameplayTag" && S->name != "GameplayTagContainer") || S->isClass || S->parent ||
                !A.empty() || S->fields.size() != 1 || S->fields[0].name != "value" || S->fields[0].init ||
                !S->fields[0].type || S->fields[0].type->ref || S->fields[0].type->baseType != das::Type::tHandle ||
                !S->fields[0].type->annotation || S->fields[0].type->annotation->name != "UEStructValue")
                DSLFail(TEXT("GameplayTag views must use the canonical ue_gameplay_tags declarations"));
            return true;
        }
        catch (const std::exception& X)
        {
            Error = X.what();
            return false;
        }
    }
};
