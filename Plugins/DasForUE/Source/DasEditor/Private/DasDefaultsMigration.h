#pragma once
#include "CoreMinimal.h"
class UBlueprint;struct FDasAuthoringModule;
bool DasMergeDefaults(UBlueprint* Blueprint,const FDasAuthoringModule& Next,const FString& ClassId,FString& Error);
#if WITH_DEV_AUTOMATION_TESTS
void DasFailDefaultsMergeForTesting(int32 Stage);
#endif

// Stage inherited literals against the actual parent layout before asset creation.
bool DasInitializeInheritedDefaults(UClass* Class,const FDasAuthoringModule& Model,const FString& Id,FString& Error,bool ValidateOnly=false);

bool DasInitializeNativeDefaults(UClass* Class,const FDasAuthoringModule& Model,const FString& Id,FString& Error,bool ValidateOnly=false);
