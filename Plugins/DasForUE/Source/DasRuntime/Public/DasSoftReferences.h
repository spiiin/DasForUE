#pragma once
#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include "UObject/SoftObjectPath.h"
// Soft properties use the shared owned-snapshot ABI, retaining native soft pins.
DASRUNTIME_API UScriptStruct* DasSoftCarrier(UClass* Base,bool ClassReference,FString& Error);
DASRUNTIME_API UScriptStruct* DasSoftCarrierForProperty(const FProperty* Property);
DASRUNTIME_API FSoftObjectProperty* DasSoftProperty(const FString& CarrierPath);
DASRUNTIME_API FProperty* CreateDasSoftProperty(FFieldVariant Owner,FName Name,const FSoftObjectProperty* Prototype);
DASRUNTIME_API bool ValidateDasSoftPath(const FString& Text,FString& Error);
DASRUNTIME_API bool ValidateDasSoftValue(const FSoftObjectProperty* Property,const void* Memory,FString& Error);
DASRUNTIME_API bool DasSoftTargetMatches(const UObject* Object,const UClass* Base,bool ClassReference);
// Widen a checked immutable snapshot without resolving/loading its asset.
struct FInstancedStruct;
DASRUNTIME_API bool UpcastDasSoftValue(const FInstancedStruct& Source,const FString& TargetPath,FInstancedStruct& Out,FString& Error);
DASRUNTIME_API bool CheckDasSoftUpcasts(FString& Error);
DASRUNTIME_API void ResetDasSoftCarriers();

DASRUNTIME_API bool CheckDasSoftReferences(FString& Error);
