#pragma once
#include "DasAuthoring.h"
#include "EdGraph/EdGraphPin.h"

// Resolve authoring-only identities before creating reflection. Native callers
// omit this callback and supply loaded paths directly.
using FDasPinTypeResolver = TFunction<UObject*(EDasTypeKind,const FString&)>;

// Both APIs leave Out unchanged on failure. Parameter direction/constness and
// authored logical IDs remain separate from the pin's value type.
bool DasTypeToPin(const FDasDeclaredType& Type,FEdGraphPinType& Out,FString& Error,const FDasPinTypeResolver& Resolver = {});
bool DescribeDasPin(const FEdGraphPinType& Pin,FDasDeclaredType& Out,FString& Error);

// Bridge for the existing class/method schema, including carrier paths and math
// signatures. Delegate pin ownership is handled by the signature asset service.
bool DasSchemaType(EDasMethodSignature Kind,const FString& Path,FDasDeclaredType& Out,FString& Error);
FEdGraphPinType DasTypePinChecked(const FDasDeclaredType& Type,const FDasPinTypeResolver& Resolver = {});
FEdGraphPinType DasSchemaPinChecked(EDasMethodSignature Kind,const FString& Path = {});
