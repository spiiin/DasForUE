#pragma once
#include "CoreMinimal.h"
// Fixed, trusted host registry. Reflection accesses only common native bridge storage.
DASRUNTIME_API bool DasActorNativeParent(const FString& Parent);
DASRUNTIME_API UClass* DasNativeScriptHost(const FString& Parent);
DASRUNTIME_API bool DasActorHostClass(const UClass* Class);
DASRUNTIME_API FString DasHostIdentity(const UObject* Object);
DASRUNTIME_API FString DasHostLifecycle(const UObject* Object);
DASRUNTIME_API FString* DasHostString(UObject* Object,FName Name);
DASRUNTIME_API TMap<FString,FString>* DasHostContracts(UObject* Object);
