#pragma once
#include "CoreMinimal.h"
struct FDasClassSchema;
class UFunction;
DASRUNTIME_API bool ValidateDasInterfaceContracts(const FDasClassSchema& Schema,FString& Error);
