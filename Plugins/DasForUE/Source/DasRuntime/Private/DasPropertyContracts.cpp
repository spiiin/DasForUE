#include "DasTypeAdapters.h"
#include "DasValueKinds.h"
#include "DasContainerTypes.h"
#include "UObject/UnrealType.h"

bool DasPropertyMatchesContract(const FProperty* Property, EDasMethodSignature Kind, const FString& TypePath)
{
    if (!Property || Property->ArrayDim != 1 || Kind == EDasMethodSignature::Void || Kind == EDasMethodSignature::Frame)
        return false;
    const auto& Registry = FDasTypeAdapters::Get();
    if (Kind == EDasMethodSignature::Struct)
    {
        if (const auto* Carrier = DasContainerProperty(TypePath))
            return Carrier->SameType(Property);
        const auto* Struct = CastField<FStructProperty>(Property);
        return Struct && Struct->Struct && Struct->Struct->GetPathName() == TypePath;
    }
    if (const auto* Adapter = Registry.FindSignature(Kind))
    {
        const auto* Struct = CastField<FStructProperty>(Property);
        return Struct && Struct->Struct == Adapter->GetStruct();
    }
    FDasDeclaredType Actual;
    FString Error;
    if (!DescribeDasProperty(Property, Actual, Error) || Actual.GetKind() != DasTypeKindOf(Kind))
        return false;
    const auto* Adapter = Registry.Find(Actual);
    return Adapter && (!Adapter->UsesTypeIdentity() || Actual.TypePath == TypePath);
}
