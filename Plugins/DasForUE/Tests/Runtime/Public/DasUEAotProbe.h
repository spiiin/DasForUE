#pragma once
#include "CoreMinimal.h"
// Editor build-agent fixture: generation and one focused, strict native-execution check.
// Script: the AOT source under Content/Das/Scripts; its native code goes to Output (a Private/Generated/*.cpp).
DASRUNTIME_API bool GenerateDasUEAotProbe(const FString& Output, FString& Error,
                                          const FString& Script = TEXT("aot_ue_guard.das"));
DASRUNTIME_API bool RunDasUEAotProbe(FString& Report);
