#pragma once
#include "CoreMinimal.h"
class UBlueprint;struct FDasAuthoringModule;
bool ApplyDasLibrary(const FDasAuthoringModule& Model,const FString& Package,const FString& Id,UBlueprint*& Out,FString& Error);
