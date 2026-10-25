#include "DasContainerAdapters.h"
#include "UObject/UnrealType.h"

namespace
{
const FDasContainerSlot ArraySlots[] = {{&FDasDeclaredType::Element, [](const FProperty* P) -> const FProperty*
                                         { return CastFieldChecked<FArrayProperty>(P)->Inner; }, false}};
const FDasContainerSlot SetSlots[] = {{&FDasDeclaredType::Element, [](const FProperty* P) -> const FProperty*
                                       { return CastFieldChecked<FSetProperty>(P)->ElementProp; }, true}};
const FDasContainerSlot MapSlots[] = {{&FDasDeclaredType::Key, [](const FProperty* P) -> const FProperty*
                                       { return CastFieldChecked<FMapProperty>(P)->KeyProp; }, true},
                                      {&FDasDeclaredType::Value, [](const FProperty* P) -> const FProperty*
                                       { return CastFieldChecked<FMapProperty>(P)->ValueProp; }, false}};

class FDasContainerAdapter final : public IDasTypeAdapter
{
    EDasTypeKind Kind;

public:
    explicit FDasContainerAdapter(EDasTypeKind InKind) : Kind(InKind) {}
    EDasTypeKind GetKind() const override { return Kind; }
    bool SupportsContainerValue() const override { return false; } // Direct nesting has no supported UE ABI.
    TConstArrayView<FDasContainerSlot> GetContainerSlots() const override
    {
        switch (Kind)
        {
        case EDasTypeKind::Array:
            return ArraySlots;
        case EDasTypeKind::Set:
            return SetSlots;
        case EDasTypeKind::Map:
            return MapSlots;
        default:
            checkNoEntry();
            return {};
        }
    }
    bool MatchesProperty(const FProperty* P) const override
    {
        if (!P)
            return false;
        return Kind == EDasTypeKind::Array ? P->IsA<FArrayProperty>()
               : Kind == EDasTypeKind::Set ? P->IsA<FSetProperty>()
                                           : P->IsA<FMapProperty>();
    }
    FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType& Type,
                            EDasPropertyUsage Usage) const override
    {
        // Also safe for callers using the adapter directly. Only the root may be attached by its caller.
        FString Error;
        if (!ValidateDasPropertyType(Type, Usage, Error) || Type.GetKind() != Kind)
            return nullptr;
        auto Child = [&](FFieldVariant Parent, FName ChildName, const TSharedPtr<const FDasDeclaredType>& T)
        {
            return CreateDasProperty(Parent, ChildName, *T, Usage, Error);
        };
        if (Kind == EDasTypeKind::Array)
        {
            auto* P = new FArrayProperty(Owner, Name);
            P->Inner =
                Child(P, Usage == EDasPropertyUsage::DelegateParameter ? TEXT("Inner") : TEXT("Values"), Type.Element);
            if (P->Inner)
                return P;
            delete P;
            return nullptr;
        }
        if (Kind == EDasTypeKind::Set)
        {
            auto* P = new FSetProperty(Owner, Name);
            P->ElementProp = Child(P, TEXT("Element"), Type.Element);
            if (P->ElementProp)
            {
                P->ElementProp->SetPropertyFlags(CPF_HasGetValueTypeHash);
                return P;
            }
            delete P;
            return nullptr;
        }
        auto* P = new FMapProperty(Owner, Name);
        P->KeyProp = Child(P, TEXT("Key"), Type.Key);
        P->ValueProp = Child(P, TEXT("Value"), Type.Value);
        if (P->KeyProp && P->ValueProp)
        {
            P->KeyProp->SetPropertyFlags(CPF_HasGetValueTypeHash);
            return P;
        }
        delete P;
        return nullptr;
    }
};
}
TSharedRef<const IDasTypeAdapter> MakeDasContainerAdapter(EDasTypeKind Kind)
{
    check(Kind == EDasTypeKind::Array || Kind == EDasTypeKind::Set || Kind == EDasTypeKind::Map);
    return MakeShared<FDasContainerAdapter>(Kind);
}
