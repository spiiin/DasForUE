#pragma once
#include "CoreMinimal.h"
class UScriptStruct;
class UFunction;
bool DasIsPaintContextType(const UScriptStruct* Type);
bool DasPaintFunctionAllowed(const UFunction* Function);
