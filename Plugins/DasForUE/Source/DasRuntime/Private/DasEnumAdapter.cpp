#include "DasEnumAdapter.h"
#include "DasDefinedEnum.h"
#include "UObject/UnrealType.h"

EDasEnumStorage DasAutomaticEnumStorage(const UEnum* Enum)
{
    return Enum && !Enum->IsA<UDasDefinedEnum>() && Enum->GetCppForm() == UEnum::ECppForm::EnumClass
               ? EDasEnumStorage::Enum8
               : EDasEnumStorage::Byte;
}
EDasEnumStorage DasResolvedEnumStorage(const FDasDeclaredType& Type)
{
    if (Type.GetKind() != EDasTypeKind::Enum)
        return EDasEnumStorage::Automatic;
    if (Type.EnumStorage != EDasEnumStorage::Automatic)
        return Type.EnumStorage;
    return DasAutomaticEnumStorage(FindObject<UEnum>(nullptr, *Type.TypePath));
}

class FDasEnumAdapter final : public IDasTypeAdapter
{
public:
    EDasTypeKind GetKind() const override { return EDasTypeKind::Enum; }
    bool CanBeContainerKey(const FDasDeclaredType&) const override { return true; }
    bool UsesTypeIdentity() const override { return true; }
    bool MatchesProperty(const FProperty* Property) const override
    {
        const auto* Byte = CastField<FByteProperty>(Property);
        return CastField<FEnumProperty>(Property) || (Byte && Byte->Enum);
    }
    TOptional<bool> ValidateDeclared(const FDasDeclaredType& Type, EDasPropertyUsage, FString& Error) const override
    {
        auto* E = FindObject<UEnum>(nullptr, *Type.TypePath);
        if (!E)
        {
            Error = TEXT("Missing loaded enum");
            return false;
        }
        for (int32 I = 0; I < E->NumEnums(); ++I)
        {
            if (E->GetNameStringByIndex(I).EndsWith(TEXT("_MAX")))
                continue;
            const auto N = E->GetValueByIndex(I);
            if (N < 0 || N > 254)
            {
                Error = TEXT("Enum property requires values in 0..254");
                return false;
            }
        }
        return true;
    }
    TOptional<bool> DescribeProperty(const FProperty* P, FDasDeclaredType& Out, FString& Error) const override
    {
        if (auto* E = CastField<FEnumProperty>(P))
        {
            if (!E->GetEnum() || !E->GetUnderlyingProperty() || !E->GetUnderlyingProperty()->IsA<FByteProperty>())
            {
                Error = TEXT("Only byte-sized enum storage is supported");
                return false;
            }
            Out.TypePath = E->GetEnum()->GetPathName();
            Out.EnumStorage = EDasEnumStorage::Enum8;
        }
        else if (auto* B = CastField<FByteProperty>(P); B && B->Enum)
        {
            Out.TypePath = B->Enum->GetPathName();
            Out.EnumStorage = EDasEnumStorage::Byte;
        }
        else
            return {};
        return true;
    }
    FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType& T,
                            EDasPropertyUsage) const override
    {
        auto* E = FindObject<UEnum>(nullptr, *T.TypePath);
        if (!E)
            return nullptr;
        if (DasResolvedEnumStorage(T) == EDasEnumStorage::Enum8)
        {
            auto* P = new FEnumProperty(Owner, Name);
            P->SetEnum(E);
            P->AddCppProperty(new FByteProperty(P, TEXT("UnderlyingType")));
            return P;
        }
        auto* P = new FByteProperty(Owner, Name);
        P->Enum = E;
        return P;
    }
};
TSharedRef<const IDasTypeAdapter> MakeDasEnumAdapter()
{
    return MakeShared<FDasEnumAdapter>();
}
