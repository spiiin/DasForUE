// Script facets attached to the existing scalar Name/Text property and value adapters.
FDasDeclaredLiteral DSLTextDefault(das::ExpressionPtr E)
{
    FDasDeclaredLiteral D;
    if (!E)
        return D;
    if (!E->rtti_isCall())
        DSLFail(TEXT("FText defaults require ue_text or ue_text_invariant literals"));
    auto* Call = static_cast<das::ExprCall*>(E);
    const bool Invariant = Call->name == "ue_text_invariant";
    if (!Invariant && Call->name != "ue_text")
        DSLFail(TEXT("FText defaults require ue_text or ue_text_invariant"));
    if (Call->arguments.size() != (Invariant ? 1 : 3))
        DSLFail(TEXT("Invalid text default arguments"));
    TArray<FString> A;
    for (auto* X : Call->arguments)
    {
        if (X->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Text default arguments must be string literals"));
        A.Add(DSLText(static_cast<das::ExprConstString*>(X)->text));
    }
    if ((!Invariant && (A[0].IsEmpty() || A[1].IsEmpty() || A[0].Len() > 1024 || A[1].Len() > 1024)) ||
        A.Last().Len() > 65536)
        DSLFail(TEXT("Text default identity/source limits exceeded"));
    D.Kind = TEXT("string");
    D.String = DasTextLiteral(Invariant ? FString() : A[0], Invariant ? FString() : A[1], A.Last(), Invariant);
    return D;
}
das::ExpressionPtr DSLTextDefaultExpression(const FDasDeclaredLiteral& D, const das::LineInfo& At)
{
    FText Text;
    FString Error;
    if (!ParseDasTextDefault(D.String, Text, Error))
        DSLFail(Error);
    auto* Call = new das::ExprCall(At, Text.IsCultureInvariant() ? "ue_text_invariant" : "ue_text");
    if (!Text.IsCultureInvariant())
    {
        const auto N = FTextInspector::GetNamespace(Text), K = FTextInspector::GetKey(Text);
        if (!N.IsSet() || !K.IsSet())
            DSLFail(TEXT("Text default has no localization identity"));
        Call->arguments.push_back(new das::ExprConstString(At, DSLUtf8(N.GetValue())));
        Call->arguments.push_back(new das::ExprConstString(At, DSLUtf8(K.GetValue())));
    }
    const auto* Source = FTextInspector::GetSourceString(Text);
    Call->arguments.push_back(new das::ExprConstString(At, DSLUtf8(Source ? *Source : FString())));
    return Call;
}


class FDasTextNameScriptFacet final : public IDasScriptTypeFacet
{
    bool Text;

public:
    explicit FDasTextNameScriptFacet(bool IsText) : Text(IsText) {}
    bool MatchesScriptType(const das::TypeDeclPtr& Type) const override
    {
        return Type && Type->baseType == das::Type::tHandle && Type->annotation &&
               Type->annotation->name == (Text ? "UETextValue" : "UENameValue");
    }
    das::ExpressionPtr Pack(const das::LineInfo&, das::ExpressionPtr Value) const override { return Value; }
    das::ExpressionPtr Unpack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr Value) const override
    {
        return Value;
    }
    EDasScriptViewUse GetViewUses() const override
    {
        return EDasScriptViewUse::NativeProperty | EDasScriptViewUse::SelfProperty |
               EDasScriptViewUse::AuthoredReference;
    }
    das::TypeDeclPtr MakeView(das::ModuleGroup& Group, das::Module* Module, const das::LineInfo&) const override
    {
        return DSLHandleType(Group, Module, Text ? "UETextValue" : "UENameValue");
    }
    const TCHAR* GetViewModule() const override { return TEXT("ue_foundation"); }
    const char* GetHandleName() const override { return Text ? "UETextValue" : "UENameValue"; }
    bool ParseDefault(EDasScriptDefaultUse Use, const das::TypeDeclPtr&, das::ExpressionPtr E, das::ModuleGroup&,
                      das::Module*, FDasDeclaredLiteral& Default) const override
    {
        if (Text)
        {
            Default = DSLTextDefault(E);
            return true;
        }
        if (Use != EDasScriptDefaultUse::Property)
            return false;
        // Preserve plain-string defaults and let the common validator reject other literal kinds.
        if (!E || !E->rtti_isCall())
            return false;
        auto* Call = static_cast<das::ExprCall*>(E);
        if (Call->name != "ue_name_value" || Call->arguments.size() != 1 ||
            Call->arguments[0]->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Name default requires ue_name_value with literal string"));
        Default.Kind = TEXT("string");
        Default.String = DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text);
        return true;
    }
    das::ExpressionPtr DefaultExpression(const FDasDeclaredLiteral& Default, const das::LineInfo& At) const override
    {
        return Text ? DSLTextDefaultExpression(Default, At) : nullptr;
    }
    TSharedPtr<FJsonValue> DefaultJson(const das::TypeDeclPtr&, das::ExpressionPtr E, das::ModuleGroup&,
                                       das::Module*) const override
    {
        if (Text)
        {
            const auto D = DSLTextDefault(E);
            auto Object = MakeShared<FJsonObject>();
            Object->SetStringField(TEXT("text"), D.String);
            return MakeShared<FJsonValueObject>(Object);
        }
        auto* Call = E->rtti_isCall() ? static_cast<das::ExprCall*>(E) : nullptr;
        if (!Call || Call->name != "ue_name_value" || Call->arguments.size() != 1 ||
            Call->arguments[0]->__rtti != das::string("ExprConstString"))
            DSLFail(TEXT("Container FName defaults require ue_name_value(string)"));
        return MakeShared<FJsonValueString>(DSLText(static_cast<das::ExprConstString*>(Call->arguments[0])->text));
    }
};
const IDasScriptTypeFacet* DasScalarScriptFacet(EDasTypeKind Kind)
{
    static const FDasTextNameScriptFacet Text(true), Name(false);
    return Kind == EDasTypeKind::Text ? &Text : Kind == EDasTypeKind::Name ? &Name : nullptr;
}
