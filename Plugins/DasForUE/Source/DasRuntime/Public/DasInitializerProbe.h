#pragma once
#include "CoreMinimal.h"
DASRUNTIME_API bool RunDasInitializerProbe(UObject* Actor,UObject* Component,FString& Report);
DASRUNTIME_API bool RunDasInitializerLifetimeProbe(UClass* Target,UClass* Inherited,UClass* Skipped,UClass* Failure,FString& Report);
