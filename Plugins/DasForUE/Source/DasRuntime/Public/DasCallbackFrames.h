#pragma once
#include "DasAuthoring.h"
#include "DasCallFrame.h"

// Private argument tuples represented by an owned reflected snapshot. The
// signature path describes transport fields, never daScript object layout.
DASRUNTIME_API UScriptStruct* DasCallbackFrame(const TArray<FDasDeclaredType>& Fields, FString& Error);
DASRUNTIME_API UScriptStruct* FindDasCallbackFrame(const FString& Path);
DASRUNTIME_API UFunction* DasCallbackFrameBuilder(UScriptStruct* Frame);
DASRUNTIME_API bool ValidateDasCallbackFrameSource(UFunction* Signature, UScriptStruct* Frame, FString& Error);
DASRUNTIME_API bool CaptureDasCallbackFrame(UFunction* Signature, const void* Memory, UScriptStruct* Frame,
                                            FDasScalarValue& Out, FString& Error);
DASRUNTIME_API void ResetDasCallbackFrames();

DASRUNTIME_API bool CheckDasCallbackFrames(FString& Error);
