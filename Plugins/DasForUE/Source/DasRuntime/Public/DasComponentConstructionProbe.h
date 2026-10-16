#pragma once
#include "CoreMinimal.h"
class UWorld;
DASRUNTIME_API bool RunDasComponentConstructionProbe(UClass* ComponentClass,UWorld* World,FString& Report);
