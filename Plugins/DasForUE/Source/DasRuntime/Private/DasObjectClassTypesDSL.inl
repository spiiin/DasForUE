// Object/Class views share their native kind adapters; the existing checked codecs retain ownership rules.
class FDasObjectClassScriptFacet final : public IDasScriptTypeFacet
{
    bool Class;

public:
    explicit FDasObjectClassScriptFacet(bool IsClass) : Class(IsClass) {}
    bool MatchesCallType(const das::TypeDeclPtr& T) const override
    {
        return T && T->baseType == das::Type::tHandle && T->annotation &&
               T->annotation->name == (Class ? "UEClassHandle" : "UObjectHandle");
    }
    bool MatchesScriptType(const das::TypeDeclPtr& T) const override
    {
        return MatchesCallType(T) || (Class ? DSLTypedClass(T) : DSLTypedReference(T));
    }
    bool IsNativeType(const das::TypeDeclPtr& T) const override { return Class ? DSLTypedClass(T) : DSLTypedObject(T); }
    FString GetTypePath(const das::TypeDeclPtr& T) const override
    {
        return Class ? DSLTypedClassPath(T) : DSLTypedReferencePath(T);
    }
    das::ExpressionPtr Pack(const das::LineInfo& At, das::ExpressionPtr V) const override
    {
        return PackTyped(At, V->type, V);
    }
    das::ExpressionPtr PackTyped(const das::LineInfo& At, const das::TypeDeclPtr& T,
                                 das::ExpressionPtr V) const override
    {
        if (MatchesCallType(T))
            return V;
        if (Class)
            return DSLClassPack(At, T, V);
        if (DSLAuthoredView(T))
            return DSLAuthoredPack(At, T, V);
        if (DSLTypedObject(T))
            return DSLCall(
                At, "ue_cast_nullable_object",
                {new das::ExprField(At, V, "handle"), new das::ExprConstString(At, DSLUtf8(DSLTypedReferencePath(T)))});
        return DSLCall(At, T->structType->module->name + "::" + T->structType->name + "_raw", {V});
    }
    das::ExpressionPtr Unpack(const das::LineInfo& At, const das::TypeDeclPtr& T, das::ExpressionPtr V) const override
    {
        if (MatchesCallType(T))
            return V;
        if (Class)
            return DSLClassUnpack(At, T, V);
        return DSLAuthoredView(T) ? DSLAuthoredUnpack(At, T, V) : DSLNativeReferenceUnpack(At, T, V);
    }
    EDasScriptViewUse GetViewUses() const override
    {
        return EDasScriptViewUse::NativeProperty | EDasScriptViewUse::SelfProperty |
               EDasScriptViewUse::AuthoredReference;
    }
    das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module, const das::LineInfo&) const override
    {
        return DSLHandleType(Group, Module, Class ? "UEClassHandle" : "UObjectHandle");
    }
    das::TypeDeclPtr MakeViewForPath(const FString&, das::ModuleGroup&, das::Module*,
                                     const das::LineInfo&) const override;
    const TCHAR* GetViewModule() const override { return TEXT("ue_native_gameplay"); }
    bool DeclareProperty(const das::TypeDeclPtr&, das::Structure::FieldDeclaration&, const FDasScriptTypeScope&,
                         FDasDeclaredProperty&) const override;
    TSharedPtr<FJsonValue> DefaultJson(const das::TypeDeclPtr&, das::ExpressionPtr, das::ModuleGroup&,
                                       das::Module*) const override;
};
