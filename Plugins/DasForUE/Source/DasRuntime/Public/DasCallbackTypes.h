#pragma once
#include "CoreMinimal.h"
#include "DasSchema.h"

struct FDasScalarValue;
struct FDasDeclaredType;
class FProperty;

enum class EDasCallbackUsage : uint8
{
    Event,
    Timer
};

// The callback wire name, ABI, owned snapshot and native proxy belong together.
// Function/delegate call frames have a broader policy; this is the queued surface.
struct FDasCallbackType
{
    const TCHAR* Name;
    EDasMethodSignature Signature;
    UScriptStruct* (*Snapshot)();
    bool bTimer;
    FName ValueProxy;
    FName ConstRefProxy;
    bool bDynamic = false;
};

DASRUNTIME_API TConstArrayView<FDasCallbackType> DasCallbackTypes();
DASRUNTIME_API const FDasCallbackType* FindDasCallbackType(const FString& Name);
DASRUNTIME_API const FDasCallbackType* FindDasCallbackType(EDasMethodSignature Signature,
                                                           const UScriptStruct* Snapshot = nullptr);
DASRUNTIME_API const FDasCallbackType* DasCallbackTypeForProperty(const FProperty* Property);
DASRUNTIME_API bool ValidateDasCallbackType(const FDasDeclaredType& Type, EDasCallbackUsage Usage, FString& Error);
DASRUNTIME_API bool ValidateDasCallbackTypePath(const FString& Kind, const FString& Path, FString& Error);
DASRUNTIME_API bool ValidateDasCallbackPayload(const FDasScalarValue& Value, EDasCallbackUsage Usage, FString& Error);
#if DAS_WITH_TEST_PROBES
DASRUNTIME_API bool CheckDasCallbackTypes(FString& Error);
#endif

// Lifecycle validity is checked again immediately before queued delivery.
DASRUNTIME_API bool DasCallbackObjectAlive(const UObject* Object);

// Container properties use the canonical owned carrier; ordinary structs keep their type.
DASRUNTIME_API UScriptStruct* DasCallbackSnapshotForProperty(const FProperty* Property);
