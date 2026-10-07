#pragma once
#include "CoreMinimal.h"
class FDasTypeService {
public:
 static bool Apply(const FString& Script,const FString& Folder,TArray<UObject*>& Out,FString& Error);
};
