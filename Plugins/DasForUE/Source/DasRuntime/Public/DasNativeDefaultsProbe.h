#pragma once
#include "CoreMinimal.h"
class UWorld;
DASRUNTIME_API bool RunDasNativeDefaultsProbe(UWorld* World,FString& Report);

// Scoped by the native-array regression probe only; disabled during normal use.
DASRUNTIME_API bool SetDasNativeArraySelfProbeEnabled(bool Enabled);
DASRUNTIME_API bool IsDasNativeArraySelfProbeEnabled();
