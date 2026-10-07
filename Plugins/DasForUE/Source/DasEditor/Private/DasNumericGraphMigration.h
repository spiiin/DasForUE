#pragma once
#include "CoreMinimal.h"
#include "EdGraph/EdGraphPin.h"
#include "DasNumericLibrary.h"
class UBlueprint;
class FDasNumericGraphMigration {
 struct FImpl;TUniquePtr<FImpl> Impl;
public:
 FDasNumericGraphMigration();~FDasNumericGraphMigration();
 bool Prepare(const TArray<UBlueprint*>& Blueprints,UClass* Root,const TMap<FName,FEdGraphPinType>& Types,EDasIntegerRule I,EDasFloatRule F,FString& Error);
 bool Apply(FString& Error);
 bool Restore(FString& Error);
};
