#pragma once
#include "CoreMinimal.h"
class UBlueprint;
class UDataAsset;
class DASEDITOR_API FDasDataAssetService {
public:
 // Creates a dirty Content Browser asset; never overwrites or saves user packages.
 static bool Create(UBlueprint* Blueprint,const FString& Package,UDataAsset*& Out,FString& Error);
};
