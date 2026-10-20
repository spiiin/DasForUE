#pragma once
#include "CoreMinimal.h"
// Read-only diagnostic surface used by targeted API-surface checks.
DASRUNTIME_API FString DasFoundationExternManifest();

// Configuration symbols generated in the loaded foundation module (read-only diagnostic copy).
DASRUNTIME_API TMap<FString, FName> DasCollisionProfileConstants();
