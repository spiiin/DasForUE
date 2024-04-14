#pragma once

#include "CoreMinimal.h"

static UPackage* AnyPackage();
static const char* FStringToCStr(const FString& str);
static const wchar_t* FStringToWChar(const FString& str);
static void BroadcastAssets();