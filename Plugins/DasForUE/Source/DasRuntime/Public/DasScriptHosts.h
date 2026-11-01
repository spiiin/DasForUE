#pragma once
#include "CoreMinimal.h"
// Existing native hosts plus exact reflected parents. Generic identity lives in a cooked class contract.
DASRUNTIME_API bool DasActorNativeParent(const FString& Parent);
DASRUNTIME_API UClass* DasNativeScriptHost(const FString& Parent);
DASRUNTIME_API bool DasActorHostClass(const UClass* Class);
DASRUNTIME_API FString DasHostIdentity(const UObject* Object);
DASRUNTIME_API FString DasHostLifecycle(const UObject* Object);
DASRUNTIME_API FString* DasHostString(UObject* Object, FName Name);
DASRUNTIME_API TMap<FString, FString>* DasHostContracts(UObject* Object);
