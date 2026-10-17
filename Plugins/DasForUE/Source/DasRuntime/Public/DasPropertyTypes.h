#pragma once
#include "DasAuthoring.h"
#include "UObject/Field.h"

// Policies select existing supported surfaces, not alternate type definitions.
enum class EDasPropertyUsage : uint8 { Value,Container,DelegateParameter };
DASRUNTIME_API EDasTypeKind DasPropertyKind(const FProperty* Property);
DASRUNTIME_API bool DescribeDasProperty(const FProperty* Property,FDasDeclaredType& Out,FString& Error);
DASRUNTIME_API bool ValidateDasPropertyType(const FDasDeclaredType& Type,EDasPropertyUsage Usage,FString& Error);
// Resolve Automatic consistently for native properties and owned carriers.
DASRUNTIME_API EDasEnumStorage DasResolvedEnumStorage(const FDasDeclaredType& Type);
// Returns an unattached tree owned by the caller; nullptr leaves Owner unchanged.
// A successful caller attaches the root and performs Bind/StaticLink on its owner.
DASRUNTIME_API FProperty* CreateDasProperty(FFieldVariant Owner,FName Name,const FDasDeclaredType& Type,EDasPropertyUsage Usage,FString& Error);

DASRUNTIME_API bool CheckDasPropertyTypes(FString& Error);
