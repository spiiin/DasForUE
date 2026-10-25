#pragma once
#include "CoreMinimal.h"
struct FDasClassSchema;
class UFunction;
// Loaded classes only: these checks never load an asset or change a class.
// A null expected identity checks validity; an empty expected identity requires a non-authored interface.
DASRUNTIME_API FString DasInterfaceStableId(const UClass* Interface);
DASRUNTIME_API bool DasInterfaceIdentityMatches(const UClass* Interface, const FString* ExpectedStableId = nullptr);
DASRUNTIME_API bool DasImplementsInterface(const UClass* Class, const UClass* Interface);
DASRUNTIME_API UFunction* DasResolveInterfaceFunction(UObject* Object, UClass* Interface, FName Name, FString& Error);
DASRUNTIME_API bool ValidateDasInterfaceContracts(const FDasClassSchema& Schema, FString& Error);
