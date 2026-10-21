#pragma once
#include "CoreMinimal.h"
// Editor build-agent fixture: generation and one focused, strict native-execution check.
DASRUNTIME_API bool GenerateDasUEAotProbe(const FString& Output, FString& Error);
DASRUNTIME_API bool RunDasUEAotProbe(FString& Report);
