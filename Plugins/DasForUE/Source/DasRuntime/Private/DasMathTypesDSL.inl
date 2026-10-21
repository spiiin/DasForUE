// Part of DasDSL.inl: script facets of the math struct adapters (DasMathAdapters.cpp). The views are
// ue_foundation handle types; values cross the boundary through __ue_math_pack/unpack_<suffix>.
class FDasMathScriptFacet final : public IDasScriptTypeFacet
{
public:
    FDasMathScriptFacet(const char* InHandle, const char* InSuffix, EDasScriptViewUse InUses,
                        std::initializer_list<const char*> InMembers = {},
                        EDasMemberUse InMemberUses = EDasMemberUse::None)
        : Handle(InHandle), Suffix(InSuffix), Uses(InUses), Members(InMembers), MemberUses(InMemberUses)
    {
    }
    virtual bool MatchesScriptType(const das::TypeDeclPtr& T) const override
    {
        return T && T->baseType == das::Type::tHandle && T->annotation && T->annotation->name == Handle;
    }
    virtual das::ExpressionPtr Pack(const das::LineInfo& At, das::ExpressionPtr V) const override
    {
        return DSLCall(At, "__ue_math_pack_" + Suffix, {V});
    }
    virtual das::ExpressionPtr Unpack(const das::LineInfo& At, const das::TypeDeclPtr&,
                                      das::ExpressionPtr V) const override
    {
        return DSLCall(At, "__ue_math_unpack_" + Suffix, {V});
    }
    virtual EDasScriptViewUse GetViewUses() const override { return Uses; }
    virtual das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module, const das::LineInfo&) const override
    {
        return DSLHandleType(Group, Module, Handle.c_str());
    }
    virtual const TCHAR* GetViewModule() const override { return TEXT("ue_foundation"); }
    virtual bool HasMember(const das::string& Name, EDasMemberUse Use) const override
    {
        if (!EnumHasAnyFlags(MemberUses, Use))
            return false;
        for (const char* Member : Members)
            if (Name == Member)
                return true;
        return false;
    }

private:
    das::string Handle, Suffix;
    EDasScriptViewUse Uses;
    TArray<const char*> Members;
    EDasMemberUse MemberUses;
};
