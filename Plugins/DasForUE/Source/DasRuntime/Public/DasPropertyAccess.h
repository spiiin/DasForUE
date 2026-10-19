#pragma once
#include "CoreMinimal.h"
#include "DasPropertyValues.h"

// Value contract, not cached FProperty/UFunction pointers. Stored by generated registries.
struct DASRUNTIME_API FDasPropertyAccess
{
    bool NativeGetter = false, NativeSetter = false, GetterUnsafe = false, SetterUnsafe = false;
    FName Getter, Setter;
    // Set when ValidateDasPropertyAccess already passed for this property; owner checks then skip it.
    bool bValidated = false;
};
DASRUNTIME_API bool DescribeDasPropertyAccess(FProperty* Property, FDasPropertyAccess& Out, FString& Error);
DASRUNTIME_API bool ValidateDasPropertyAccess(FProperty* Property, const FDasPropertyAccess& Access, FString& Error);
DASRUNTIME_API bool ReadDasObjectProperty(FProperty* Property, UObject* Owner, const FDasPropertyAccess& Access,
                                          bool Construction, FDasScalarValue& Out, FString& Error);
DASRUNTIME_API bool WriteDasObjectProperty(FProperty* Property, UObject* Owner, const FDasPropertyAccess& Access,
                                           bool Construction, const FDasScalarValue& Value, FString& Error);
