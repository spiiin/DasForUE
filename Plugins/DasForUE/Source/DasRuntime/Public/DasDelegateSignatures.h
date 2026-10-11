#pragma once
#include "CoreMinimal.h"
struct FDasDeclaredMethod;
class UFunction;
// Signature functions are rooted by the caller, never registered as callable methods.
DASRUNTIME_API UFunction* MakeDasDelegateSignature(const FDasDeclaredMethod& Method,bool Multicast,UObject* Outer,FName Name,FString& Error);

DASRUNTIME_API class UFunction* MakeDasSingleDelegateSignature(const FString& Wire,FString& Error);
