#pragma once
#include "CoreMinimal.h"
class UScriptStruct;
class UFunction;
bool DasIsEventReplyType(const UScriptStruct* Type);
bool DasEventReplyFunctionAllowed(const UFunction* Function);
