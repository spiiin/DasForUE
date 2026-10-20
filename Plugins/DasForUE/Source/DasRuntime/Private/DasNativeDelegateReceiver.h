#pragma once
#include "CoreMinimal.h"
#include "UObject/Class.h"
UFunction* MakeDasNativeDelegateReceiver(UClass* Class, UFunction* Signature, FNativeFuncPtr Thunk, FString& Error);
void ResetDasNativeDelegateReceivers(UClass* Class);
