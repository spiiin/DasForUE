// Part of DasRuntime.cpp: built-in type adapters (DasTypeAdapters.h). Included after the scalar kind table
// and the DSL lowering, so kind traits and script facets are visible here.

// Scalar kinds own their reflected property class: creating the property and recognising it.
template <class K> class TDasScalarTypeAdapter final : public IDasTypeAdapter
{
public:
    explicit TDasScalarTypeAdapter(EDasTypeKind InKind) : Kind(InKind) {}
    virtual EDasTypeKind GetKind() const override { return Kind; }
    virtual bool MatchesProperty(const FProperty* Property) const override
    {
        const auto* Typed = CastField<typename K::Property>(Property);
        if constexpr (std::is_same_v<typename K::Property, FByteProperty>)
            return Typed && !Typed->Enum; // enum bytes are enums
        else
            return Typed != nullptr;
    }
    virtual FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType&,
                                    EDasPropertyUsage) const override
    {
        auto* Property = new typename K::Property(Owner, Name);
        if constexpr (std::is_same_v<typename K::Property, FBoolProperty>)
            Property->SetBoolSize(sizeof(bool), true);
        return Property;
    }

private:
    EDasTypeKind Kind;
};

void RegisterBuiltinTypeAdapters(FDasTypeAdapters& Registry)
{
#define DAS_REGISTER_SCALAR_ADAPTER(Suffix, SelfKind, RefKind)                                                         \
    Registry.Register(MakeShared<TDasScalarTypeAdapter<SelfKind>>(DasTypeKind(TEXT(#Suffix))),                         \
                                                                  DasScalarScriptFacet(DasTypeKind(TEXT(#Suffix))));
    DAS_SCALAR_ACCESSOR_KINDS(DAS_REGISTER_SCALAR_ADAPTER)
#undef DAS_REGISTER_SCALAR_ADAPTER
    static const FDasSoftReferenceScriptFacet SoftScript;
    Registry.Register(MakeDasSoftReferenceAdapter(), &SoftScript);
    static const FDasGameplayTagScriptFacet TagScript(false), TagContainerScript(true);
    Registry.Register(MakeDasGameplayTagAdapter(false), &TagScript);
    Registry.Register(MakeDasGameplayTagAdapter(true), &TagContainerScript);
    // Math structs and InputActionValue; the view uses keep the existing per-type asymmetries as data.
    using EUse = EDasScriptViewUse;
    const EUse AllUses = EUse::NativeProperty | EUse::SelfProperty | EUse::AuthoredReference;
    const EDasMemberUse AllMembers = EDasMemberUse::SelfLeaf | EDasMemberUse::AuthoredMutation;
    // Vector, Rotator and Transform own call-frame slots and method bridges; InputActionValue only a method
    // bridge; Vector2D crosses calls as a struct snapshot.
    static const FDasMathScriptFacet VectorScript("UEVector", "vector", AllUses, true, true, {"x", "y", "z"},
                                                  AllMembers),
        Vector2DScript("UEVector2D", "vector2d", EUse::NativeProperty | EUse::SelfProperty, false, false, {"x", "y"},
                       EDasMemberUse::AuthoredMutation),
        RotatorScript("UERotator", "rotator", AllUses, true, true, {"pitch", "yaw", "roll"}, AllMembers),
        TransformScript("UETransform", "transform", AllUses, true, true),
        InputValueScript("InputActionValue", "input_value", EUse::None, false, true);
    Registry.Register(MakeDasVectorAdapter(), &VectorScript);
    Registry.Register(MakeDasVector2DAdapter(), &Vector2DScript);
    Registry.Register(MakeDasRotatorAdapter(), &RotatorScript);
    Registry.Register(MakeDasTransformAdapter(), &TransformScript);
    Registry.Register(MakeDasInputActionValueAdapter(), &InputValueScript);
}
