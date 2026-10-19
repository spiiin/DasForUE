#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
DASRUNTIME_API bool DasIsGameplayTagType(const UScriptStruct* Type);
DASRUNTIME_API bool ValidateDasGameplayTagValue(const UScriptStruct* Type,const void* Data,FString& Error);
