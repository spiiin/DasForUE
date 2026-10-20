#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
DASRUNTIME_API bool ValidateDasGameplayTagValue(const UScriptStruct* Type, const void* Data, FString& Error);
class IDasTypeAdapter;
// Registered with the built-in type adapters.
TSharedRef<const IDasTypeAdapter> MakeDasGameplayTagAdapter(bool bContainer);
