#pragma once
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
class UScriptStruct;
class UFunction;
DASRUNTIME_API bool DasIsInputEventType(const UScriptStruct* Type);
DASRUNTIME_API bool DasInputEventFunctionAllowed(const UFunction* Function);
DASRUNTIME_API bool CaptureDasInputEvent(const UScriptStruct* Type,const void* Source,FInstancedStruct& Out,FString& Error);
DASRUNTIME_API bool ValidateDasInputEventData(const UScriptStruct* Type,const void* Data,FString& Error);
DASRUNTIME_API bool ValidateDasInputEventSnapshot(const FInstancedStruct& Value,FString& Error);

DASRUNTIME_API bool DasInputNavigationFunctionAllowed(const UFunction* Function);
