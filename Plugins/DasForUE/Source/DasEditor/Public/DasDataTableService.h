#pragma once
#include "CoreMinimal.h"
class UScriptStruct;
class UDataTable;
class DASEDITOR_API FDasDataTableService {
public:
 // Validate/import into a transient table first. Create a new dirty asset only on success.
 // Never overwrites existing packages or saves assets/types/project settings.
 static bool CreateFromJson(UScriptStruct* RowType,const FString& Json,const FString& Package,UDataTable*& Out,FString& Error);
};
