#pragma once
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
class UFunction;
class UScriptStruct;
DASRUNTIME_API bool DasIsGeometryType(const UScriptStruct* Type);
DASRUNTIME_API bool DasGeometryFunctionAllowed(const UFunction* Function);
DASRUNTIME_API bool ValidateDasGeometryData(const UScriptStruct* Type,const void* Data,FString& Error,bool Inverse=false);
DASRUNTIME_API bool ValidateDasGeometrySnapshot(const FInstancedStruct& Value,FString& Error);
DASRUNTIME_API bool CaptureDasGeometry(const UScriptStruct* Type,const void* Source,FInstancedStruct& Out,FString& Error);
// Read-only functions only: validate before dispatch and before exposing any outputs.
DASRUNTIME_API bool ValidateDasGeometryCall(const UFunction* Function,void* Frame,FString& Error,bool Outputs=false);
